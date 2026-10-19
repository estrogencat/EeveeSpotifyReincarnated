import struct,pickle,os,sys,json,collections

ex=pickle.load(open("exports15.pkl","rb"))
rex=pickle.load(open("reexports15.pkl","rb"))
paths=pickle.load(open("cachepaths.pkl","rb"))
where=collections.defaultdict(list)
for l,e in ex.items():
    for sy in e: where[sy].append(l)

def has(lib,sym,seen=None):
    seen=seen if seen is not None else set()
    if lib in seen: return False
    seen.add(lib)
    if sym in ex.get(lib,()): return True
    return any(has(r,sym,seen) for r in rex.get(lib,()))

SHIM_SYMS={f"_objc_{k}_x{n}" for k in("retain","release") for n in list(range(1,16))+list(range(19,29))}|{"_objc_claimAutoreleasedReturnValue"}

def scan(p):
    d=open(p,'rb').read()
    magic,cpu,_,ft,nc,sz,fl,_r=struct.unpack_from("<8I",d,0)
    assert magic==0xfeedfacf
    off=32;libs=[];fix=None
    for i in range(nc):
        cmd,cs=struct.unpack_from("<II",d,off)
        if cmd in(0xC,0x80000018,0x8000001F,0x20):
            no=struct.unpack_from("<I",d,off+8)[0]
            libs.append((cmd,d[off+no:off+cs].split(b"\0")[0].decode()))
        if cmd==0x80000034: fix=struct.unpack_from("<II",d,off+8)
        off+=cs
    fo,fs=fix
    v,so,imo,syo,ic,ifmt,sfmt=struct.unpack_from("<7I",d,fo)
    absent={idx for idx,(cmd,path) in enumerate(libs,1)
            if path.startswith(("/System","/usr/lib")) and path not in paths and path not in ex and path not in rex}
    out=[]
    for i in range(ic):
        w=struct.unpack_from("<I",d,fo+imo+4*i)[0]
        ordv=w&0xff;weak=(w>>8)&1;no=w>>9
        e=d.find(b"\0",fo+syo+no);name=d[fo+syo+no:e].decode()
        if not (0<ordv<=len(libs)): continue
        path=libs[ordv-1][1]
        if ordv in absent: out.append((name,path,"absentlib",weak)); continue
        if path.startswith("@"): continue
        if has(path,name): continue
        if name in SHIM_SYMS: out.append((name,path,"shim",weak)); continue
        if where.get(name): out.append((name,path,"rebound",weak)); continue
        out.append((name,path,"hard",weak))
    return out

res={}
for p in ["ipa/Payload/Spotify.app/Spotify",
          "ipa/Payload/Spotify.app/Frameworks/SpotifyShared.framework/SpotifyShared",
          "ipa/Payload/Spotify.app/Frameworks/MetaSmartGlassesKit.framework/MetaSmartGlassesKit"]:
    res[os.path.basename(p)]=scan(p)

json.dump(res,open("hardsyms.json","w"),indent=1)

for b,rows in res.items():
    hard=[(n,l) for n,l,k,w in rows if k=="hard" and not w]
    print("==",b,"total",len(rows),"hard-strong",len(hard))
    c=collections.Counter(l for n,l in hard)
    for l,n in c.most_common(): print("   %5d %s"%(n,l))
