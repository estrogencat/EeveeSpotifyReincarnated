#!/usr/bin/env bash
# Patch a decrypted Spotify IPA to run on iOS 15. Nothing else - no EeveeSpotify,
# no zxPluginsInject, no re-signing (TrollStore signs at install time).
#
#   Tools/ios15/ios15-patch.sh <input.ipa> <output.ipa> [workdir]
#
# The toolchain's scripts all use relative paths rooted at the project directory
# and expect the app at ipa/Payload/Spotify.app, so everything is copied into a
# scratch workdir and run with that as the cwd.
#
# Needs: python3 with lief, capstone and ziglang (pip). No Xcode, no macOS - the
# shim is built by `python -m ziglang cc`, so a linux runner is fine.
set -euo pipefail

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ "$#" -lt 2 ]; then
  echo "usage: $0 <input.ipa> <output.ipa> [workdir]" >&2
  exit 2
fi

IN_IPA="$(cd "$(dirname "$1")" && pwd)/$(basename "$1")"
OUT_ARG="$2"
mkdir -p "$(dirname "$OUT_ARG")"
OUT_IPA="$(cd "$(dirname "$OUT_ARG")" && pwd)/$(basename "$OUT_ARG")"
WORK="${3:-${RUNNER_TEMP:-/tmp}/ios15-build}"

[ -f "$IN_IPA" ] || { echo "::error::input ipa not found: $IN_IPA" >&2; exit 1; }

echo "=== toolchain fingerprint (so a build log identifies what produced the ipa)"
( cd "$here" && for f in patch.py hard_syms.py gen_shim.py gen_xj_patches.py \
    scan_xj.py shape_proto.py fixplat.py check_pac.py dump_shim_exports.py \
    gen_bindmap.py unpack_ios15db.py build_shim.sh ios15-symbols.txt.xz \
    work/impl.c work/tol.c work/shim.s work/impl_syms.txt; do
    printf '%s  %s\n' "$(sha256sum "$f" | cut -c1-16)" "$f"
  done )

echo "=== staging $WORK"
rm -rf "$WORK"
mkdir -p "$WORK"
cp -R "$here/." "$WORK/"
rm -f "$WORK/pack_ios15db.py"   # only useful where dsc15/ exists
cd "$WORK"

echo "=== unzip input ipa"
mkdir -p ipa
unzip -q "$IN_IPA" -d ipa
APP=ipa/Payload/Spotify.app
[ -d "$APP" ] || { echo "::error::no Payload/Spotify.app in the input ipa"; ls -R ipa | head -20; exit 1; }
for f in "$APP/Spotify" \
         "$APP/Frameworks/SpotifyShared.framework/SpotifyShared" \
         "$APP/Frameworks/MetaSmartGlassesKit.framework/MetaSmartGlassesKit"; do
  [ -f "$f" ] || { echo "::error::expected binary missing: $f"; exit 1; }
done

echo "=== check the input really is decrypted"
python3 - <<'PYEOF'
import struct
d = open("ipa/Payload/Spotify.app/Spotify", "rb").read(0x40000)
magic, _cpu, _sub, _ft, ncmds, _sz, _fl, _r = struct.unpack_from("<8I", d, 0)
if magic != 0xFEEDFACF:
    raise SystemExit("::error::main binary is not a thin arm64 mach-o (magic %#x). "
                     "A fat/universal or encrypted App Store ipa will not work." % magic)
off = 32
for _ in range(ncmds):
    cmd, cs = struct.unpack_from("<II", d, off)
    if cmd in (0x21, 0x2C):  # LC_ENCRYPTION_INFO[_64]
        cryptid = struct.unpack_from("<I", d, off + 20)[0]
        if cryptid:
            raise SystemExit("::error::cryptid=%d - this ipa is still ENCRYPTED. "
                             "Use a decrypted dump." % cryptid)
    off += cs
print("input looks like a decrypted thin arm64 binary")
PYEOF

echo "=== unpack the ios 15 symbol database"
python3 unpack_ios15db.py ios15-symbols.txt.xz

echo "=== bindmap"
python3 gen_bindmap.py ipa/Payload/Spotify.app/Spotify

echo "=== classify imports against ios 15"
python3 hard_syms.py

echo "=== build the shim"
bash build_shim.sh

echo "=== patch and repack"
python3 patch.py

[ -f out/Spotify15.ipa ] || { echo "::error::patch.py did not produce out/Spotify15.ipa"; exit 1; }
cp out/Spotify15.ipa "$OUT_IPA"
echo "=== wrote $OUT_IPA ($(du -h "$OUT_IPA" | cut -f1))"
