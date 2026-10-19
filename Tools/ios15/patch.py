import struct,pickle,shutil,os,plistlib,zipfile,sys,re,json
SRC="ipa"; DST="work/app"
ex=pickle.load(open("exports15.pkl","rb"))
rex=pickle.load(open("reexports15.pkl","rb"))
import collections
where=collections.defaultdict(list)
for l,e in ex.items():
    for sy in e: where[sy].append(l)
def has(lib,sym,seen=None):
    seen=seen if seen is not None else set()
    if lib in seen: return False
    seen.add(lib)
    if sym in ex.get(lib,()): return True
    return any(has(r,sym,seen) for r in rex.get(lib,()))
try:
    paths=pickle.load(open("cachepaths.pkl","rb"))
except Exception:
    import subprocess
    subprocess.run([sys.executable,"dump_cachepaths.py"],check=True)
    paths=pickle.load(open("cachepaths.pkl","rb"))
SHIM_SYMS={l.strip() for l in open("work/shim_exports.txt") if l.strip()}
if os.path.exists(DST): shutil.rmtree(DST)
shutil.copytree(SRC+"/Payload/Spotify.app",DST)
for d in("PlugIns","Watch","_CodeSignature"): shutil.rmtree(DST+"/"+d,ignore_errors=True)
shutil.copy("work/libshim.dylib",DST+"/Frameworks/libshim.dylib")

def patch_macho(p,shim_slot=True):
    d=bytearray(open(p,'rb').read())
    magic,cpu,_,ft,nc,sz,fl,_r=struct.unpack_from("<8I",d,0)
    assert magic==0xfeedfacf
    off=32;libs=[];fix=None
    for i in range(nc):
        cmd,cs=struct.unpack_from("<II",d,off)
        if cmd==0x32:
            plat,minos,sdk=struct.unpack_from("<3I",d,off+8)
            if plat==2 and minos>0xF0000: struct.pack_into("<I",d,off+12,0xF0000)
        if cmd in(0xC,0x80000018,0x8000001F,0x20):
            no=struct.unpack_from("<I",d,off+8)[0]
            libs.append((off,cmd,cs,d[off+no:off+cs].split(b"\0")[0].decode()))
        if cmd==0x80000034: fix=struct.unpack_from("<II",d,off+8)
        off+=cs
    fo,fs=fix
    v,so,imo,syo,ic,ifmt,sfmt=struct.unpack_from("<7I",d,fo)
    assert ifmt==1 and sfmt==0
    used=[0]*(len(libs)+1)
    ent=[]
    for i in range(ic):
        w=struct.unpack_from("<I",d,fo+imo+4*i)[0]
        ordv=w&0xff;weak=(w>>8)&1;no=w>>9
        e=d.find(b"\0",fo+syo+no);name=bytes(d[fo+syo+no:e]).decode()
        ent.append((i,ordv,weak,name));
        if 0<ordv<=len(libs): used[ordv]+=1
    absent=set()
    for idx,(off,cmd,cs,path) in enumerate(libs,1):
        if path.startswith(("/System","/usr/lib")) and path not in paths and path not in ex and path not in rex:
            absent.add(idx)
            if cmd==0xC: struct.pack_into("<I",d,off,0x80000018)
    freeslots=[idx for idx,(off,cmd,cs,path) in enumerate(libs,1) if cmd==0x80000018 and used[idx]==0 and cs>=64 and "libswift" in path]
    def setpath(idx,newpath):
        off,cmd,cs,path=libs[idx-1]
        nb=newpath.encode()+bytes(1)
        assert 24+len(nb)<=cs
        d[off+24:off+cs]=nb+bytes(1)*(cs-24-len(nb))
        libs[idx-1]=(off,cmd,cs,newpath)
    slots={}
    def ordinal_for(target):
        if target in slots: return slots[target]
        for idx,(off,cmd,cs,path) in enumerate(libs,1):
            if path==target and idx not in absent: slots[target]=idx;return idx
        idx=freeslots.pop(0);setpath(idx,target);slots[target]=idx;return idx
    FORCE={"_swift_getKeyPath","_swift_dynamicCast","_swift_dynamicCastClass","_swift_dynamicCastMetatype","_swift_getTypeByMangledNameInContext","_swift_getTypeByMangledNameInEnvironment","_swift_getWitnessTable","_swift_getAssociatedTypeWitness","_swift_getGenericMetadata","_swift_getAssociatedConformanceWitness","_swift_allocateGenericClassMetadata","_swift_initClassMetadata2","_swift_allocateGenericValueMetadata","_swift_updateClassMetadata2","_swift_getSingletonMetadata"}
    nweak=nshim=nre=0
    if shim_slot: SH=ordinal_for("@rpath/libshim.dylib")
    PREF=["libswiftFoundation.dylib","Foundation","libswiftCoreGraphics.dylib"]
    for i,ordv,weak,name in ent:
        if not (0<ordv<=len(libs)): continue
        path=libs[ordv-1][3]
        if name in FORCE and shim_slot:
            w=struct.unpack_from("<I",d,fo+imo+4*i)[0]
            struct.pack_into("<I",d,fo+imo+4*i,(w&~0xff&~(1<<8))|SH);nshim+=1
            continue
        if (name in SHIM_SYMS and shim_slot and not path.startswith("@")
                and (ordv in absent or not has(path,name))):
            w=struct.unpack_from("<I",d,fo+imo+4*i)[0]
            struct.pack_into("<I",d,fo+imo+4*i,(w&~0xff&~(1<<8))|SH);nshim+=1
            continue
        if ordv in absent:
            if not weak:
                w0=struct.unpack_from('<I',d,fo+imo+4*i)[0];struct.pack_into('<I',d,fo+imo+4*i,w0|(1<<8));nweak+=1
            continue
        if path.startswith("@"): continue
        if has(path,name): continue
        w=struct.unpack_from("<I",d,fo+imo+4*i)[0]
        if name in SHIM_SYMS and shim_slot:
            w=(w&~0xff)|SH;nshim+=1
        elif where.get(name):
            cands=where[name]
            tgt=sorted(cands,key=lambda c:(0 if c.split("/")[-1] in PREF else 1,len(c)))[0]
            w=(w&~0xff)|ordinal_for(tgt);nre+=1
        elif not weak:
            w|=1<<8;nweak+=1
        struct.pack_into("<I",d,fo+imo+4*i,w)
    print(os.path.basename(p),"absent libs",len(absent),"shim",nshim,"rebound",nre,"weakened",nweak,"slots",slots)
    open(p,'wb').write(d)

