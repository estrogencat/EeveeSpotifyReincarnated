"""Fail if an IPA's main executable has a load-command stream dyld would reject.

This exists because ipapatch produced exactly that on an already-patched
binary: a bogus load command (0x00050000, size 0x10000) swallowing the tail of
the stream, and ncmds=150 when only 106 commands parse. dyld walks ncmds
commands, runs into zero padding, finds cmdsize==0 and refuses the image, so
the app is SIGKILLed at exec with no crash report at all.
"""
import struct
import sys
import zipfile

LC_CODE_SIGNATURE = 0x1D


def main(path):
    z = zipfile.ZipFile(path)
    mains = []
    for n in z.namelist():
        p = n.split("/")
        if len(p) == 3 and p[0] == "Payload" and p[1].endswith(".app") \
                and p[2] == p[1][:-4]:
            mains.append(n)
    if not mains:
        print("::error::%s has no Payload/<App>.app/<App> executable" % path)
        return 1

    rc = 0
    for name in mains:
        info = z.getinfo(name)
        with z.open(name) as f:
            d = f.read(2 << 20)
        magic, _cpu, _sub, _ft, ncmds, szcmds, _fl, _r = struct.unpack_from("<8I", d, 0)
        if magic != 0xFEEDFACF:
            print("::error::%s: %s is not a thin arm64 mach-o (magic %#x)"
                  % (path, name, magic))
            rc = 1
            continue

        print("%s: %s  ncmds=%d sizeofcmds=%#x size=%d"
              % (path, name, ncmds, szcmds, info.file_size))

        off, walked, bad = 32, 0, None
        while walked < ncmds:
            if off + 8 > len(d):
                bad = "command %d starts past the %d bytes we read" % (walked, len(d))
                break
            cmd, cs = struct.unpack_from("<II", d, off)
            if cs == 0:
                bad = ("command %d at %#x has cmdsize 0 (zero padding) - "
                       "ncmds claims %d" % (walked, off, ncmds))
                break
            if cs < 8 or off + cs > 32 + szcmds:
                bad = ("command %d at %#x type=%#x cmdsize=%#x overflows the "
                       "declared area (ends %#x)" % (walked, off, cmd, cs, 32 + szcmds))
                break
            if cmd == LC_CODE_SIGNATURE:
                dataoff, datasize = struct.unpack_from("<II", d, off + 8)
                if dataoff + datasize > info.file_size:
                    print("::error::%s: %s LC_CODE_SIGNATURE ends at %d, past EOF %d"
                          % (path, name, dataoff + datasize, info.file_size))
                    rc = 1
            off += cs
            walked += 1

        if bad:
            print("::error::%s: %s malformed load commands: %s" % (path, name, bad))
            rc = 1
        elif walked != ncmds:
            print("::error::%s: %s walked %d commands but ncmds=%d"
                  % (path, name, walked, ncmds))
            rc = 1
        else:
            print("   %d/%d commands parse cleanly" % (walked, ncmds))
    return rc


if __name__ == "__main__":
    sys.exit(max(main(p) for p in sys.argv[1:]))
