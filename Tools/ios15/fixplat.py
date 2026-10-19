import struct
p="work/libshim.dylib"
d=bytearray(open(p,'rb').read())
nc=struct.unpack_from("<I",d,16)[0];off=32
for i in range(nc):
    cmd,cs=struct.unpack_from("<II",d,off)
    if cmd==0x32: struct.pack_into("<3I",d,off+8,2,0xF0000,0xF0000)
    off+=cs
open(p,'wb').write(d)
