"""unpack ios15-symbols.txt.xz into the three pickles the ios 15 tooling expects.

patch.py and hard_syms.py want exports15.pkl (library -> exported symbol names),
reexports15.pkl (library -> reexported libraries) and cachepaths.pkl (libraries
present in the cache). Those are derived from the ios 15 dyld shared cache, which
is 2.2 GB and cannot live in a repo; the exports pickle alone is 84 MB. So the
three are shipped as one 7 MB xz text blob and expanded here.

Regenerate the blob with pack_ios15db.py on a machine that has dsc15/.
"""
import lzma
import pickle
import sys

SRC = sys.argv[1] if len(sys.argv) > 1 else "ios15-symbols.txt.xz"

with lzma.open(SRC, "rt", encoding="utf-8") as fh:
    lines = fh.read().split("\n")

i = 0
if lines[i] != "#v1":
    raise SystemExit("unexpected header %r in %s" % (lines[i], SRC))
i += 1

n = int(lines[i].split()[1]); i += 1
paths = set(lines[i:i + n]); i += n

n = int(lines[i].split()[1]); i += 1
rex = {}
for _ in range(n):
    parts = lines[i].split("\t"); i += 1
    rex[parts[0]] = [p for p in parts[1:] if p]

n = int(lines[i].split()[1]); i += 1
ex = {}
cur = None
while i < len(lines):
    line = lines[i]; i += 1
    if not line:
        continue
    if line.startswith("@"):
        cur = line[1:]
        ex[cur] = set()
    elif cur is not None:
        ex[cur].add(line)

if len(ex) != n:
    raise SystemExit("expected %d libraries, decoded %d" % (n, len(ex)))

pickle.dump(ex, open("exports15.pkl", "wb"))
pickle.dump(rex, open("reexports15.pkl", "wb"))
pickle.dump(paths, open("cachepaths.pkl", "wb"))

print("unpacked %d libraries, %d symbols, %d reexport entries, %d cache paths"
      % (len(ex), sum(len(v) for v in ex.values()), len(rex), len(paths)))
