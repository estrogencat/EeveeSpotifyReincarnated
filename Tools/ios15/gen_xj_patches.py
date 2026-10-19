import lief, struct, json, os
import scan_xj
import shape_proto as sp

b = sp.b
SECTIONS = ("__swift5_typeref", "__const", "__constg_swiftt")

# shapes rewritten to a synthetic generic nominal type instead of being flattened,
# so the generalization argument survives into the name. see readme 8d.
KEEP_ARGS = {"StateHolderInternal": b"1S1AV"}

patches = []
stats = {"names": 0, "runs": 0, "patched": 0, "skip_unresolved": 0, "skip_toolong": 0,
         "kept": 0}
kept_shapes = {}


def proto_name(desc, slot):
    if desc:
        return sp.desc_name(desc)
    if slot:
        n = sp.mp_slots.get(slot, "")
        return n
    return ""


def keep_form(nm):
    for k, v in KEEP_ARGS.items():
        if k in nm:
            return k, v
    return None, None


def copy_args(data, srcva, a, z, dstva):
    """copy the arg bytes of a run, re-relativising any symbolic references"""
    out = bytearray()
    i = a + 5
    end = z - 2
    while i < end:
        c = data[i]
        if 1 <= c <= 0x17 and i + 5 <= end:
            rel = struct.unpack_from("<i", data, i + 1)[0]
            tgt = srcva + i + 1 + rel
            newrel = tgt - (dstva + len(out) + 1)
            out += bytes([c]) + struct.pack("<i", newrel)
            i += 5
            continue
        out.append(c)
        i += 1
    return bytes(out)


for s in b.sections:
    if s.name not in SECTIONS or s.segment_name != "__TEXT":
        continue
    va = s.virtual_address
    off = s.offset
    assert off == va - 0x100000000, (s.name, hex(off), hex(va))
    data = bytes(s.content)
    i, n = 0, len(data)
    while i < n:
        if data[i] == 0:
            i += 1
            continue
        ln, _ = scan_xj.parse_name(data, i, n)
        runs = scan_xj.find_runs(data, va, i, ln)
        if not runs:
            i += ln + 1
            continue
        stats["names"] += 1
        stats["runs"] += len(runs)
        out = bytearray()
        cur = i
        bad = False
        for (a, z) in runs:
            out += data[cur:a]
            kind = data[a]
            shape = va + a + 1 + struct.unpack_from("<i", data, a + 1)[0]
            desc, slot, _ex = sp.shape_protocol(shape, kind)
            if not desc and not slot:
                bad = True
                break
            pname = proto_name(desc, slot) or ""
            key, nominal = keep_form(pname)
            if key:
                argsva = va + i + len(out) + len(nominal) + 1
                out += nominal + b"y" + copy_args(data, va, a, z, argsva) + b"G"
                sbase = shape + 4 if kind == 0x0B else shape
                gnp = struct.unpack("<H", sp.rd(sbase + 16, 2))[0]
                kept_shapes.setdefault(key, {})[(kind, shape)] = gnp
                stats["kept"] += 1
                cur = z
                continue
            base = shape + 4 if kind == 0x0B else shape
            special = sp.u32(base) & 0xFF if sp.in_image(base) else 0
            suffix = b"_Xl" if special == 1 else b"_p"
            refkind = 1 if desc else 2
            target = desc if desc else slot
            # the rel32 is relative to its own address
            relpos = va + i + len(out) + 1
            out += bytes([refkind]) + struct.pack("<i", target - relpos) + suffix
            cur = z
        if bad:
            stats["skip_unresolved"] += 1
            i += ln + 1
            continue
        out += data[cur:i + ln]
        if len(out) > ln:
            stats["skip_toolong"] += 1
            i += ln + 1
            continue
        blob = bytes(out) + b"\0" * (ln - len(out))
        patches.append([off + i, blob.hex()])
        stats["patched"] += 1
        i += ln + 1

json.dump(patches, open("xj_patches.json", "w"))

os.makedirs("work", exist_ok=True)
with open("work/xj_gen.h", "w", newline="\n") as f:
    f.write("#define SHA_PROTO \"StateHolderInternal\"\n")
    f.write("#define SHA_NPARAMS 1\n")
    f.write("#define SHA_NSHAPES %d\n" % len(kept_shapes.get("StateHolderInternal", ())))

print(stats)
for k, v in kept_shapes.items():
    print("kept", k, "shapes", ["%#x np=%d" % (s, np) for (_, s), np in sorted(v.items())])
print("patches written:", len(patches))
