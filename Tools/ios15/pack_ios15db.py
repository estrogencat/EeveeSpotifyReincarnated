"""pack the ios 15 symbol database into one portable file for CI.

patch.py needs three pickles derived from the ios 15 dyld shared cache:
exports15.pkl (lib -> exported symbol names), reexports15.pkl (lib -> reexported
libs) and cachepaths.pkl (libs present in the cache). The cache itself is 2.2 GB
and the exports pickle is 84 MB, neither of which belongs in a git repo, so this
writes a single xz-compressed text blob instead. symbol names share long prefixes,
so xz gets it down by an order of magnitude.

  python pack_ios15db.py            # writes ios15-symbols.txt.xz
  python pack_ios15db.py --verify   # unpack and prove it round-trips exactly
"""
import lzma
import pickle
import sys

OUT = "ios15-symbols.txt.xz"


def pack():
    ex = pickle.load(open("exports15.pkl", "rb"))
    rex = pickle.load(open("reexports15.pkl", "rb"))
    paths = pickle.load(open("cachepaths.pkl", "rb"))

    buf = []
    buf.append("#v1\n")
    buf.append("#paths %d\n" % len(paths))
    for p in sorted(paths):
        buf.append(p + "\n")
    buf.append("#reexports %d\n" % len(rex))
    for lib in sorted(rex):
        buf.append("%s\t%s\n" % (lib, "\t".join(rex[lib])))
    buf.append("#exports %d\n" % len(ex))
    for lib in sorted(ex):
        buf.append("@%s\n" % lib)
        for s in sorted(ex[lib]):
            buf.append(s + "\n")
    text = "".join(buf).encode("utf-8")

    # preset 9 with a large dictionary; the names are extremely repetitive
    filters = [{"id": lzma.FILTER_LZMA2, "preset": 9 | lzma.PRESET_EXTREME}]
    blob = lzma.compress(text, format=lzma.FORMAT_XZ, filters=filters)
    open(OUT, "wb").write(blob)
    print("libs=%d reexport-entries=%d cachepaths=%d symbols=%d"
          % (len(ex), len(rex), len(paths), sum(len(v) for v in ex.values())))
    print("plain text %.1f MB -> %s %.1f MB"
          % (len(text) / 1e6, OUT, len(blob) / 1e6))


def unpack(path=OUT):
    """mirror of pack(); Scripts/ios15-unpack-symbols.py uses the same format"""
    data = lzma.open(path, "rt", encoding="utf-8").read().split("\n")
    i = 0
    assert data[i] == "#v1", data[i]
    i += 1
    n = int(data[i].split()[1]); i += 1
    paths = set(data[i:i + n]); i += n
    n = int(data[i].split()[1]); i += 1
    rex = {}
    for _ in range(n):
        parts = data[i].split("\t"); i += 1
        rex[parts[0]] = [p for p in parts[1:] if p]
    n = int(data[i].split()[1]); i += 1
    ex = {}
    cur = None
    while i < len(data):
        line = data[i]; i += 1
        if not line:
            continue
        if line.startswith("@"):
            cur = line[1:]
            ex[cur] = set()
        elif cur is not None:
            ex[cur].add(line)
    return ex, rex, paths


def verify():
    ex, rex, paths = unpack()
    ex0 = pickle.load(open("exports15.pkl", "rb"))
    rex0 = pickle.load(open("reexports15.pkl", "rb"))
    paths0 = pickle.load(open("cachepaths.pkl", "rb"))
    ok = True
    if set(ex) != set(ex0):
        print("FAIL: library sets differ"); ok = False
    else:
        bad = [l for l in ex0 if ex[l] != set(ex0[l])]
        if bad:
            print("FAIL: %d libraries have different symbol sets, e.g. %s" % (len(bad), bad[:3]))
            ok = False
    if {k: list(v) for k, v in rex.items()} != {k: list(v) for k, v in rex0.items()}:
        print("FAIL: reexports differ"); ok = False
    if paths != set(paths0):
        print("FAIL: cachepaths differ"); ok = False
    print("round-trip exact" if ok else "ROUND-TRIP MISMATCH")
    return 0 if ok else 1


if __name__ == "__main__":
    if "--verify" in sys.argv:
        sys.exit(verify())
    pack()
