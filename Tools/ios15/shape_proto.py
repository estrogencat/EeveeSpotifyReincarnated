import lief, struct, pickle

b = lief.parse("ipa/Payload/Spotify.app/Spotify")
bindmap = pickle.load(open("work/bindmap_Spotify.pkl", "rb"))

protos = set()
for s in b.sections:
    if s.name == "__swift5_protos":
        va = s.virtual_address
        d = bytes(s.content)
        for i in range(0, len(d), 4):
            protos.add(va + i + struct.unpack_from("<i", d, i)[0])

mp_slots = {a: n for a, n in bindmap.items() if n.endswith("Mp")}


def rd(va, n):
    return bytes(b.get_content_from_virtual_address(va, n))


def i32(va):
    return struct.unpack("<i", rd(va, 4))[0]


def u32(va):
    return struct.unpack("<I", rd(va, 4))[0]


def u64(va):
    return struct.unpack("<Q", rd(va, 8))[0]


def cstr(va, n=200):
    raw = rd(va, n)
    out = []
    i = 0
    while i < len(raw) and raw[i]:
        c = raw[i]
        if 1 <= c <= 0x17:
            out.append("<%02x>" % c)
            i += 5
            continue
        out.append(chr(c) if 0x20 <= c < 0x7F else ".")
        i += 1
    return "".join(out)


def desc_name(va):
    return cstr(va + 8 + i32(va + 8), 96)


def in_image(va):
    for s in b.sections:
        if s.virtual_address <= va < s.virtual_address + s.size and s.size:
            return True
    return False


def shape_protocol(shape_va, kind):
    """return (protocol_descriptor_va_in_binary, got_slot_va_or_None, existential_name)"""
    base = shape_va + 4 if kind == 0x0B else shape_va
    if not in_image(base) or not in_image(base + 64):
        return None, None, "<outside image>"
    flags = u32(base)
    ex = cstr(base + 4 + i32(base + 4))
    np, nr, nk, nx = struct.unpack("<4H", rd(base + 8, 8))
    p = base + 16
    if flags & 0x100:
        p += 8
    if flags & 0x800:
        p += 4
    if flags & 0x1000:
        p += 4
    p += np
    p = (p + 3) & ~3
    for _ in range(nr):
        rf = u32(p)
        if (rf & 0x1F) == 0:
            fld = p + 8
            v = i32(fld)
            if v & 1:
                slot = fld + (v & ~1)
                if not in_image(slot) or not in_image(slot + 8):
                    p += 12
                    continue
                raw = u64(slot)
                tgt = 0x100000000 + (raw & 0xFFFFFFFFF)
                if slot in mp_slots:
                    return None, slot, ex
                if tgt in protos:
                    return tgt, None, ex
            else:
                tgt = fld + v
                if tgt in protos:
                    return tgt, None, ex
        p += 12
    return None, None, ex


if __name__ == "__main__":
    import scan_xj  # reuse the run scanner's shape list
    ok = imported = fail = 0
    seen = {}
    for secname in ("__swift5_typeref", "__const", "__constg_swiftt"):
        for s in b.sections:
            if s.name != secname:
                continue
            va, data = s.virtual_address, bytes(s.content)
            i, n = 0, len(data)
            while i < n:
                if data[i] == 0:
                    i += 1
                    continue
                ln, _ = scan_xj.parse_name(data, i, n)
                for (a, z) in scan_xj.find_runs(data, va, i, ln):
                    kind = data[a]
                    tgt = va + a + 1 + struct.unpack_from("<i", data, a + 1)[0]
                    seen[(kind, tgt)] = seen.get((kind, tgt), 0) + 1
                i += ln + 1
    print("distinct shapes:", len(seen))
    for (kind, shape), cnt in sorted(seen.items(), key=lambda kv: -kv[1]):
        d, slot, ex = shape_protocol(shape, kind)
        if d:
            ok += 1
            tag = "LOCAL  %#x %s" % (d, desc_name(d))
        elif slot:
            imported += 1
            tag = "IMPORT slot %#x %s" % (slot, mp_slots[slot])
        else:
            fail += 1
            tag = "FAIL"
        if fail <= 6 or d or slot:
            pass
        print("%5d %s  | %s" % (cnt, tag, ex[:70]))
    print("\nresolved local=%d imported=%d failed=%d" % (ok, imported, fail))
