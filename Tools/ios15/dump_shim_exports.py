import lief
lief.logging.disable()
f=lief.MachO.parse("work/libshim.dylib").at(0)
ex=sorted({s.name for s in f.symbols
           if s.name and s.has_export_info})
open("work/shim_exports.txt","w",newline="\n").write("\n".join(ex)+"\n")
print("shim exports",len(ex))
