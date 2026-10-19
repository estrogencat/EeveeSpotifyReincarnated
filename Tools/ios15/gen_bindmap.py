"""write work/bindmap_<name>.pkl: bound address -> imported symbol name.

shape_proto.py loads this with a bare pickle.load and has no fallback, so it has
to exist before gen_xj_patches.py runs. Locally disasm.py happens to build it as
a side effect; in a clean checkout nothing does, hence this script.
"""
import os
import pickle
import sys

import lief

lief.logging.disable()

BIN = sys.argv[1] if len(sys.argv) > 1 else "ipa/Payload/Spotify.app/Spotify"
out = "work/bindmap_" + os.path.basename(BIN) + ".pkl"

f = lief.MachO.parse(BIN).at(0)
bm = {}
for x in f.bindings:
    if x.has_symbol:
        bm[x.address] = x.symbol.name

os.makedirs("work", exist_ok=True)
pickle.dump(bm, open(out, "wb"))
print("wrote %s with %d bindings" % (out, len(bm)))
