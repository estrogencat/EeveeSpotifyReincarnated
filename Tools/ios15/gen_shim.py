import json,os,sys

DATA_SUF=("Mn","Mc","MD","Mp","MQ","WV","Tq","Tl","Tb","WC","MV","N")
FUNC_SUF=("Ma","Tj","vgZ","vg","vsZ","vs","vMZ","vM","FZ","F","fC","fc","ig")

def kind(n):
    if not n.startswith("_$s"):
        if n.startswith(("_swift_","_objc_")): return "func"
        return "data"
    if n.endswith("Tu"): return "data"
    for s in DATA_SUF:
        if n.endswith(s): return "data"
    for s in FUNC_SUF:
        if n.endswith(s): return "func"
    return "data"

def throws(n):
    return "KF" in n[-24:] or "KFTj" in n

r=json.load(open("hardsyms.json"))
hard=sorted({n for b in r for n,l,k,w in r[b] if k=="hard" and not w})

impl=set()
if os.path.exists("work/impl_syms.txt"):
    impl={l.strip() for l in open("work/impl_syms.txt") if l.strip() and not l.startswith("#")}

stubs=[n for n in hard if kind(n)=="func" and n not in impl]

out=[]
out.append(".text")
for i,n in enumerate(stubs):
    out.append(".globl %s"%n)
    out.append(".p2align 2")
    out.append("%s:"%n)
    out.append("  stp x29, x30, [sp, #-16]!")
    out.append("  mov x29, sp")
    out.append("  adrp x0, Ls%d@PAGE"%i)
    out.append("  add x0, x0, Ls%d@PAGEOFF"%i)
    out.append("  bl _shim_unimpl")
    out.append("  mov x0, #0")
    out.append("  mov x1, #0")
    out.append("  fmov d0, xzr")
    out.append("  fmov d1, xzr")
    if throws(n): out.append("  mov x21, #0")
    out.append("  ldp x29, x30, [sp], #16")
    out.append("  ret")
out.append(".section __TEXT,__cstring,cstring_literals")
for i,n in enumerate(stubs):
    out.append("Ls%d:"%i)
    out.append('  .asciz "%s"'%n)
open("work/gen.s","w",newline="\n").write("\n".join(out)+"\n")
print("generated stubs",len(stubs),"implemented",len(impl),"data left null",len(hard)-len(stubs)-len(impl&set(hard)))
