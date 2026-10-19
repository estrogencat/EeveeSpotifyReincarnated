import sys
import lief
from capstone import Cs, CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN

lief.logging.disable()
path = sys.argv[1] if len(sys.argv) > 1 else "work/libshim.dylib"
f = lief.MachO.parse(path).at(0)
data = open(path, "rb").read()
text = [s for s in f.sections if s.name == "__text"][0]
md = Cs(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN)
body = data[text.offset:text.offset + text.size]

PAC = ("xpac", "pac", "aut", "braa", "brab", "blraa", "blrab", "retaa", "retab")
# armv8.1 lse and armv8.3 rcpc, neither of which the a10 has
LSE = ("cas", "swp", "ldadd", "ldclr", "ldeor", "ldset", "ldsmax", "ldsmin",
       "ldumax", "ldumin", "stadd", "stclr", "steor", "stset", "stsmax",
       "stsmin", "stumax", "stumin")
RCPC = ("ldapr", "ldapur", "stlur")

def walk(buf, base):
    """capstone stops at the first undecodable word; data sits in __text, so restart"""
    pos = 0
    while pos < len(buf):
        n = 0
        for i in md.disasm(buf[pos:], base + pos):
            n += 1
            yield i
        pos += max(n * 4, 4)


bad = []
ndec = 0
for i in walk(body, text.virtual_address):
    ndec += 1
    m = i.mnemonic
    if m.startswith(PAC):
        bad.append((i.address, m, i.op_str, "pac"))
    elif m.startswith(RCPC):
        bad.append((i.address, m, i.op_str, "rcpc armv8.3"))
    elif m.startswith(LSE):
        bad.append((i.address, m, i.op_str, "lse armv8.1"))

if bad:
    print(f"ILLEGAL INSTRUCTIONS IN {path} - not available on the a10:")
    for addr, m, ops, why in bad:
        print(f"  +{addr:#x}  {m} {ops}   [{why}]")
    sys.exit(1)
print(f"instruction scan clean ({ndec} of {len(body) // 4} words in __text)")