KPNAMEFIX=[]

def patch_names(p):
    d=bytearray(open(p,'rb').read())
    for fa,na in KPNAMEFIX:
        struct.pack_into("<i",d,fa-0x100000000,(na|1)-fa)
    n=0
    if os.path.exists("xj_patches.json"):
        for off,hx in json.load(open("xj_patches.json")):
            blob=bytes.fromhex(hx)
            d[off:off+len(blob)]=blob
            n+=1
    open(p,'wb').write(d)
    print("keypath name fixups",len(KPNAMEFIX),"xj rewrites",n)

patch_macho(DST+"/Spotify")
patch_names(DST+"/Spotify")
patch_macho(DST+"/Frameworks/SpotifyShared.framework/SpotifyShared",shim_slot=True)
patch_macho(DST+"/Frameworks/MetaSmartGlassesKit.framework/MetaSmartGlassesKit",shim_slot=False)
for pl in [DST+"/Info.plist",DST+"/Frameworks/SpotifyShared.framework/Info.plist"]:
    d=plistlib.load(open(pl,'rb'));d["MinimumOSVersion"]="15.0";plistlib.dump(d,open(pl,'wb'),fmt=plistlib.FMT_XML)
os.makedirs("out",exist_ok=True)
with zipfile.ZipFile("out/Spotify15.ipa","w",zipfile.ZIP_DEFLATED,compresslevel=1) as z:
    for r,_,fs in os.walk(DST):
        for f in fs:
            full=os.path.join(r,f);z.write(full,"Payload/Spotify.app/"+os.path.relpath(full,DST).replace("\\","/"))
print("ok")
