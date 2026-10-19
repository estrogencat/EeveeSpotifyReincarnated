import lief, struct, collections, sys

BIN = "ipa/Payload/Spotify.app/Spotify"
b = lief.parse(BIN)
secs = {}
for s in b.sections:
    secs.setdefault(s.name, []).append(s)


def sec(name):
    out = []
    for s in b.sections:
        if s.name == name:
            out.append((s.virtual_address, bytes(s.content)))
    return out


def parse_name(data, off, end):
    """walk a mangled name, return (length, [(pos, kind)])"""
    refs = []
    i = off
    while i < end:
        c = data[i]
        if c == 0:
            break
        if 1 <= c <= 0x17:
            refs.append((i, c))
            i += 5
            continue
        i += 1
    return i - off, refs


def find_runs(data, base, start, length):
    """return list of (run_start_off, run_end_off) for <0a|0b ...> Xj runs"""
    runs = []
    i = start
    end = start + length
    while i < end:
        c = data[i]
        if c in (0x0A, 0x0B):
            j = i + 5
            ok = False
            while j + 1 < end:
                d = data[j]
                if d == 0:
                    break
                if 1 <= d <= 0x17:
                    j += 5
                    continue
                if d == 0x58 and data[j + 1] == 0x6A:  # 'Xj'
                    ok = True
                    j += 2
                    break
                j += 1
            if ok:
                runs.append((i, j))
                i = j
                continue
        if 1 <= c <= 0x17:
            i += 5
            continue
        i += 1
    return runs


total = 0
runlens = collections.Counter()
shapes = collections.Counter()
examples = []

for secname in ("__swift5_typeref", "__const", "__constg_swiftt"):
    for va, data in sec(secname):
        # names are NUL-separated; walk records
        i = 0
        n = len(data)
        while i < n:
            if data[i] == 0:
                i += 1
                continue
            ln, refs = parse_name(data, i, n)
            if ln:
                runs = find_runs(data, va, i, ln)
                for (a, z) in runs:
                    total += 1
                    runlens[z - a] += 1
                    kind = data[a]
                    rel = struct.unpack_from("<i", data, a + 1)[0]
                    tgt = va + a + 1 + rel
                    shapes[(kind, tgt)] += 1
                    if len(examples) < 6:
                        examples.append((secname, va + i, va + a, z - a, kind, tgt,
                                         bytes(data[i:i + ln])))
            i += ln + 1

print("total Xj runs:", total)
print("run length histogram:", dict(sorted(runlens.items())))
print("distinct shapes:", len(shapes))
print("top shapes:", shapes.most_common(5))
print()
for secname, nameva, runva, rl, kind, tgt, raw in examples:
    print("%s name@%#x run@%#x len=%d kind=%#x shape=%#x" % (secname, nameva, runva, rl, kind, tgt))
    print("   raw:", raw[:80])
