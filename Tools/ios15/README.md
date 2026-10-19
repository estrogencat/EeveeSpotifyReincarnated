# iOS 15 patch toolchain

Patches a decrypted Spotify IPA so it runs on **iOS 15**. Spotify 9.1.90 declares a
minimum of iOS 16.1 and calls a few hundred symbols that iOS 15 does not have, so the
patcher rewrites its imports and ships a shim dylib that implements or stubs the missing
ones.

This does **nothing else** — no EeveeSpotify, no zxPluginsInject, no signing. It exists so
the shim can be tested from CI instead of building locally and installing over ssh.

Run it from `.github/workflows/zzz-EXPERIMENTAL-ios15-patch-only.yml`, or by hand:

```bash
Tools/ios15/ios15-patch.sh <input.ipa> <output.ipa> [workdir]
```

Needs `python3` with `lief`, `capstone` and `ziglang` (all from pip). It does **not** need
Xcode or macOS — the shim is compiled by `python -m ziglang cc`, so an ubuntu runner is fine.

## The output is not signed

Install it with TrollStore, which signs at install time. It is only useful on iOS 15; on a
supported device use a normal build.

## What runs, in order

| step | what it does |
|---|---|
| `unpack_ios15db.py` | expands `ios15-symbols.txt.xz` into `exports15.pkl`, `reexports15.pkl`, `cachepaths.pkl` |
| `gen_bindmap.py` | `work/bindmap_Spotify.pkl`: bound address → imported symbol |
| `hard_syms.py` | classifies every import against iOS 15 → `hardsyms.json` |
| `build_shim.sh` | `gen_xj_patches.py`, `gen_shim.py`, `ziglang cc`, `fixplat.py`, `check_pac.py`, `dump_shim_exports.py` → `work/libshim.dylib` |
| `patch.py` | rewrites the import table, applies the typeref patches, sets `MinimumOSVersion`, repacks |
| `check_macho.py` | refuses to ship a binary whose load commands dyld would reject |

A good run prints `294 xj rewrites`, `0 keypath fixups`, `instruction scan clean` and
`146/146 commands parse cleanly` for Spotify 9.1.90.

## The symbol database

`patch.py` and `hard_syms.py` need to know what iOS 15 exports. That comes from the iOS 15
dyld shared cache, which is 2.2 GB, and the pickle derived from it is 84 MB — neither belongs
in a repo. So the three pickles are shipped as one **7 MB** xz text blob,
`ios15-symbols.txt.xz` (1,417,404 symbols across 2,351 libraries; symbol names share long
prefixes, so xz does very well). `unpack_ios15db.py` restores it, and the round-trip is
verified exact.

To regenerate it you need the cache (`dsc15/dyld_shared_cache_arm64` plus `.1`–`.5`) and the
upstream tooling:

```bash
python dump_exports.py      # exports15.pkl, reexports15.pkl
python dump_cachepaths.py   # cachepaths.pkl
python pack_ios15db.py      # -> ios15-symbols.txt.xz
python pack_ios15db.py --verify
```

Only needed for a different iOS version. iOS 15.7.7 (19H357) is what this blob is from.

## `check_pac.py` is not optional

The iPhone 7 is an A10: no pointer authentication, no armv8.1 LSE, no armv8.3 RCPC. Harmless
C can lower to any of those (`__atomic_load_n` acquire → `ldapr`,
`__sync_bool_compare_and_swap` → `casal`, `__builtin_return_address` → `xpaci`) and the shim
then faults inside itself. `check_pac.py` scans `__text` and fails the build, and
`build_shim.sh` passes `-mcpu=apple_a7`. Do not remove either.

## Keeping this in sync

The source of truth for the shim is the separate `spotify-ios15` project; the files here are
a vendored copy. `ios15-patch.sh` prints a sha256 fingerprint of every vendored file at the
start of a run, so a build log identifies exactly which toolchain produced an IPA. If you
change `work/impl.c` upstream, re-copy it here.

Verified: this toolchain produces a main binary **byte-identical** to the locally built
`out/Spotify15.ipa` that runs on the test device.
