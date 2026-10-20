#include <stdint.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <stdlib.h>

typedef struct { uint64_t low; int64_t high; } Dur;
typedef struct { int64_t seconds; int64_t attoseconds; } DurComp;

#define ATTOS_PER_SEC 1000000000000000000ULL

static inline __int128 unpack(Dur d)
{
    return (__int128)(((unsigned __int128)(uint64_t)d.high << 64) | d.low);
}

static inline Dur pack(__int128 a)
{
    Dur d;
    d.low = (uint64_t)a;
    d.high = (int64_t)(uint64_t)((unsigned __int128)a >> 64);
    return d;
}

static void udiv(unsigned __int128 n, uint64_t d, unsigned __int128 *q, uint64_t *r)
{
    unsigned __int128 quo = 0;
    uint64_t rem = 0;
    for (int i = 127; i >= 0; i--) {
        rem = (rem << 1) | (uint64_t)((n >> i) & 1);
        if (rem >= d) { rem -= d; quo |= ((unsigned __int128)1 << i); }
    }
    *q = quo; *r = rem;
}

Dur dur_zero(void) __asm__("_$ss8DurationV4zeroABvgZ");
Dur dur_zero(void)
{
    return pack(0);
}

Dur dur_seconds(double s) __asm__("_$ss8DurationV7secondsyABSdFZ");
Dur dur_seconds(double s)
{
    int64_t whole = (int64_t)s;
    int64_t frac = (int64_t)((s - (double)whole) * 1e18);
    unsigned __int128 hi = (unsigned __int128)(uint64_t)whole * ATTOS_PER_SEC;
    if (whole < 0) hi = (unsigned __int128)0 - ((unsigned __int128)(uint64_t)(-whole) * ATTOS_PER_SEC);
    return pack((__int128)hi + frac);
}

Dur dur_add(Dur a, Dur b) __asm__("_$ss8DurationV1poiyA2B_ABtFZ");
Dur dur_add(Dur a, Dur b)
{
    return pack(unpack(a) + unpack(b));
}

bool dur_lt(Dur a, Dur b) __asm__("_$ss8DurationV1loiySbAB_ABtFZ");
bool dur_lt(Dur a, Dur b)
{
    return unpack(a) < unpack(b);
}

DurComp dur_components(Dur self) __asm__("_$ss8DurationV10componentss5Int64V7seconds_AE11attosecondstvg");
DurComp dur_components(Dur self)
{
    __int128 a = unpack(self);
    int neg = a < 0;
    unsigned __int128 u = neg ? ((unsigned __int128)0 - (unsigned __int128)a) : (unsigned __int128)a;
    unsigned __int128 q; uint64_t r;
    udiv(u, ATTOS_PER_SEC, &q, &r);
    DurComp c;
    c.seconds = neg ? -(int64_t)q : (int64_t)q;
    c.attoseconds = neg ? -(int64_t)r : (int64_t)r;
    return c;
}

typedef struct { void *metadata; uint64_t state; } MetaResponse;

MetaResponse adm_metadata(uint64_t request) __asm__("_$s10AppIntents0A17DependencyManagerCMa");
MetaResponse adm_metadata(uint64_t request)
{
    MetaResponse r = { 0, 0 };
    return r;
}

void *adm_shared(void) __asm__("_$s10AppIntents0A17DependencyManagerC6sharedACvgZ");
void *adm_shared(void)
{
    return 0;
}

void adm_add(void) __asm__("_$s10AppIntents0A17DependencyManagerC3add3key10dependencyys11AnyHashableVSg_xyXAts8SendableRzlF");
void adm_add(void)
{
}

void asp_update(void) __asm__("_$s10AppIntents0A17ShortcutsProviderPAAE06updateA18ShortcutParametersyyFZ");
void asp_update(void)
{
}

typedef struct {
    void *(*init_buf_copy)(void *, void *, const void *);
    void (*destroy)(void *, const void *);
    void *(*init_copy)(void *, const void *, const void *);
    void *(*assign_copy)(void *, const void *, const void *);
    void *(*init_take)(void *, const void *, const void *);
    void *(*assign_take)(void *, const void *, const void *);
    unsigned (*get_tag_sp)(const void *, unsigned, const void *);
    void (*store_tag_sp)(void *, unsigned, unsigned, const void *);
    uint64_t size;
    uint64_t stride;
    uint32_t flags;
    uint32_t xi;
    unsigned (*get_tag)(const void *, const void *);
    void (*project)(void *, const void *);
    void (*inject)(void *, unsigned, const void *);
} VWT;

static void *vw_copy1(void *d, const void *s, const void *m)
{
    *(uint8_t *)d = *(const uint8_t *)s;
    return d;
}

static void *vw_buf_copy1(void *d, void *s, const void *m)
{
    *(uint8_t *)d = *(const uint8_t *)s;
    return d;
}

static void vw_destroy1(void *p, const void *m)
{
}

static unsigned vw_get_tag_sp1(const void *p, unsigned n, const void *m)
{
    return 0;
}

static void vw_store_tag_sp1(void *p, unsigned w, unsigned n, const void *m)
{
}

static unsigned vw_get_tag1(const void *p, const void *m)
{
    return *(const uint8_t *)p;
}

static void vw_project1(void *p, const void *m)
{
}

static void vw_inject1(void *p, unsigned tag, const void *m)
{
    *(uint8_t *)p = (uint8_t)tag;
}

static const VWT byte_enum_vwt = {
    vw_buf_copy1, vw_destroy1, vw_copy1, vw_copy1, vw_copy1, vw_copy1,
    vw_get_tag_sp1, vw_store_tag_sp1, 1, 1, 0, 0,
    vw_get_tag1, vw_project1, vw_inject1
};

static const struct { const void *vwt; uint64_t kind; const void *desc; } byte_enum_meta = {
    &byte_enum_vwt, 0x201, 0
};

MetaResponse dirhint_metadata(uint64_t request) __asm__("_$s10Foundation3URLV13DirectoryHintOMa");
MetaResponse dirhint_metadata(uint64_t request)
{
    MetaResponse r = { (void *)&byte_enum_meta.kind, 0 };
    return r;
}

const uint32_t dirhint_infer __asm__("_$s10Foundation3URLV13DirectoryHintO13inferFromPathyA2EmFWC") = 0;

extern void shim_call_appendpc(void *res, void *selfurl, uint64_t lo, uint64_t hi, uint64_t isdir, void *fn);
extern void shim_unimpl(const char *name);

static void *g_appendpc;
static void *g_strmeta;

void shim_url_appending(void *res, const uint64_t *path, void *selfurl, const void *meta);
void shim_url_appending(void *res, const uint64_t *path, void *selfurl, const void *meta)
{
    if (!g_appendpc) {
        g_appendpc = dlsym(RTLD_DEFAULT, "$s10Foundation3URLV22appendingPathComponent_11isDirectoryACSS_SbtF");
        g_strmeta = dlsym(RTLD_DEFAULT, "$sSSN");
    }
    if (!g_appendpc || !res || !selfurl || (g_strmeta && meta != g_strmeta)) {
        shim_unimpl("shim_url_appending-unhandled");
        return;
    }
    shim_call_appendpc(res, selfurl, path[0], path[1], 0, g_appendpc);
}

extern void shim_log(const char *a, const char *b);
extern void shim_logobj(const char *a, void *obj);

typedef const void *(*GetKeyPath)(void *, const void *);
static GetKeyPath g_gkp;

static void putnum(char *out, unsigned long long v)
{
    static const char *h = "0123456789abcdef";
    int i, j = 0;
    char t[20];
    if (!v) { out[0] = '0'; out[1] = 0; return; }
    while (v) { t[j++] = h[v & 15]; v >>= 4; }
    for (i = 0; i < j; i++) out[i] = t[j - 1 - i];
    out[j] = 0;
}

static int name_is_extended(const char *n, int *opt)
{
    int i, bad = 0, last = 0;
    for (i = 0; i < 512; i++) {
        unsigned char c = (unsigned char)n[i];
        if (!c) break;
        if (c >= 1 && c <= 0x17) {
            if (c == 0x0a || c == 0x0b) bad = 1;
            i += 4;
            continue;
        }
        last = i;
    }
    if (opt) *opt = (last >= 1 && n[last - 1] == 'S' && n[last] == 'g');
    return bad;
}

static void note_fixup(const void *fieldaddr, const void *name, const void *base)
{
    char a[64], b[64];
    int opt = 0;
    if (!name_is_extended((const char *)name, &opt)) return;
    putnum(a, (unsigned long long)((const char *)fieldaddr - (const char *)base));
    putnum(b, (unsigned long long)opt);
    shim_log("kpfix", a);
    shim_log("kpfix-opt", b);
}

const void *shim_getkeypath(void *pattern, const void *args);
const void *shim_getkeypath(void *pattern, const void *args)
{
    const int32_t *w = (const int32_t *)pattern;
    Dl_info di;
    const void *base = 0;
    if (dladdr(pattern, &di)) base = di.dli_fbase;
    const char *root = (const char *)pattern + 8 + w[2];
    const char *leaf = (const char *)pattern + 12 + w[3];
    if (base) {
        if ((uintptr_t)root & 1)
            note_fixup((const char *)pattern + 8, (const void *)((uintptr_t)root & ~(uintptr_t)1), base);
        if ((uintptr_t)leaf & 1)
            note_fixup((const char *)pattern + 12, (const void *)((uintptr_t)leaf & ~(uintptr_t)1), base);
    }
    if (!g_gkp) g_gkp = (GetKeyPath)dlsym(RTLD_DEFAULT, "swift_getKeyPathImpl");
    return g_gkp ? g_gkp(pattern, args) : 0;
}

typedef struct { const char *data; long len; } TName;
typedef TName (*GetTypeName)(const void *, int);
typedef bool (*DynCast)(void *, void *, const void *, const void *, unsigned long);

static void *swiftcore(void)
{
    static void *h;
    if (!h) h = dlopen("/usr/lib/swift/libswiftCore.dylib", RTLD_NOLOAD | RTLD_LAZY);
    return h;
}

static void logtype(const char *tag, const void *md)
{
    static GetTypeName gtn;
    char buf[200];
    if (!md) { shim_log(tag, "<null>"); return; }
    if (!gtn) gtn = (GetTypeName)dlsym(swiftcore(), "swift_getTypeName");
    if (!gtn) { shim_log(tag, "<nogtn>"); return; }
    TName n = gtn(md, 1);
    long i = 0;
    if (!n.data || n.len <= 0) { shim_log(tag, "<noname>"); return; }
    for (; i < n.len && i < (long)sizeof buf - 1; i++) buf[i] = n.data[i];
    buf[i] = 0;
    shim_log(tag, buf);
}

static const char *desc_name(const void *desc);
static int has_sub(const char *s, const char *sub);
static unsigned acat(char *b, unsigned j, const char *s);
static unsigned atypename(char *b, unsigned j, unsigned cap, const void *md);

static unsigned g_sh_zone;

static const void *sha_get(const void *arg0);
static int sha_find(const void *md, const void **arg);
static int sha_check(void *src, const void *srcType, const void *want, const void **gotout);
static const void *sha_canon(void);

typedef const void *(*GetObjType)(const void *);

static const void *exist_dyntype(const void *src, const void *md, uint32_t *outfl)
{
    uint32_t fl;
    const void *t = 0;
    if (outfl) *outfl = 0;
    if (!src || ((uintptr_t)src & 7) || !md || ((uintptr_t)md & 7)) return 0;
    if (*(const unsigned long *)md != 0x303) return 0;
    __builtin_memcpy(&fl, (const char *)md + sizeof(unsigned long), 4);
    if (outfl) *outfl = fl;
    if (fl & 0x3f000000u) return 0;
    if (fl & 0x80000000u) {
        t = ((const void *const *)src)[3];
    } else {
        static GetObjType got;
        const void *obj = *(const void *const *)src;
        if (!obj || ((uintptr_t)obj & 7)) return 0;
        if (!got) got = (GetObjType)dlsym(swiftcore(), "swift_getObjectType");
        if (!got) return 0;
        t = got(obj);
    }
    return (t && !((uintptr_t)t & 7) && (uintptr_t)t > 0x1000) ? t : 0;
}

static unsigned long g_shseen[256];

static int shcast_new(unsigned long key)
{
    unsigned i;
    if (!key) key = 1;
    for (i = 0; i < 256; i++) {
        if (g_shseen[i] == key) return 0;
        if (!g_shseen[i]) { g_shseen[i] = key; return 1; }
    }
    return 0;
}

bool shim_dyncast(void *dest, void *src, const void *srcType, const void *targetType, unsigned long flags)
    __asm__("_swift_dynamicCast");
bool shim_dyncast(void *dest, void *src, const void *srcType, const void *targetType, unsigned long flags)
{
    static DynCast real;
    static int nbad;
    if (!real) real = (DynCast)dlsym(swiftcore(), "swift_dynamicCast");
    if (!real) return false;
    if (!srcType || !targetType) {
        if (nbad < 2000) {
            nbad++;
            logtype("cast-src", srcType);
            logtype("cast-dst", targetType);
        }
        return false;
    }
    {
        unsigned long sk = *(const unsigned long *)srcType;
        if ((sk > 0x800 || sk == 0x305) && src && !((uintptr_t)src & 7)) {
            const void *obj = *(const void *const *)src;
            const void *isa = (obj && !((uintptr_t)obj & 7))
                              ? *(const void *const *)obj : 0;
            if (!obj || !isa) {
                static int nnull;
                if (nnull < 40) {
                    nnull++;
                    logtype(obj ? "castnullisa-src" : "castnullobj-src", srcType);
                    logtype(obj ? "castnullisa-dst" : "castnullobj-dst", targetType);
                }
                return false;
            }
        }
    }
    {
        const void *want = 0;
        if (*(const unsigned long *)targetType == 0x303 && sha_find(targetType, &want)) {
            const void *got = 0;
            int m = sha_check(src, srcType, want, &got);
            unsigned long key = (unsigned long)(uintptr_t)srcType * 131ul
                              + (unsigned long)(uintptr_t)got * 7ul
                              + (unsigned long)(uintptr_t)want * 17ul
                              + (unsigned long)(m + 2)
                              + (unsigned long)(g_sh_zone ? 64 : 0);
            if (shcast_new(key)) {
                char b[344];
                unsigned j = 0;
                j = acat(b, j, m == 1 ? "match " : (m == 0 ? "MISMATCH " : "unknown "));
                j = acat(b, j, g_sh_zone ? "zone " : "pre ");
                j = acat(b, j, "want=");
                j = atypename(b, j, 110, want);
                j = acat(b, j, " got=");
                j = atypename(b, j, 220, got);
                j = acat(b, j, " src=");
                j = atypename(b, j, 330, srcType);
                b[j] = 0;
                shim_log("shconstr", b);
            }
            if (m == 0 && !(flags & 1)) return false;
        }
    }
    bool r = real(dest, src, srcType, targetType, flags);
    {
        unsigned long tk = *(const unsigned long *)targetType;
        if (tk == 0x303) {
            unsigned int np = 0;
            __builtin_memcpy(&np, (const char *)targetType + sizeof(unsigned long) + 4, 4);
            if (np >= 1 && np <= 4) {
                unsigned i;
                for (i = 0; i < np; i++) {
                    uintptr_t pr = *(const uintptr_t *)((const char *)targetType + 16 + 8 * i);
                    const char *pn = desc_name((const void *)(pr & ~(uintptr_t)1));
                    if (has_sub(pn, "StateHolder")) {
                        uint32_t efl = 0;
                        const void *dt = exist_dyntype(src, srcType, &efl);
                        unsigned long key = (unsigned long)(uintptr_t)srcType * 131ul
                                          + (unsigned long)(uintptr_t)targetType * 31ul
                                          + (unsigned long)(uintptr_t)dt * 7ul
                                          + (unsigned long)(r ? 1 : 2)
                                          + (unsigned long)(g_sh_zone ? 4 : 0);
                        if (shcast_new(key)) {
                            char b[344];
                            unsigned j = 0;
                            j = acat(b, j, r ? "OK " : "NO ");
                            j = acat(b, j, g_sh_zone ? "zone " : "pre ");
                            j = acat(b, j, pn);
                            j = acat(b, j, " src=");
                            j = atypename(b, j, 150, srcType);
                            j = acat(b, j, " f=");
                            {
                                char nb[24];
                                putnum(nb, (unsigned long long)efl);
                                j = acat(b, j, nb);
                            }
                            if (dt) {
                                j = acat(b, j, " dyn=");
                                j = atypename(b, j, 300, dt);
                            } else {
                                j = acat(b, j, " dyn=-");
                            }
                            b[j] = 0;
                            shim_log("shcast", b);
                        }
                        break;
                    }
                }
            }
        }
    }
    if (!r && nbad < 2000) {
        nbad++;
        logtype("castfail-src", srcType);
        logtype("castfail-dst", targetType);
    }
    return r;
}

typedef const void *(*DynCastClass)(const void *, const void *);

const void *shim_dyncastclass(const void *obj, const void *target) __asm__("_swift_dynamicCastClass");
const void *shim_dyncastclass(const void *obj, const void *target)
{
    static DynCastClass real;
    static int nbad;
    if (!real) real = (DynCastClass)dlsym(swiftcore(), "swift_dynamicCastClass");
    if (!real) return 0;
    const void *r = real(obj, target);
    if (!r && nbad < 400) {
        nbad++;
        shim_logobj("ccfail-src", (void *)obj);
        logtype("ccfail-dst", target);
    }
    return r;
}

const void *shim_castmeta(const void *src, const void *dst) __asm__("_swift_dynamicCastMetatype");
const void *shim_castmeta(const void *src, const void *dst)
{
    static DynCastClass real;
    static int nbad;
    if (!real) real = (DynCastClass)dlsym(swiftcore(), "swift_dynamicCastMetatype");
    if (!real) return 0;
    const void *r = real(src, dst);
    if (!r && nbad < 400) {
        nbad++;
        logtype("mcfail-src", src);
        logtype("mcfail-dst", dst);
    }
    return r;
}

typedef const void *(*ByMangled)(const char *, unsigned long, const void *, const void *const *);

static unsigned long mangled_len(const char *p)
{
    unsigned long i = 0;
    while (i < 4096) {
        unsigned char c = (unsigned char)p[i];
        if (!c) break;
        if (c >= 1 && c <= 0x17) { i += 5; continue; }
        i++;
    }
    return i;
}

static unsigned long flat_existential(const char *shape, unsigned char kind, char *out, unsigned long cap);
static void log_shape(const char *shape, unsigned char kind, const void *md);
static unsigned long padded_existential(const char *shape, unsigned char kind, char *out,
                                        unsigned long cap, const void *const *args,
                                        const void **mdout, ByMangled bym);

const void *shim_extexist(const void *shape, const void *const *args)
    __asm__("_swift_getExtendedExistentialTypeMetadata");
const void *shim_extexist(const void *shape, const void *const *args)
{
    static ByMangled bym;
    static int nlog;
    char buf[220];
    const void *md = 0;
    unsigned long len;
    if (!shape) return 0;
    if (!bym) bym = (ByMangled)dlsym(swiftcore(), "swift_getTypeByMangledNameInContext");
    len = padded_existential((const char *)shape, 0x0b, buf, sizeof buf - 20, args, &md, bym);
    if (md && args && md == sha_canon()) {
        const void *our = sha_get(args[0]);
        if (our) {
            if (nlog < 20) { nlog++; shim_log("extexist-sha", buf); }
            return our;
        }
    }
    if (nlog < 20) {
        nlog++;
        shim_log(md ? "extexist-ok" : "extexist-fail", buf);
        log_shape((const char *)shape, 0x0b, md);
    }
    return md;
}

static void log_mangled(const char *tag, const char *p, unsigned long n)
{
    char b[200];
    unsigned long i;
    for (i = 0; i < n && i < sizeof b - 1; i++) {
        unsigned char c = (unsigned char)p[i];
        b[i] = (c >= 0x20 && c < 0x7f) ? (char)c : '.';
    }
    b[i] = 0;
    shim_log(tag, b);
}


extern void *malloc(unsigned long);

static unsigned long flat_existential(const char *shape, unsigned char kind, char *out, unsigned long cap)
{
    int32_t rel;
    uint32_t flags;
    const char *base = shape + (kind == 0x0b ? 8 : 4);
    const char *ex;
    unsigned long el, k;
    __builtin_memcpy(&flags, base - 4, 4);
    __builtin_memcpy(&rel, base, 4);
    ex = base + rel;
    el = mangled_len(ex);
    if (el >= 2 && ex[el - 2] == 'X' && ex[el - 1] == 'P') {
        for (k = 0; k + 1 < el; k++) {
            unsigned char d = (unsigned char)ex[k];
            if (d >= 1 && d <= 0x17) { k += 4; continue; }
            if (d == 'p' && ex[k + 1] == 'x') { el = k + 1; break; }
        }
    }
    for (k = 0; k + 1 < el; k++) {
        unsigned char d = (unsigned char)ex[k];
        if (d >= 1 && d <= 0x17) { k += 4; continue; }
        if (d == '_' && ex[k + 1] == 'p') { el = k + 2; break; }
    }
    if (el + 3 > cap) return 0;
    for (k = 0; k < el; k++) out[k] = ex[k];
    if ((flags & 0xff) == 1) { out[el - 1] = 'X'; out[el++] = 'l'; }
    out[el] = 0;
    return el;
}


extern int mprotect(void *, unsigned long, int);

static struct { const uint64_t *orig; uint64_t want; uint64_t *patched; } g_vwtfix[16];
static int g_nvwtfix;

__attribute__((unused)) static void patch_md_size(const void *md, uint64_t want)
{
    const uint64_t **slot = (const uint64_t **)((char *)md - 8);
    const uint64_t *orig = *slot;
    uint64_t *nv = 0;
    char b[80];
    int i;
    if (!orig || orig[8] == want || want < 8 || want > 4096) return;
    for (i = 0; i < g_nvwtfix; i++)
        if (g_vwtfix[i].orig == orig && g_vwtfix[i].want == want) { nv = g_vwtfix[i].patched; break; }
    if (!nv) {
        if (g_nvwtfix >= 16) return;
        nv = (uint64_t *)malloc(14 * 8);
        if (!nv) return;
        for (i = 0; i < 14; i++) nv[i] = orig[i];
        nv[8] = want;
        nv[9] = want;
        g_vwtfix[g_nvwtfix].orig = orig;
        g_vwtfix[g_nvwtfix].want = want;
        g_vwtfix[g_nvwtfix].patched = nv;
        g_nvwtfix++;
    }
    {
        uintptr_t page = (uintptr_t)slot & ~(uintptr_t)0x3fff;
        if (mprotect((void *)page, 0x4000, 3) != 0) {
            shim_log("mdsize-ro", "mprotect failed");
            return;
        }
    }
    *slot = nv;
    putnum(b, (unsigned long long)((orig[8] << 16) | want));
    shim_log("mdsize origsize<<16|want", b);
}

static unsigned long md_size(const void *md)
{
    const uint64_t *vw = *(const uint64_t *const *)((const char *)md - 8);
    return (unsigned long)vw[8];
}

static const char *PADS[] = { "s8SendableP", "s8Sendable", "SQ", "SH" };

static unsigned long padded_existential(const char *shape, unsigned char kind, char *out,
                                        unsigned long cap, const void *const *args,
                                        const void **mdout, ByMangled bym)
{
    uint32_t flags;
    const char *base = shape + (kind == 0x0b ? 8 : 4);
    const uint16_t *h = (const uint16_t *)(base + 4);
    unsigned long len = flat_existential(shape, kind, out, cap);
    unsigned long want, i, j;
    const void *md;
    __builtin_memcpy(&flags, base - 4, 4);
    if (!len) { *mdout = 0; return 0; }
    want = ((flags & 0xff) == 1) ? 8 + 8 * (unsigned long)(h[2] - h[0]) : 24 + 8 * (unsigned long)h[2];
    md = bym ? bym(out, len, 0, args) : 0;
    if (md && md_size(md) == want) { *mdout = md; return len; }
    if (0) {
        char cand[240];
        for (i = 0; i < sizeof PADS / sizeof PADS[0]; i++) {
            unsigned long cl = len - 1;
            const char *pad = PADS[i];
            if (cl + 16 >= sizeof cand) break;
            for (j = 0; j < cl; j++) cand[j] = out[j];
            for (j = 0; pad[j]; j++) cand[cl + j] = pad[j];
            cl += j;
            cand[cl++] = 'p';
            cand[cl] = 0;
            const void *m2 = bym ? bym(cand, cl, 0, args) : 0;
            if (m2 && md_size(m2) == want) {
                for (j = 0; j < cl; j++) out[j] = cand[j];
                out[cl] = 0;
                *mdout = m2;
                return cl;
            }
        }
    }
    *mdout = md;
    return len;
}

static char *rewrite_extended(const char *n, unsigned long l, unsigned long *outlen)
{
    unsigned long cap = l + 256;
    char *nb = (char *)malloc(cap);
    unsigned long o = 0, i = 0;
    int did = 0;
    if (!nb) return 0;
    while (i < l) {
        unsigned char c = (unsigned char)n[i];
        if ((c == 0x0a || c == 0x0b) && i + 5 <= l) {
            int32_t rel;
            unsigned long el;
            __builtin_memcpy(&rel, n + i + 1, 4);
            if (o + 160 >= cap) { return 0; }
            {
                const void *dummy = 0;
                static ByMangled bym2;
                if (!bym2) bym2 = (ByMangled)dlsym(swiftcore(), "swift_getTypeByMangledNameInContext");
                el = padded_existential(n + i + 1 + rel, c, nb + o, 150, 0, &dummy, bym2);
            }
            if (!el) { return 0; }
            o += el;
            i += 5;
            while (i + 1 < l) {
                unsigned char d = (unsigned char)n[i];
                if (d >= 1 && d <= 0x17) { i += 5; continue; }
                if (d == 'X' && n[i + 1] == 'j') { i += 2; break; }
                i++;
            }
            did = 1;
            continue;
        }
        if (c >= 1 && c <= 0x17 && i + 5 <= l) {
            int32_t rel;
            __builtin_memcpy(&rel, n + i + 1, 4);
            const char *tgt = n + i + 1 + rel;
            long nrel = (long)(tgt - (nb + o + 1));
            if (o + 6 >= cap || nrel > 0x7ff00000L || nrel < -0x7ff00000L) { return 0; }
            nb[o++] = (char)c;
            int32_t v = (int32_t)nrel;
            __builtin_memcpy(nb + o, &v, 4);
            o += 4;
            i += 5;
            continue;
        }
        if (o + 2 >= cap) { return 0; }
        nb[o++] = n[i++];
    }
    nb[o] = 0;
    *outlen = o;
    return did ? nb : 0;
}

static void log_shape(const char *shape, unsigned char kind, const void *md)
{
    const char *base = shape + (kind == 0x0b ? 8 : 4);
    uint32_t flags;
    const uint16_t *h = (const uint16_t *)(base + 4);
    char b[80];
    unsigned long sz = 0;
    __builtin_memcpy(&flags, base - 4, 4);
    if (md) {
        const uint64_t *vw = *(const uint64_t *const *)((const char *)md - 8);
        sz = (unsigned long)vw[8];
    }
    putnum(b, ((unsigned long long)flags << 48) | ((unsigned long long)h[0] << 32)
              | ((unsigned long long)h[1] << 24) | ((unsigned long long)h[2] << 16) | sz);
    shim_log("shape flags/np/nr/nk/size", b);
}

static int nxlog;

static int has_extref(const char *n, unsigned long l)
{
    unsigned long i;
    for (i = 0; i + 5 <= l; i++) {
        unsigned char c = (unsigned char)n[i];
        if (c == 0x0a || c == 0x0b) return 1;
        if (c >= 1 && c <= 0x17) i += 4;
    }
    return 0;
}

static int sane_ptr(const void *p)
{
    uintptr_t v = (uintptr_t)p;
    return v >= 0x100000000ULL && v < 0x0000800000000000ULL && !(v & 3);
}

/* 1 if any symbolic reference in the name resolves to `want` */
static int name_refs_desc(const char *n, unsigned long l, const void *want)
{
    unsigned long i;
    for (i = 0; i + 5 <= l; i++) {
        unsigned char c = (unsigned char)n[i];
        int32_t rel;
        const char *t;
        const void *desc;
        if (c < 1 || c > 0x17) continue;
        __builtin_memcpy(&rel, n + i + 1, 4);
        t = n + i + 1 + rel;
        i += 4;
        desc = t;
        if (c == 0x02) {
            if (!sane_ptr(t)) continue;
            desc = *(const void *const *)t;
        }
        if (desc == want) return 1;
    }
    return 0;
}

static void log_name_refs(const char *tag, const char *n, unsigned long l)
{
    unsigned long i;
    for (i = 0; i + 5 <= l; i++) {
        unsigned char c = (unsigned char)n[i];
        int32_t rel;
        const char *t;
        const void *desc;
        if (c < 1 || c > 0x17) continue;
        __builtin_memcpy(&rel, n + i + 1, 4);
        t = n + i + 1 + rel;
        i += 4;
        desc = t;
        if (c == 0x02) {
            if (!sane_ptr(t)) { shim_log(tag, "<badslot>"); continue; }
            desc = *(const void *const *)t;
        }
        if (!desc) {
            Dl_info di;
            char b[48];
            if (dladdr(t, &di) && di.dli_fbase)
                putnum(b, (unsigned long long)((const char *)t - (const char *)di.dli_fbase) + 0x100000000ULL);
            else
                putnum(b, (unsigned long long)t);
            shim_log(tag, b);
            shim_log("mangledfail-nullslot", b);
            continue;
        }
        if ((c == 0x01 || c == 0x02) && sane_ptr(desc)) {
            int32_t nrel;
            __builtin_memcpy(&nrel, (const char *)desc + 8, 4);
            {
                const char *nm = (const char *)desc + 8 + nrel;
                shim_log(tag, sane_ptr(nm) || nm ? nm : "?");
            }
        } else {
            char b[32];
            putnum(b, c);
            shim_log(tag, b);
        }
    }
}

static int nnullfix;

static char *rewrite_nullrefs(const char *n, unsigned long l, unsigned long *outlen)
{
    unsigned long cap = l + 64, o = 0, i = 0;
    int did = 0;
    char *nb = (char *)malloc(cap);
    if (!nb) return 0;
    while (i < l) {
        unsigned char c = (unsigned char)n[i];
        if (c >= 1 && c <= 0x17 && i + 5 <= l) {
            int32_t rel, v;
            const char *t;
            const void *desc;
            long nrel;
            __builtin_memcpy(&rel, n + i + 1, 4);
            t = n + i + 1 + rel;
            desc = t;
            if (c == 0x02) desc = sane_ptr(t) ? *(const void *const *)t : (const void *)1;
            if (!desc) {
                if (i + 5 < l && n[i + 5] == 'y') return 0;
                if (o + 3 >= cap) return 0;
                nb[o++] = 'S';
                nb[o++] = 'S';
                did = 1;
                i += 5;
                continue;
            }
            nrel = (long)(t - (nb + o + 1));
            if (o + 6 >= cap || nrel > 0x7ff00000L || nrel < -0x7ff00000L) return 0;
            nb[o++] = (char)c;
            v = (int32_t)nrel;
            __builtin_memcpy(nb + o, &v, 4);
            o += 4;
            i += 5;
            continue;
        }
        if (o + 2 >= cap) return 0;
        nb[o++] = n[i++];
    }
    nb[o] = 0;
    *outlen = o;
    return did ? nb : 0;
}

static const void *retry_nullrefs(ByMangled real, const char *n, unsigned long l,
                                  const void *ctx, const void *const *a)
{
    unsigned long nl = 0;
    char *nb = rewrite_nullrefs(n, l, &nl);
    const void *r;
    if (!nb) return 0;
    r = real(nb, nl, ctx, a);
    if (nnullfix < 40) {
        nnullfix++;
        log_mangled(r ? "nullfix-ok" : "nullfix-fail", nb, nl);
        logtype("nullfix-md", r);
    }
    return r;
}

static const void *retry_mangled(ByMangled real, const char *n, unsigned long l, const void *ctx, const void *const *a, const char *tag)
{
    unsigned long nl = 0;
    char *nb = rewrite_extended(n, l, &nl);
    const void *r;
    unsigned long i;
    if (!nb) return 0;
    r = real(nb, nl, ctx, a);
    if (nxlog < 60) {
        nxlog++;
        log_mangled(r ? "xfix-ok" : "xfix-fail", nb, nl);
        logtype("xfix-md", r);
    }
    for (i = 0; i + 5 <= l; i++) {
        unsigned char c = (unsigned char)n[i];
        if (c == 0x0a || c == 0x0b) {
            int32_t rel;
            __builtin_memcpy(&rel, n + i + 1, 4);
            log_shape(n + i + 1 + rel, c, r);
            break;
        }
        if (c >= 1 && c <= 0x17) i += 4;
    }
    return r;
}

static unsigned g_exlog;
static unsigned long long g_exseen[1024];

static void log_existential(const char *tag, const char *n, unsigned long l, const void *md)
{
    unsigned long kind;
    unsigned int flags, nproto;
    const unsigned long long *vwt;
    unsigned long long size = 0, stride = 0, key;
    unsigned wt, i, j;
    char b[256], num[24];

    if (!md) return;
    kind = *(const unsigned long *)md;
    if (kind != 0x303 && kind != 0x307) return;

    __builtin_memcpy(&flags, (const char *)md + sizeof(unsigned long), 4);
    __builtin_memcpy(&nproto, (const char *)md + sizeof(unsigned long) + 4, 4);
    vwt = (const unsigned long long *)*(const void *const *)((const char *)md - sizeof(void *));
    if (vwt) { size = vwt[8]; stride = vwt[9]; }
    wt = (kind == 0x303) ? (flags & 0x00ffffffu) : 0;

    if (size < 0x20) return;

    key = (unsigned long long)l * 1000003ull + size * 97ull + wt;
    for (i = 0; i < l && i < 32; i++) key = key * 131ull + (unsigned char)n[i];
    if (!key) key = 1;
    for (i = 0; i < 1024; i++) {
        if (g_exseen[i] == key) return;
        if (!g_exseen[i]) { g_exseen[i] = key; break; }
    }
    if (g_exlog > 900) return;
    g_exlog++;

    j = 0;
    b[j++] = 'k'; b[j++] = '=';
    putnum(num, kind);
    for (i = 0; num[i] && j < 200; i++) b[j++] = num[i];
    b[j++] = ' '; b[j++] = 's'; b[j++] = 'z'; b[j++] = '=';
    putnum(num, size);
    for (i = 0; num[i] && j < 200; i++) b[j++] = num[i];
    b[j++] = ' '; b[j++] = 's'; b[j++] = 't'; b[j++] = '=';
    putnum(num, stride);
    for (i = 0; num[i] && j < 200; i++) b[j++] = num[i];
    b[j++] = ' '; b[j++] = 'w'; b[j++] = 't'; b[j++] = '=';
    putnum(num, wt);
    for (i = 0; num[i] && j < 200; i++) b[j++] = num[i];
    b[j++] = ' '; b[j++] = 'n'; b[j++] = 'p'; b[j++] = '=';
    putnum(num, nproto);
    for (i = 0; num[i] && j < 200; i++) b[j++] = num[i];
    b[j++] = ' ';
    {
        static GetTypeName gtn;
        if (!gtn) gtn = (GetTypeName)dlsym(swiftcore(), "swift_getTypeName");
        if (gtn) {
            TName tn = gtn(md, 1);
            if (tn.data && tn.len > 0) {
                for (i = 0; i < (unsigned)tn.len && j < sizeof b - 1; i++) {
                    unsigned char c = (unsigned char)tn.data[i];
                    b[j++] = (c >= 0x20 && c < 0x7f) ? (char)c : '.';
                }
                b[j] = 0;
                shim_log(tag, b);
                return;
            }
        }
    }
    for (i = 0; i < l && j < sizeof b - 1; i++) {
        unsigned char c = (unsigned char)n[i];
        b[j++] = (c >= 0x20 && c < 0x7f) ? (char)c : '.';
    }
    b[j] = 0;
    shim_log(tag, b);
}

static void *g_anymd;
static void *g_voidmd;
static unsigned g_badgen;

static void log_badgenerics(const char *tag, const char *n, unsigned long l, const void *md)
{
    unsigned long kind;
    const void *desc;
    unsigned i, j, hits = 0;
    char b[320], num[24];

    if (!md) return;
    if (!g_anymd) {
        static int said;
        g_anymd = dlsym(RTLD_DEFAULT, "$sypN");
        g_voidmd = dlsym(RTLD_DEFAULT, "$sytN");
        if (!said) {
            said = 1;
            shim_log("badgen-armed", (g_anymd && g_voidmd) ? "both"
                                   : g_anymd ? "any-only"
                                   : g_voidmd ? "void-only" : "neither");
        }
    }
    if (!g_anymd && !g_voidmd) return;
    kind = *(const unsigned long *)md;
    if (kind <= 0x800) return;
    desc = *(void *const *)((const char *)md + 0x40);
    if (!desc) return;

    j = 0;
    for (i = 0x50; i <= 0x78; i += 8) {
        const void *v = *(void *const *)((const char *)md + i);
        if (v != g_anymd && v != g_voidmd) continue;
        hits++;
        if (j < 100) {
            b[j++] = ' ';
            b[j++] = '+';
            putnum(num, i);
            for (unsigned k = 0; num[k] && j < 100; k++) b[j++] = num[k];
            b[j++] = '=';
            const char *w = (v == g_anymd) ? "Any" : "()";
            for (unsigned k = 0; w[k] && j < 110; k++) b[j++] = w[k];
        }
    }
    if (!hits) return;
    if (g_badgen > 40) return;
    g_badgen++;
    for (i = 0; i < l && j < sizeof b - 1; i++) {
        unsigned char c = (unsigned char)n[i];
        b[j++] = (c >= 0x20 && c < 0x7f) ? (char)c : '.';
    }
    b[j] = 0;
    shim_log(tag, b);
}

static const void *hostcfg_md(void);
extern const char ntd_hostcfg[] __asm__("_$s7SwiftUI22UIHostingConfigurationVMn");

/* a bound-generic UIHostingConfiguration<...> cannot demangle against our
   non-generic descriptor, so answer it here with the one box metadata */
static const void *hostcfg_byname(const char *n, unsigned long l)
{
    static unsigned nlog;
    if (!name_refs_desc(n, l, (const void *)ntd_hostcfg)) return 0;
    if (nlog < 12) {
        nlog++;
        shim_log("hostcfg-byname", "resolved");
    }
    return hostcfg_md();
}

const void *shim_bymangled_ctx(const char *n, unsigned long l, const void *ctx, const void *const *a)
    __asm__("_swift_getTypeByMangledNameInContext");
const void *shim_bymangled_ctx(const char *n, unsigned long l, const void *ctx, const void *const *a)
{
    static ByMangled real;
    static int nbad;
    if (!real) real = (ByMangled)dlsym(swiftcore(), "swift_getTypeByMangledNameInContext");
    if (!real) return 0;
    const void *r = real(n, l, ctx, a);
    if (has_extref(n, l)) {
        const void *r2 = retry_mangled(real, n, l, ctx, a, "ctx");
        if (r2) r = r2;
    }
    if (!r) {
        if (nbad < 60) {
            nbad++;
            log_mangled("mangledfail-ctx", n, l);
            log_name_refs("mangledfail-ref", n, l);
        }
        r = retry_mangled(real, n, l, ctx, a, "ctx");
    }
    if (!r) r = retry_nullrefs(real, n, l, ctx, a);
    if (!r) r = hostcfg_byname(n, l);
    log_existential("exmd-ctx", n, l, r);
    log_badgenerics("badgen-ctx", n, l, r);
    return r;
}

const void *shim_bymangled_env(const char *n, unsigned long l, const void *env, const void *const *a)
    __asm__("_swift_getTypeByMangledNameInEnvironment");
const void *shim_bymangled_env(const char *n, unsigned long l, const void *env, const void *const *a)
{
    static ByMangled real;
    static int nbad;
    if (!real) real = (ByMangled)dlsym(swiftcore(), "swift_getTypeByMangledNameInEnvironment");
    if (!real) return 0;
    const void *r = real(n, l, env, a);
    if (has_extref(n, l)) {
        const void *r2 = retry_mangled(real, n, l, env, a, "env");
        if (r2) r = r2;
    }
    if (!r) {
        if (nbad < 60) {
            nbad++;
            log_mangled("mangledfail-env", n, l);
            log_name_refs("mangledfail-ref", n, l);
        }
        r = retry_mangled(real, n, l, env, a, "env");
    }
    if (!r) r = retry_nullrefs(real, n, l, env, a);
    if (!r) r = hostcfg_byname(n, l);
    log_existential("exmd-env", n, l, r);
    log_badgenerics("badgen-env", n, l, r);
    return r;
}

typedef const void *(*GetWitness)(const void *, const void *, const void *const *);

static uintptr_t rel_indirectable(const int32_t *p)
{
    int32_t v = *p;
    uintptr_t t;
    if (!v) return 0;
    t = (uintptr_t)p + (uintptr_t)(intptr_t)(v & ~1);
    if (v & 1) t = *(const uintptr_t *)t;
    return t;
}

static unsigned g_dummycall;

static uint64_t wt_dummy_hit(unsigned slot)
{
    if (g_dummycall < 60) {
        char b[24];
        g_dummycall++;
        putnum(b, slot);
        shim_log("wtdummy-call", b);
    }
    return 0;
}

static uint64_t wt_dummy_slot(void)
{
    return wt_dummy_hit(0xff);
}

#define DUMMY_SLOT(n) static uint64_t wt_dummy_##n(void) { return wt_dummy_hit(n); }
DUMMY_SLOT(1) DUMMY_SLOT(2) DUMMY_SLOT(3) DUMMY_SLOT(4)
DUMMY_SLOT(5) DUMMY_SLOT(6) DUMMY_SLOT(7) DUMMY_SLOT(8)
DUMMY_SLOT(9) DUMMY_SLOT(10) DUMMY_SLOT(11) DUMMY_SLOT(12)
DUMMY_SLOT(13) DUMMY_SLOT(14) DUMMY_SLOT(15) DUMMY_SLOT(16)

static const void *const g_dummy_named[] = {
    (const void *)wt_dummy_1, (const void *)wt_dummy_2, (const void *)wt_dummy_3,
    (const void *)wt_dummy_4, (const void *)wt_dummy_5, (const void *)wt_dummy_6,
    (const void *)wt_dummy_7, (const void *)wt_dummy_8, (const void *)wt_dummy_9,
    (const void *)wt_dummy_10, (const void *)wt_dummy_11, (const void *)wt_dummy_12,
    (const void *)wt_dummy_13, (const void *)wt_dummy_14, (const void *)wt_dummy_15,
    (const void *)wt_dummy_16,
};

static const void *g_dummy_wt[128];
static int g_dummy_wt_init;
static const uint64_t g_dummy_conf[8];

static unsigned acat(char *b, unsigned j, const char *s);
static unsigned anum(char *b, unsigned j, unsigned long long v);
static unsigned aimg(char *b, unsigned j, const void *p);
static unsigned amdbrief(char *b, unsigned j, const void *md);
static int md_is_erased(const void *md);
int sh_peek(void *dst, const void *src, unsigned long n);
static unsigned g_dummyarg;
static const void *geo_witness_table(void);
extern const char geo_conf[] __asm__("_$s7SwiftUI23_GeometryActionModifierVyxGAA04ViewE0AAMc");
extern const char ntd_geo[] __asm__("_$s7SwiftUI23_GeometryActionModifierVMn");
static const void *geo_md(const void *arg0);
static int conf_interesting(const void *conf);
static void sh_patch_getter(const void *wt, const void *type);
static unsigned g_wtshlog;

static const void *g_wtshseen[512];

static int wtsh_new(const void *type)
{
    unsigned i;
    for (i = 0; i < 512; i++) {
        if (g_wtshseen[i] == type) return 0;
        if (!g_wtshseen[i]) { g_wtshseen[i] = type; return 1; }
    }
    return 1;
}

const void *shim_getwitness(const void *conf, const void *type, const void *const *args)
    __asm__("_swift_getWitnessTable");
const void *shim_getwitness(const void *conf, const void *type, const void *const *args)
{
    static GetWitness real;
    static int nlog;
    if (conf == (const void *)geo_conf) {
        const void *wt = geo_witness_table();
        if (wt) return wt;
    }
    if (!conf || !rel_indirectable((const int32_t *)conf)) {
        if (!g_dummy_wt_init) {
            unsigned i;
            for (i = 1; i < sizeof g_dummy_wt / sizeof g_dummy_wt[0]; i++)
                g_dummy_wt[i] = (i <= 16) ? g_dummy_named[i - 1]
                                          : (const void *)wt_dummy_slot;
            g_dummy_wt_init = 1;
        }
        g_dummy_wt[0] = conf ? conf : (const void *)g_dummy_conf;
        if (nlog < 40) {
            char b[80], c[48];
            void **fp = (void **)__builtin_frame_address(0);
            void *lr = fp ? fp[1] : 0;
            Dl_info di;
            putnum(b, (unsigned long long)conf);
            if (lr && dladdr(lr, &di) && di.dli_fbase)
                putnum(c, (unsigned long long)((const char *)lr - (const char *)di.dli_fbase)
                          + 0x100000000ULL);
            else
                putnum(c, (unsigned long long)lr);
            nlog++;
            shim_log(conf ? "wtnull" : "wtnullconf", b);
            shim_log("wtnull-lr", c);
            if (conf && dladdr((void *)conf, &di) && di.dli_fbase) {
                const char *f = di.dli_fname, *s = f;
                char ib[200];
                unsigned k = 0;
                for (; f && *f; f++) if (*f == '/') s = f + 1;
                for (; s && *s && k < sizeof ib - 24; k++, s++) ib[k] = *s;
                ib[k++] = '+';
                putnum(ib + k, (unsigned long long)((const char *)conf
                                                    - (const char *)di.dli_fbase));
                shim_log("wtnull-img", ib);
            } else {
                shim_log("wtnull-img", "<no image>");
            }
        }
        return g_dummy_wt;
    }
    if (!real) real = (GetWitness)dlsym(swiftcore(), "swift_getWitnessTable");
    if (!real) return 0;
    {
        const void *wt;
        int dummyarg = -1;
        if (args) {
            unsigned i;
            for (i = 0; i < 4; i++) {
                const void *a;
                if (!sh_peek(&a, (const char *)args + i * 8, 8)) break;
                if (a == (const void *)g_dummy_wt) { dummyarg = (int)i; break; }
            }
        }
        if (dummyarg >= 0 && g_dummyarg < 20) {
            char b[300];
            unsigned j = 0;
            g_dummyarg++;
            j = acat(b, j, "arg");
            j = anum(b, j, (unsigned long long)dummyarg);
            j = acat(b, j, " conf=");
            j = aimg(b, j, conf);
            j = acat(b, j, " t=");
            j = amdbrief(b, j, type);
            b[j] = 0;
            shim_log("wtdummy-arg-pre", b);
        }
        wt = real(conf, type, args);
        if (dummyarg >= 0 && g_dummyarg <= 20) shim_log("wtdummy-arg-post", "ok");
        if (wt && conf_interesting(conf)) sh_patch_getter(wt, type);
        if (wt && conf_interesting(conf) && g_wtshlog < 400 && wtsh_new(type)) {
            char b[300];
            unsigned j = 0;
            g_wtshlog++;
            j = acat(b, j, "wt=");
            j = aimg(b, j, wt);
            j = acat(b, j, " t=");
            j = amdbrief(b, j, type);
            if (type && !((uintptr_t)type & 7)) {
                unsigned long k = *(const unsigned long *)type;
                const void *a = *(const void *const *)((const char *)type
                                                       + ((k > 0x800) ? 0x50 : 0x10));
                j = acat(b, j, " arg0=");
                if (md_is_erased(a) == 1) j = acat(b, j, "Any");
                else if (md_is_erased(a) == 2) j = acat(b, j, "()");
                else j = amdbrief(b, j, a);
            }
            b[j] = 0;
            shim_log("wtsh", b);
        }
        return wt;
    }
}
typedef struct { const void *value; unsigned long state; } MDResp;
typedef MDResp (*GetAssocType)(unsigned long, const void *const *, const void *,
                               const void *, const void *);

static unsigned g_assoclog, g_assocany, g_assocsh;
static unsigned long long g_assocseen[512];

#define ACAP 330

static unsigned acat(char *b, unsigned j, const char *s)
{
    unsigned i;
    for (i = 0; s[i] && j < ACAP; i++) b[j++] = s[i];
    return j;
}

static unsigned anum(char *b, unsigned j, unsigned long long v)
{
    char num[24];
    putnum(num, v);
    return acat(b, j, num);
}

static unsigned aimg(char *b, unsigned j, const void *p)
{
    Dl_info di;
    if (p && dladdr((void *)p, &di) && di.dli_fbase)
        return anum(b, j, (unsigned long long)((const char *)p - (const char *)di.dli_fbase)
                          + 0x100000000ULL);
    return anum(b, j, (unsigned long long)p);
}

static int in_image(const void *p)
{
    Dl_info di;
    return p && dladdr((void *)p, &di) && di.dli_fbase;
}

static const char *desc_name(const void *desc)
{
    int32_t rel;
    const char *p;
    if (!in_image(desc)) return 0;
    __builtin_memcpy(&rel, (const char *)desc + 8, 4);
    if (!rel) return 0;
    p = (const char *)desc + 8 + rel;
    return in_image(p) ? p : 0;
}

static const void *conf_proto(const void *conf)
{
    if (!in_image(conf)) return 0;
    return (const void *)rel_indirectable((const int32_t *)conf);
}

static const void *conf_type(const void *conf)
{
    int32_t rel;
    uint32_t flags;
    const char *p;
    if (!in_image(conf)) return 0;
    __builtin_memcpy(&rel, (const char *)conf + 4, 4);
    __builtin_memcpy(&flags, (const char *)conf + 12, 4);
    if (!rel || (flags & 7) > 1) return 0;
    p = (const char *)conf + 4 + rel;
    if (!in_image(p)) return 0;
    if (flags & 7) {
        p = *(const char *const *)p;
        if (!in_image(p)) return 0;
    }
    return p;
}

static unsigned atypename(char *b, unsigned j, unsigned cap, const void *md)
{
    static GetTypeName gtn;
    unsigned i;
    if (!md) { for (i = 0; "<null>"[i] && j < cap; i++) b[j++] = "<null>"[i]; return j; }
    if (!gtn) gtn = (GetTypeName)dlsym(swiftcore(), "swift_getTypeName");
    if (gtn) {
        TName tn = gtn(md, 1);
        if (tn.data && tn.len > 0) {
            for (i = 0; i < (unsigned)tn.len && j < cap; i++) {
                unsigned char c = (unsigned char)tn.data[i];
                b[j++] = (c >= 0x20 && c < 0x7f) ? (char)c : '.';
            }
            return j;
        }
    }
    if (md == g_anymd) { for (i = 0; "Any"[i] && j < cap; i++) b[j++] = "Any"[i]; return j; }
    if (md == g_voidmd) { for (i = 0; "()"[i] && j < cap; i++) b[j++] = "()"[i]; return j; }
    return aimg(b, j, md);
}

static int has_sub(const char *s, const char *sub)
{
    unsigned i, k;
    if (!s) return 0;
    for (i = 0; s[i] && i < 128; i++) {
        for (k = 0; sub[k] && s[i + k] == sub[k]; k++) ;
        if (!sub[k]) return 1;
    }
    return 0;
}

static int md_is_erased(const void *md)
{
    unsigned long kind;
    unsigned int nproto;
    if (!md) return 0;
    if (md == g_anymd) return 1;
    if (md == g_voidmd) return 2;
    if ((uintptr_t)md < 0x1000 || ((uintptr_t)md & 7)) return 0;
    kind = *(const unsigned long *)md;
    if (kind == 0x303) {
        __builtin_memcpy(&nproto, (const char *)md + sizeof(unsigned long) + 4, 4);
        return nproto == 0 ? 1 : 0;
    }
    if (kind == 0x301)
        return *(const unsigned long *)((const char *)md + sizeof(unsigned long)) == 0 ? 2 : 0;
    return 0;
}

static unsigned amdbrief(char *b, unsigned j, const void *md)
{
    unsigned long kind;
    const void *desc;
    const char *nm;
    if (!md || ((uintptr_t)md & 7)) return aimg(b, j, md);
    kind = *(const unsigned long *)md;
    if (kind > 0x800) desc = *(const void *const *)((const char *)md + 0x40);
    else if (kind == 0x200 || kind == 0x201 || kind == 0x202)
        desc = *(const void *const *)((const char *)md + 0x08);
    else return aimg(b, j, md);
    nm = desc_name(desc);
    return nm ? acat(b, j, nm) : aimg(b, j, md);
}

static unsigned amangled(char *b, unsigned j, const char *n)
{
    unsigned i;
    if (!in_image(n)) return acat(b, j, "<unreadable>");
    for (i = 0; i < 64 && j < ACAP; i++) {
        unsigned char c = (unsigned char)n[i];
        if (!c) break;
        if (c >= 1 && c <= 0x17) {
            int32_t rel;
            b[j++] = '<';
            j = anum(b, j, c);
            j = acat(b, j, ">@");
            __builtin_memcpy(&rel, n + i + 1, 4);
            j = aimg(b, j, n + i + 1 + rel);
            i += 4;
            continue;
        }
        b[j++] = (c >= 0x20 && c < 0x7f) ? (char)c : '.';
    }
    return j;
}

static void log_assoc(const void *conf, const void *ctype, unsigned long idx,
                      const void *slot, const void *res)
{
    unsigned long long key;
    unsigned i, j = 0, isbad;
    char b[400], rn[400];
    const char *nm, *tn, *tag;

    if (!g_anymd) {
        g_anymd = dlsym(RTLD_DEFAULT, "$sypN");
        g_voidmd = dlsym(RTLD_DEFAULT, "$sytN");
    }
    tn = desc_name(conf_type(conf));
    isbad = md_is_erased(res);
    if (has_sub(tn, "StateHolder")) {
        tag = "assoc-SH";
        if (g_assocsh >= 200) return;
        g_assocsh++;
    } else if (isbad && !(tn && tn[0] == 'A' && tn[1] == 'n' && tn[2] == 'y')) {
        tag = "assoc-ANY";
        if (g_assocany >= 60) return;
        g_assocany++;
    } else {
        tag = "assoc";
        key = (unsigned long long)(uintptr_t)conf * 131ull + idx + 1;
        for (i = 0; i < 512; i++) {
            if (g_assocseen[i] == key) return;
            if (!g_assocseen[i]) { g_assocseen[i] = key; break; }
        }
        if (g_assoclog >= 40) return;
        g_assoclog++;
    }

    j = acat(b, j, "conf=");
    j = aimg(b, j, conf);
    nm = desc_name(conf_proto(conf));
    if (nm) { j = acat(b, j, " p="); j = acat(b, j, nm); }
    if (tn) { j = acat(b, j, " t="); j = acat(b, j, tn); }
    j = acat(b, j, " i=");
    j = anum(b, j, idx);
    j = acat(b, j, " slot=");
    j = aimg(b, j, slot);
    if (slot && ((uintptr_t)slot & 1)) {
        j = acat(b, j, " n=");
        j = amangled(b, j, (const char *)((uintptr_t)slot & ~(uintptr_t)1));
    }
    rn[atypename(rn, 0, 380, res)] = 0;
    j = acat(b, j, " ct=");
    j = atypename(b, j, ACAP, ctype);
    j = acat(b, j, " -> ");
    j = acat(b, j, rn);
    b[j] = 0;
    shim_log(tag, b);
}

MDResp shim_getassoctype(unsigned long request, const void *const *wtable,
                         const void *ctype, const void *reqBase, const void *assocType)
    __asm__("_swift_getAssociatedTypeWitness");
MDResp shim_getassoctype(unsigned long request, const void *const *wtable,
                         const void *ctype, const void *reqBase, const void *assocType)
{
    static GetAssocType real;
    static int armed;
    MDResp r;
    unsigned long idx = 0;
    const void *slot = 0, *conf = 0;

    if (!armed) {
        armed = 1;
        real = (GetAssocType)dlsym(swiftcore(), "swift_getAssociatedTypeWitness");
        shim_log("assoc-armed", real ? "real" : "noreal");
    }
    if (!real) { r.value = 0; r.state = 0; return r; }
    if (reqBase && assocType && (const char *)assocType >= (const char *)reqBase) {
        unsigned long d = (unsigned long)((const char *)assocType - (const char *)reqBase);
        if (!(d & 7) && d < 2048) idx = d / 8 + 1;
    }
    if (wtable && idx) slot = wtable[idx];
    if (wtable) conf = wtable[0];
    r = real(request, wtable, ctype, reqBase, assocType);
    log_assoc(conf, ctype, idx, slot, r.value);
    return r;
}

typedef MDResp (*GetGenericMD)(unsigned long, const void *const *, const void *);

static unsigned g_genlog;
static unsigned long long g_genseen[256];
static const void *g_gdesc[1024];
static unsigned char g_gverdict[1024];

#define SH_STATE_GETTER_SLOT 2

static const void *g_sh_orig_getter;
static unsigned g_sh_patched, g_sh_calls;
static const void *g_sh_void_md;

extern void shim_t_sh_state(void) __asm__("_shim_t_sh_state");

void shim_sh_state_get(void *out, void *self);
void shim_sh_state_get(void *out, void *self)
{
    uint64_t *w = (uint64_t *)out;
    (void)self;
    if (g_sh_calls < 40) {
        char b[48];
        g_sh_calls++;
        putnum(b, (unsigned long long)(uintptr_t)out);
        shim_log("sh-state-box", b);
    }
    if (!out || ((uintptr_t)out & 7)) return;
    if (!g_sh_void_md) g_sh_void_md = dlsym(RTLD_DEFAULT, "$sytN");
    w[0] = 0;
    w[1] = 0;
    w[2] = 0;
    w[3] = (uint64_t)(uintptr_t)g_sh_void_md;
}

static void sh_patch_getter(const void *wt, const void *type)
{
    const void **slots = (const void **)wt;
    const void *arg0;
    unsigned long kind;
    char b[64];

    if (!type || ((uintptr_t)type & 7)) return;
    kind = *(const unsigned long *)type;
    {
        const void *tdesc = *(const void *const *)((const char *)type
                                                   + ((kind > 0x800) ? 0x40 : 0x08));
        if (!has_sub(desc_name(tdesc), "DefaultStateHolder")) return;
    }
    arg0 = *(const void *const *)((const char *)type + ((kind > 0x800) ? 0x50 : 0x10));
    if (md_is_erased(arg0) != 2) return;
    if (!g_sh_zone) { g_sh_zone = 1; shim_log("sh-zone-open", "DefaultStateHolder<()>"); }
    if (!getenv("SHIM_BOX_STATE")) { shim_log("sh-patch-disabled", "skip"); return; }

    if (slots[SH_STATE_GETTER_SLOT] == (const void *)shim_t_sh_state) return;

    if (!g_sh_orig_getter) {
        g_sh_orig_getter = slots[SH_STATE_GETTER_SLOT];
        putnum(b, (unsigned long long)(uintptr_t)g_sh_orig_getter);
        shim_log("sh-orig-getter", b);
    } else if (slots[SH_STATE_GETTER_SLOT] != g_sh_orig_getter) {
        putnum(b, (unsigned long long)(uintptr_t)slots[SH_STATE_GETTER_SLOT]);
        shim_log("sh-slot-mismatch", b);
        return;
    }

    slots[SH_STATE_GETTER_SLOT] = (const void *)shim_t_sh_state;
    g_sh_patched++;
    putnum(b, (unsigned long long)(uintptr_t)wt);
    shim_log("sh-patched", b);
}

static const void *g_cdesc[256];
static unsigned char g_cverdict[256];

static int conf_interesting(const void *conf)
{
    unsigned h;
    const char *tn;
    if (!conf || ((uintptr_t)conf & 3) || (uintptr_t)conf < 0x100000000ULL) return 0;
    h = (unsigned)(((uintptr_t)conf >> 2) & 255);
    if (g_cdesc[h] == conf) return g_cverdict[h] == 1;
    tn = desc_name(conf_type(conf));
    g_cverdict[h] = has_sub(tn, "StateHolder") ? 1 : 2;
    g_cdesc[h] = conf;
    return g_cverdict[h] == 1;
}

static int genmd_interesting(const void *desc)
{
    unsigned h;
    const char *tn;
    if (!desc || ((uintptr_t)desc & 3) || (uintptr_t)desc < 0x100000000ULL) return 0;
    h = (unsigned)(((uintptr_t)desc >> 2) & 1023);
    if (g_gdesc[h] == desc) return g_gverdict[h] == 1;
    tn = desc_name(desc);
    g_gverdict[h] = (tn && (has_sub(tn, "ElementRuntime") || has_sub(tn, "StateHolder"))) ? 1 : 2;
    g_gdesc[h] = desc;
    return g_gverdict[h] == 1;
}

static void log_genmd_tag(const char *tag, const void *desc, const void *const *args,
                          const void *res, const void *lr)
{
    unsigned long long key;
    unsigned i, j = 0, nerased = 0;
    char b[400];
    const char *tn;

    tn = desc_name(desc);
    if (!tn) return;
    if (!has_sub(tn, "ElementRuntime") && !has_sub(tn, "StateHolder")) return;

    key = (unsigned long long)(uintptr_t)res * 131ull;
    for (i = 0; i < 4 && args; i++)
        key = key * 31ull + (unsigned long long)(uintptr_t)args[i];
    if (!key) key = 1;
    for (i = 0; i < 256; i++) {
        if (g_genseen[i] == key) return;
        if (!g_genseen[i]) { g_genseen[i] = key; break; }
    }
    if (g_genlog >= 120) return;
    g_genlog++;

    j = acat(b, j, tn);
    j = acat(b, j, " lr=");
    j = aimg(b, j, lr);
    j = acat(b, j, " md=");
    j = aimg(b, j, res);
    if (res && !((uintptr_t)res & 7)) {
        unsigned long rkind = *(const unsigned long *)res;
        unsigned lo = (rkind > 0x800) ? 0x50 : 0x10;
        unsigned hi = lo + 0x28;
        for (i = lo; i <= hi && j < ACAP - 60; i += 8) {
            const void *a = *(const void *const *)((const char *)res + i);
            if (!a || ((uintptr_t)a & 7) || (uintptr_t)a < 0x100000000ULL) continue;
            b[j++] = ' ';
            b[j++] = '+';
            j = anum(b, j, i);
            b[j++] = '=';
            if (md_is_erased(a)) { nerased++; j = acat(b, j, md_is_erased(a) == 2 ? "()" : "Any"); }
            else j = amdbrief(b, j, a);
        }
    }
    j = acat(b, j, " erased=");
    j = anum(b, j, nerased);
    b[j] = 0;
    shim_log(tag, b);
}

static void log_genmd(const void *desc, const void *const *args, const void *res,
                      const void *lr)
{
    log_genmd_tag("genmd", desc, args, res, lr);
}

MDResp shim_getgenericmd(unsigned long request, const void *const *args, const void *desc)
    __asm__("_swift_getGenericMetadata");
MDResp shim_getgenericmd(unsigned long request, const void *const *args, const void *desc)
{
    static GetGenericMD real;
    static int armed;
    MDResp r;

    if (!armed) {
        armed = 1;
        real = (GetGenericMD)dlsym(swiftcore(), "swift_getGenericMetadata");
        shim_log("genmd-armed", real ? "real" : "noreal");
    }
    /* our descriptors carry a null instantiation pattern, so the real one cannot
       build them - hand out our own identity instead */
    if (desc == (const void *)ntd_geo && args) {
        r.value = geo_md(args[0]);
        r.state = 0;
        return r;
    }
    if (!real) { r.value = 0; r.state = 0; return r; }
    r = real(request, args, desc);
    if (genmd_interesting(desc)) {
        void **fp = (void **)__builtin_frame_address(0);
        log_genmd(desc, args, r.value, fp ? fp[1] : 0);
    }
    return r;
}

typedef const void *(*AllocGenClass)(const void *, const void *, const void *);
typedef struct { const void *value; unsigned long state; } MDDep;
typedef MDDep (*InitClassMD2)(void *, unsigned long, unsigned long,
                              const void *const *, unsigned long *);

const void *shim_allocgenclass(const void *desc, const void *args, const void *pattern)
    __asm__("_swift_allocateGenericClassMetadata");
const void *shim_allocgenclass(const void *desc, const void *args, const void *pattern)
{
    static AllocGenClass real;
    static int armed;
    const void *r;

    if (!armed) {
        armed = 1;
        real = (AllocGenClass)dlsym(swiftcore(), "swift_allocateGenericClassMetadata");
        shim_log("agc-armed", real ? "real" : "noreal");
    }
    if (!real) return 0;
    r = real(desc, args, pattern);
    if (genmd_interesting(desc)) {
        void **fp = (void **)__builtin_frame_address(0);
        log_genmd_tag("genmd-alloc", desc, (const void *const *)args, r, fp ? fp[1] : 0);
    }
    return r;
}

MDDep shim_initclassmd2(void *self, unsigned long flags, unsigned long nfields,
                        const void *const *ftypes, unsigned long *foffsets)
    __asm__("_swift_initClassMetadata2");
MDDep shim_initclassmd2(void *self, unsigned long flags, unsigned long nfields,
                        const void *const *ftypes, unsigned long *foffsets)
{
    static InitClassMD2 real;
    static int armed;
    MDDep r;

    if (!armed) {
        armed = 1;
        real = (InitClassMD2)dlsym(swiftcore(), "swift_initClassMetadata2");
        shim_log("icm-armed", real ? "real" : "noreal");
    }
    if (!real) { r.value = 0; r.state = 0; return r; }
    r = real(self, flags, nfields, ftypes, foffsets);
    if (self && !((uintptr_t)self & 7)) {
        const void *desc = *(const void *const *)((const char *)self + 0x40);
        if (genmd_interesting(desc)) {
            void **fp = (void **)__builtin_frame_address(0);
            log_genmd_tag("genmd-init", desc, 0, self, fp ? fp[1] : 0);
        }
    }
    return r;
}

typedef const void *(*AllocGenValue)(const void *, const void *, const void *, unsigned long);
typedef MDDep (*UpdClassMD2)(void *, unsigned long, unsigned long,
                             const void *const *, unsigned long *);
typedef MDResp (*GetSingleton)(unsigned long, const void *);

const void *shim_allocgenvalue(const void *desc, const void *args, const void *pattern,
                               unsigned long extra)
    __asm__("_swift_allocateGenericValueMetadata");
const void *shim_allocgenvalue(const void *desc, const void *args, const void *pattern,
                               unsigned long extra)
{
    static AllocGenValue real;
    static int armed;
    const void *r;
    if (!armed) {
        armed = 1;
        real = (AllocGenValue)dlsym(swiftcore(), "swift_allocateGenericValueMetadata");
        shim_log("agv-armed", real ? "real" : "noreal");
    }
    if (!real) return 0;
    r = real(desc, args, pattern, extra);
    if (genmd_interesting(desc)) {
        void **fp = (void **)__builtin_frame_address(0);
        log_genmd_tag("genmd-aval", desc, (const void *const *)args, r, fp ? fp[1] : 0);
    }
    return r;
}

MDDep shim_updclassmd2(void *self, unsigned long flags, unsigned long nfields,
                       const void *const *ftypes, unsigned long *foffsets)
    __asm__("_swift_updateClassMetadata2");
MDDep shim_updclassmd2(void *self, unsigned long flags, unsigned long nfields,
                       const void *const *ftypes, unsigned long *foffsets)
{
    static UpdClassMD2 real;
    static int armed;
    MDDep r;
    if (!armed) {
        armed = 1;
        real = (UpdClassMD2)dlsym(swiftcore(), "swift_updateClassMetadata2");
        shim_log("ucm-armed", real ? "real" : "noreal");
    }
    if (!real) { r.value = 0; r.state = 0; return r; }
    r = real(self, flags, nfields, ftypes, foffsets);
    if (self && !((uintptr_t)self & 7)) {
        const void *desc = *(const void *const *)((const char *)self + 0x40);
        if (genmd_interesting(desc)) {
            void **fp = (void **)__builtin_frame_address(0);
            log_genmd_tag("genmd-upd", desc, 0, self, fp ? fp[1] : 0);
        }
    }
    return r;
}

MDResp shim_getsingleton(unsigned long request, const void *desc)
    __asm__("_swift_getSingletonMetadata");
MDResp shim_getsingleton(unsigned long request, const void *desc)
{
    static GetSingleton real;
    static int armed;
    MDResp r;
    if (!armed) {
        armed = 1;
        real = (GetSingleton)dlsym(swiftcore(), "swift_getSingletonMetadata");
        shim_log("sgl-armed", real ? "real" : "noreal");
    }
    if (!real) { r.value = 0; r.state = 0; return r; }
    r = real(request, desc);
    if (genmd_interesting(desc)) {
        void **fp = (void **)__builtin_frame_address(0);
        log_genmd_tag("genmd-sgl", desc, 0, r.value, fp ? fp[1] : 0);
    }
    return r;
}

typedef const void *(*GetAssocConf)(const void *, const void *, const void *,
                                    const void *, const void *);

static unsigned g_acolog;
static const void *g_acoseen[512];

static int aco_new(const void *t)
{
    unsigned i;
    for (i = 0; i < 512; i++) {
        if (g_acoseen[i] == t) return 0;
        if (!g_acoseen[i]) { g_acoseen[i] = t; return 1; }
    }
    return 1;
}

const void *shim_getassocconf(const void *wtable, const void *ctype, const void *atype,
                              const void *reqBase, const void *assocConf)
    __asm__("_swift_getAssociatedConformanceWitness");
const void *shim_getassocconf(const void *wtable, const void *ctype, const void *atype,
                              const void *reqBase, const void *assocConf)
{
    static GetAssocConf real;
    static int armed;
    const void *r;

    if (!armed) {
        armed = 1;
        real = (GetAssocConf)dlsym(swiftcore(), "swift_getAssociatedConformanceWitness");
        shim_log("aco-armed", real ? "real" : "noreal");
    }
    if (!real) return 0;
    r = real(wtable, ctype, atype, reqBase, assocConf);
    if (r && g_acolog < 300) {
        const void *conf = (wtable && !((uintptr_t)wtable & 7))
                           ? *(const void *const *)wtable : 0;
        if (conf_interesting(conf) && aco_new(r)) {
            char b[300];
            unsigned j = 0;
            g_acolog++;
            j = acat(b, j, "wt=");
            j = aimg(b, j, r);
            j = acat(b, j, " from=");
            j = aimg(b, j, wtable);
            j = acat(b, j, " ct=");
            j = amdbrief(b, j, ctype);
            j = acat(b, j, " at=");
            if (md_is_erased(atype) == 1) j = acat(b, j, "Any");
            else if (md_is_erased(atype) == 2) j = acat(b, j, "()");
            else j = amdbrief(b, j, atype);
            b[j] = 0;
            shim_log("wtaco", b);
        }
    }
    return r;
}


typedef struct { uint64_t a, b; } Str2;
extern void shim_call_subinit(void *out, uint64_t a, uint64_t b, void *fn);

void shim_regex_dynmember(void *out);
void shim_regex_dynmember(void *out)
{
    static void *strinit, *subinit;
    static int once;
    uint64_t *w = (uint64_t *)out;
    if (!out) return;
    if (!once) {
        once = 1;
        strinit = dlsym(RTLD_DEFAULT, "$sS2SycfC");
        subinit = dlsym(RTLD_DEFAULT, "$sSsySsSScfC");
        shim_log("regex-dynmember", (strinit && subinit) ? "real" : "fallback");
    }
    if (strinit && subinit) {
        Str2 s = ((Str2 (*)(void))strinit)();
        shim_call_subinit(out, s.a, s.b, subinit);
        return;
    }
    w[0] = 0; w[1] = 0; w[2] = 0; w[3] = 0xE000000000000000ULL;
}

#include <objc/runtime.h>
#include <objc/message.h>

extern id objc_retain(id);

id uikit_detent_custom(void *ident, void *fn, void *ctx)
    __asm__("_$sSo35UISheetPresentationControllerDetentC5UIKitE6custom10identifier8resolverABSo0abcD10IdentifieraSg_12CoreGraphics7CGFloatVSgSo0abcD17ResolutionContext_pctFZ");
id uikit_detent_custom(void *ident, void *fn, void *ctx)
{
    static int once;
    Class c = (Class)objc_getClass("UISheetPresentationControllerDetent");
    id d = c ? ((id (*)(id, SEL))objc_msgSend)((id)c, sel_registerName("largeDetent")) : 0;
    if (!once) {
        once = 1;
        shim_log("detent-custom", d ? "large" : "<none>");
    }
    return d ? objc_retain(d) : 0;
}


static void *vw_copy8(void *d, const void *s, const void *m)
{
    *(uint64_t *)d = *(const uint64_t *)s;
    return d;
}

static void *vw_buf_copy8(void *d, void *s, const void *m)
{
    *(uint64_t *)d = *(uint64_t *)s;
    return d;
}

static void vw_destroy8(void *p, const void *m)
{
}

static unsigned vw_get_tag_sp8(const void *p, unsigned n, const void *m)
{
    return 0;
}

static void vw_store_tag_sp8(void *p, unsigned w, unsigned n, const void *m)
{
}

static const VWT word_pod_vwt = {
    vw_buf_copy8, vw_destroy8, vw_copy8, vw_copy8, vw_copy8, vw_copy8,
    vw_get_tag_sp8, vw_store_tag_sp8, 8, 8, 7, 0,
    0, 0, 0
};

static const struct { const void *vwt; uint64_t kind; const void *desc; } word_pod_meta = {
    &word_pod_vwt, 0x200, 0
};

typedef struct { const void *vwt; uint64_t kind; const void *desc; } MDRec;

extern const char ntd_langcode[] __asm__("_$s10Foundation6LocaleV12LanguageCodeVMn");
extern const char ntd_region[] __asm__("_$s10Foundation6LocaleV6RegionVMn");
extern const char ntd_script[] __asm__("_$s10Foundation6LocaleV6ScriptVMn");
extern const char ntd_currency[] __asm__("_$s10Foundation6LocaleV8CurrencyVMn");
extern const char ntd_unit[] __asm__("_$ss8DurationV10FoundationE16UnitsFormatStyleV4UnitVMn");
extern const char ntd_sizing[] __asm__("_$s7SwiftUI32UIHostingControllerSizingOptionsVMn");

static const void *string_vwt(void)
{
    static const void *v;
    if (!v) {
        const void *md = dlsym(RTLD_DEFAULT, "$sSSN");
        v = md ? *(const void *const *)((const char *)md - 8) : (const void *)&word_pod_vwt;
    }
    return v;
}

static void *init_md(MDRec *m, const void *vwt, const void *desc, const char *tag)
{
    if (!m->kind) {
        m->vwt = vwt;
        m->kind = 0x200;
        m->desc = desc;
        shim_log("ntd", tag);
    }
    return (void *)&m->kind;
}

#define NTD_ACC(fn, sym, rec, vwt, desc, tag)                           static MDRec rec;                                                   MetaResponse fn(uint64_t request) __asm__(sym);                     MetaResponse fn(uint64_t request)                                   {                                                                       MetaResponse r = { init_md(&rec, vwt, desc, tag), 0 };              return r;                                                       }

NTD_ACC(acc_langcode, "_$s10Foundation6LocaleV12LanguageCodeVMa", g_md_langcode,
        string_vwt(), ntd_langcode, "LanguageCode")
NTD_ACC(acc_region, "_$s10Foundation6LocaleV6RegionVMa", g_md_region,
        string_vwt(), ntd_region, "Region")
NTD_ACC(acc_script, "_$s10Foundation6LocaleV6ScriptVMa", g_md_script,
        string_vwt(), ntd_script, "Script")
NTD_ACC(acc_currency, "_$s10Foundation6LocaleV8CurrencyVMa", g_md_currency,
        string_vwt(), ntd_currency, "Currency")
NTD_ACC(acc_unit, "_$ss8DurationV10FoundationE16UnitsFormatStyleV4UnitVMa", g_md_unit,
        &word_pod_vwt, ntd_unit, "Unit")
NTD_ACC(hc_sizing_metadata, "_$s7SwiftUI32UIHostingControllerSizingOptionsVMa", g_md_sizing,
        &word_pod_vwt, ntd_sizing, "SizingOptions")

/* ios 15 has no Locale.region, and it is returned INDIRECTLY through x8.
   gen_shim.py's logging stubs only set x0/x1, so the stub left spotify's 16-byte
   stack buffer holding garbage; spotify read the optional's tag out of that
   (`swift_getEnumTagSinglePayload(buf, 1, RegionVMa)`), took the .some branch and
   destroyed a String whose _object was junk -> `swift_release` fault. so write a
   real `.none`, using the same value witness table the caller reads the tag with,
   which keeps the write and the read self-consistent.

   WATCH FOR THIS SHAPE: any `unimpl` symbol whose return is indirect has the same
   problem, and a stub is worse than nothing there - it looks like it returned a
   value. `Locale.language` (two calls earlier at Spotify+84b2598, also sret) is
   the next candidate; it has not bitten yet only because its own metadata
   accessor is a stub returning 0, so nothing destroys the result. */

typedef void (*StoreTagSP)(void *, unsigned, unsigned, const void *);

void shim_locale_region(void *out) __asm__("_shim_locale_region");
void shim_locale_region(void *out)
{
    static unsigned n;
    const void *const *vwt = (const void *const *)string_vwt();
    const void *md = acc_region(0).metadata;
    unsigned long size;
    if (!out || !vwt || !md) return;
    size = (unsigned long)(uintptr_t)vwt[8];
    if (size && size <= 64) __builtin_memset(out, 0, size);
    {
        StoreTagSP st = (StoreTagSP)vwt[7];
        if (st) st(out, 1, 1, md);
    }
    if (n < 8) {
        n++;
        shim_log("locale-region", "nil");
    }
}

extern void objc_release(id);

/* the value in a UIHostingConfiguration is not always an objc box: spotify also
   puts swift-native boxes (an AnyView, say) through here, and objc_retain on a
   swift object writes its extra_rc into word 0 - the metadata word - which later
   kills the object's dealloc with a corrupt-metadata fault. the unknownObject
   entry points pick the right refcounting for either kind. */

extern id swift_unknownObjectRetain(id);
extern void swift_unknownObjectRelease(id);

static id ur_retain(id o)
{
    return o ? swift_unknownObjectRetain(o) : o;
}

static void ur_release(id o)
{
    if (o) swift_unknownObjectRelease(o);
}

static void *vw_obj_buf_copy(void *d, void *s, const void *m)
{
    (void)m;
    *(id *)d = ur_retain(*(id *)s);
    return d;
}

static void vw_obj_destroy(void *p, const void *m)
{
    (void)m;
    ur_release(*(id *)p);
}

static void *vw_obj_init_copy(void *d, const void *s, const void *m)
{
    (void)m;
    *(id *)d = ur_retain(*(id const *)s);
    return d;
}

static void *vw_obj_assign_copy(void *d, const void *s, const void *m)
{
    id old = *(id *)d;
    (void)m;
    *(id *)d = ur_retain(*(id const *)s);
    ur_release(old);
    return d;
}

static void *vw_obj_init_take(void *d, const void *s, const void *m)
{
    (void)m;
    *(id *)d = *(id const *)s;
    return d;
}

static void *vw_obj_assign_take(void *d, const void *s, const void *m)
{
    id old = *(id *)d;
    (void)m;
    *(id *)d = *(id const *)s;
    ur_release(old);
    return d;
}

static const VWT obj_vwt = {
    vw_obj_buf_copy, vw_obj_destroy, vw_obj_init_copy, vw_obj_assign_copy,
    vw_obj_init_take, vw_obj_assign_take,
    vw_get_tag_sp8, vw_store_tag_sp8, 8, 8, 0x10007, 0,
    0, 0, 0
};

extern const char ntd_hostcfg[] __asm__("_$s7SwiftUI22UIHostingConfigurationVMn");

/* UIHostingConfiguration<Content, Background> is generic in two parameters, so a
   non-generic descriptor makes every bound-generic mangled name for it fail to
   demangle, and the caller then stores a null type in the existential - which is
   what made destroying it read a wild vwt. making the descriptor generic is NOT
   the answer: a metadata we allocate ourselves has no entry in the runtime's
   generic cache, so swift_checkMetadataState never sees it complete and spins in
   a libdispatch wait forever. so the descriptor stays non-generic (which the
   runtime treats as complete on sight) and the name is satisfied in our own
   swift_getTypeByMangledName hooks instead - see hostcfg_byname(). the extra two
   words are left zero so anything reading past the descriptor sees nothing. */

static uint64_t g_hcmd[5];

static const void *hostcfg_md(void)
{
    if (!g_hcmd[1]) {
        g_hcmd[0] = (uint64_t)(uintptr_t)&word_pod_vwt;
        g_hcmd[1] = 0x200;
        g_hcmd[2] = (uint64_t)(uintptr_t)ntd_hostcfg;
        shim_log("ntd", "UIHostingConfiguration");
    }
    return (const void *)&g_hcmd[1];
}

MetaResponse hostcfg_metadata(uint64_t request)
    __asm__("_$s7SwiftUI22UIHostingConfigurationVMa");
MetaResponse hostcfg_metadata(uint64_t request)
{
    MetaResponse r;
    (void)request;
    r.metadata = (void *)hostcfg_md();
    r.state = 0;
    return r;
}

/* ios 15 has no _GeometryActionModifier at all. rather than synthesize a type -
   a hand-rolled generic metadata is not in the runtime's generic cache, so
   swift_checkMetadataState never sees it complete and spins forever - the
   descriptor's access function hands back SwiftUI's own EmptyModifier. that is
   the identity ViewModifier, fully present on ios 15: real metadata, real
   completion, a real ViewModifier conformance whose _makeView / _makeViewList /
   _viewListCount pass the content straight through without ever looking at the
   modifier value. so the view renders and the geometry action simply never
   fires, and every runtime object on the path is genuine. the collapse of
   _GeometryActionModifier<A> to one identity for every A is harmless for a
   modifier that does nothing. */

extern void swift_release(void *);

extern const char ntd_geo[] __asm__("_$s7SwiftUI23_GeometryActionModifierVMn");

static const void *geo_md(const void *arg0)
{
    static const void *md;
    static int armed;
    (void)arg0;
    if (!armed) {
        MetaResponse (*acc)(uint64_t);
        armed = 1;
        acc = (MetaResponse (*)(uint64_t))dlsym(RTLD_DEFAULT,
                                                "$s7SwiftUI13EmptyModifierVMa");
        if (acc) md = acc(0).metadata;
        shim_log("geo-md", md ? "emptymodifier" : "missing");
    }
    return md;
}

MetaResponse acc_geo(uint64_t request, const void *arg0)
    __asm__("_$s7SwiftUI23_GeometryActionModifierVMa");
MetaResponse acc_geo(uint64_t request, const void *arg0)
{
    MetaResponse r;
    (void)request;
    r.metadata = (void *)geo_md(arg0);
    r.state = 0;
    return r;
}

/* init(value:action:). the modifier is EmptyModifier, so there is nothing to
   store - x8 points at a zero-byte temporary. the two escaping closures arrive
   owned, so release them here or they leak. */
void shim_geo_init(void *vfn, void *vctx, void *afn, void *actx, const void *mda,
                   void *out) __asm__("_shim_geo_init");
void shim_geo_init(void *vfn, void *vctx, void *afn, void *actx, const void *mda,
                   void *out)
{
    static unsigned n;
    (void)vfn;
    (void)afn;
    (void)mda;
    (void)out;
    if (vctx) swift_release(vctx);
    if (actx) swift_release(actx);
    if (n < 8) {
        n++;
        shim_log("geo-init", "noop");
    }
}

extern const char geo_conf[] __asm__("_$s7SwiftUI23_GeometryActionModifierVyxGAA04ViewE0AAMc");

static const void *geo_witness_table(void)
{
    static const void *wt;
    static int armed;
    if (!armed) {
        armed = 1;
        wt = dlsym(RTLD_DEFAULT, "$s7SwiftUI13EmptyModifierVAA04ViewD0AAWP");
        shim_log("geo-wt", wt ? "emptymodifier" : "missing");
    }
    return wt;
}

extern const void *shim_wt_cfg[9] __asm__("_shim_wt_cfg");
extern const void *shim_wt_view[9] __asm__("_shim_wt_view");

extern void shim_t_copyself_ret(void) __asm__("_shim_t_copyself_ret");
extern void shim_t_makeview(void) __asm__("_shim_t_makeview");
extern void shim_t_view_get(void) __asm__("_shim_t_view_get");
extern void shim_t_view_set(void) __asm__("_shim_t_view_set");

static uint64_t wt_slot_stub(void)
{
    shim_log("hostcfg-wtstub", "called");
    return 0;
}

static unsigned thunk_slot(const char *sym)
{
    const uint32_t *p = (const uint32_t *)dlsym(RTLD_DEFAULT, sym);
    unsigned i;
    if (!p) return 0;
    for (i = 0; i < 10; i++) {
        uint32_t w = p[i];
        if ((w & 0xffc00000u) == 0xf9400000u) {
            unsigned imm = ((w >> 10) & 0xfff) * 8;
            if (imm) return imm;
        }
        if ((w & 0xfffffc1fu) == 0xd61f0000u) break;
    }
    return 0;
}

static void wt_put(const void **wt, unsigned byteoff, const void *fn, const char *tag)
{
    char b[48];
    unsigned idx = byteoff / 8;
    putnum(b, byteoff);
    if (!byteoff || idx >= 9) {
        shim_log("hostcfg-badslot", b);
        return;
    }
    wt[idx] = fn;
    shim_log(tag, b);
}

static int g_hostcfg_ready;

static void hostcfg_setup(void)
{
    unsigned i;
    if (g_hostcfg_ready) return;
    g_hostcfg_ready = 1;
    for (i = 1; i < 9; i++) {
        shim_wt_cfg[i] = (const void *)wt_slot_stub;
        shim_wt_view[i] = (const void *)wt_slot_stub;
    }
    wt_put(shim_wt_cfg,
           thunk_slot("$s5UIKit22UIContentConfigurationP15makeContentViewAA0bF0_So6UIViewCXcyFTj"),
           (const void *)shim_t_makeview, "hostcfg-slot-makeview");
    wt_put(shim_wt_cfg,
           thunk_slot("$s5UIKit22UIContentConfigurationP7updated3forxAA20UIConfigurationState_p_tFTj"),
           (const void *)shim_t_copyself_ret, "hostcfg-slot-updated");
    wt_put(shim_wt_view,
           thunk_slot("$s5UIKit13UIContentViewP13configurationAA0B13Configuration_pvgTj"),
           (const void *)shim_t_view_get, "hostcfg-slot-cfgget");
    wt_put(shim_wt_view,
           thunk_slot("$s5UIKit13UIContentViewP13configurationAA0B13Configuration_pvsTj"),
           (const void *)shim_t_view_set, "hostcfg-slot-cfgset");
    wt_put(shim_wt_view,
           thunk_slot("$s5UIKit13UIContentViewP13configurationAA0B13Configuration_pvMTj"),
           (const void *)wt_slot_stub, "hostcfg-slot-cfgmod");
}

/* stage 2: host the real content.

   stage 1 never called the content closure, so every cell got an empty view -
   41 of them on one device-picker open, which is exactly what an empty device
   list looks like. the closure is non-escaping, so it has to run inside
   init(content:).

   the configuration word stays ZERO, which is the only value stage 1 ever
   proved safe: libswiftUIKit's existential destroy swift_releases whatever is
   in it (see 8e cause 3, and the 2026-10-19 crash that put a cell pointer
   there). so the cell travels from init(content:) to makeContentView through
   g_pending instead - they run back to back in one call chain, see
   Spotify+0x85be810. the cell holds a retained UIHostingController<Content>
   and is never freed; one cell per cfg, capped.

   every convention here was read off spotify's own call sites and SwiftUI's own
   code, not assumed:
     init(content:)  x0=closure fn  x1=closure ctx  x2=Content metadata
                     x3=Content:View witness table  x8=sret
     the closure     x8=result buffer  x20=captures (ldp x0,x1,[x20,#0x10])
     UIHostingControllerCMa(request, Content md, Content:View wt)
     init(rootView:) x0=&rootView (@in, consumed)  x20=class metadata
   self arrives as a POINTER in x20 everywhere, including through the direct
   method symbols, because spotify's callers are generic over Content and so
   the configuration is address-only to them. */

typedef struct { double w, h; } ShSize;
typedef struct { double x, y, w, h; } ShRect;
typedef struct { void *hc; } CfgCell;

extern void free(void *);
extern void shim_call_mk(void *out, void *ctx, void *fn) __asm__("_shim_call_mk");
extern void *shim_call_hcinit(void *arg, const void *md, void *fn) __asm__("_shim_call_hcinit");

static id msg0(id o, const char *s)
{
    return o ? ((id (*)(id, SEL))objc_msgSend)(o, sel_registerName(s)) : 0;
}

static unsigned long msg_count(id o)
{
    return o ? ((unsigned long (*)(id, SEL))objc_msgSend)(o, sel_registerName("count")) : 0;
}

static id msg_at(id o, unsigned long i)
{
    return o ? ((id (*)(id, SEL, unsigned long))objc_msgSend)(o,
                   sel_registerName("objectAtIndex:"), i) : 0;
}

/* the cell height comes from here when the hosted view has no intrinsic size
   of its own. width stays unconstrained either way - the row is as wide as the
   table gives it. */
static ShSize scv_intrinsic(id self, SEL cmd)
{
    static int logged;
    ShSize r;
    id subs, hv;
    r.w = -1; r.h = -1;
    (void)cmd;
    subs = msg0(self, "subviews");
    if (!msg_count(subs)) return r;
    hv = msg_at(subs, 0);
    if (!hv) return r;
    r = ((ShSize (*)(id, SEL))objc_msgSend)(hv, sel_registerName("intrinsicContentSize"));
    if (r.h > 0) {
        if (!logged) { logged = 1; shim_log("hostcfg-size", "intrinsic"); }
        return r;
    }
    {
        ShRect bb = ((ShRect (*)(id, SEL))objc_msgSend)(self, sel_registerName("bounds"));
        ShSize fit;
        fit.w = bb.w > 0 ? bb.w : 375;
        fit.h = 0;
        r = ((ShSize (*)(id, SEL, ShSize))objc_msgSend)(hv,
                sel_registerName("sizeThatFits:"), fit);
    }
    if (!logged) {
        char b[32];
        logged = 1;
        putnum(b, (unsigned long long)(r.h > 0 ? (uint64_t)r.h : 0));
        shim_log("hostcfg-size fits-h", b);
    }
    if (r.h <= 0) { r.w = -1; r.h = -1; return r; }
    r.w = -1;
    return r;
}

static Class shim_view_class(void)
{
    static Class c;
    static int tried;
    if (!tried) {
        Class sup = objc_getClass("UIView");
        tried = 1;
        c = objc_getClass("ShimContentView");
        if (!c && sup) {
            c = objc_allocateClassPair(sup, "ShimContentView", 0);
            if (c) {
                class_addMethod(c, sel_registerName("intrinsicContentSize"),
                                (IMP)scv_intrinsic, "{CGSize=dd}16@0:8");
                objc_registerClassPair(c);
            }
        }
        shim_log("hostcfg-viewclass", c ? "ok" : "FAILED");
    }
    return c;
}

static unsigned g_cells;
static void *g_pending;

static unsigned ahex(char *b, unsigned j, uint64_t v)
{
    int i;
    int lead = 1;
    for (i = 60; i >= 0; i -= 4) {
        unsigned d = (unsigned)((v >> i) & 0xf);
        if (lead && d == 0 && i) continue;
        lead = 0;
        b[j++] = (char)(d < 10 ? '0' + d : 'a' + d - 10);
    }
    return j;
}

static const void *hc_metadata(const void *content_md, const void *view_wt)
{
    static MetaResponse (*acc)(uint64_t, const void *, const void *);
    static int tried;
    if (!tried) {
        tried = 1;
        acc = (MetaResponse (*)(uint64_t, const void *, const void *))
                  dlsym(RTLD_DEFAULT, "$s7SwiftUI19UIHostingControllerCMa");
        shim_log("hostcfg-hcma", acc ? "ok" : "missing");
    }
    if (!acc) return 0;
    return acc(0, content_md, view_wt).metadata;
}

static void *cfg_cell_new(void *fn, void *ctx, const void *content_md, const void *view_wt)
{
    static void *initfn;
    static int tried;
    const VWT *vwt;
    const void *hcmd;
    CfgCell *cell;
    void *buf, *hc;
    uint64_t sz;

    if (!tried) {
        tried = 1;
        initfn = dlsym(RTLD_DEFAULT,
                       "$s7SwiftUI19UIHostingControllerC8rootViewACyxGx_tcfC");
        shim_log("hostcfg-hcinit", initfn ? "ok" : "missing");
    }
    if (!fn || !content_md || !initfn) return 0;
    if (g_cells >= 8192) {
        shim_log("hostcfg-init", "cellcap");
        return 0;
    }
    vwt = *(const VWT *const *)((const char *)content_md - 8);
    if (!vwt) return 0;
    sz = vwt->size;
    if (sz > 8192) {
        shim_log("hostcfg-init", "content-too-big");
        return 0;
    }
    hcmd = hc_metadata(content_md, view_wt);
    if (!hcmd) return 0;
    buf = malloc(sz < 8 ? 8 : (unsigned long)sz);
    if (!buf) return 0;
    shim_call_mk(buf, ctx, fn);
    hc = shim_call_hcinit(buf, hcmd, initfn);
    free(buf);
    if (!hc) {
        shim_log("hostcfg-init", "hc-null");
        return 0;
    }
    cell = (CfgCell *)malloc(sizeof *cell);
    if (!cell) return 0;
    cell->hc = hc;
    g_cells++;
    if (g_cells < 4) {
        char b[64];
        unsigned j = 0;
        j = acat(b, j, "cell=");
        j = ahex(b, j, (uint64_t)(uintptr_t)cell);
        j = acat(b, j, " hc=");
        j = ahex(b, j, (uint64_t)(uintptr_t)hc);
        b[j] = 0;
        shim_log("hostcfg-cell", b);
    }
    return cell;
}

static void cfg_attach(id container, id hv)
{
    static const char *const ax[4] = {
        "leadingAnchor", "trailingAnchor", "topAnchor", "bottomAnchor"
    };
    unsigned i;
    if (!container || !hv) return;
    ((void (*)(id, SEL, bool))objc_msgSend)(hv,
        sel_registerName("setTranslatesAutoresizingMaskIntoConstraints:"), 0);
    ((void (*)(id, SEL, id))objc_msgSend)(container,
        sel_registerName("addSubview:"), hv);
    ((void (*)(id, SEL, id))objc_msgSend)(hv,
        sel_registerName("setBackgroundColor:"),
        msg0((id)objc_getClass("UIColor"), "clearColor"));
    for (i = 0; i < 4; i++) {
        id a = msg0(hv, ax[i]);
        id b = msg0(container, ax[i]);
        id c;
        if (!a || !b) continue;
        c = ((id (*)(id, SEL, id))objc_msgSend)(a,
                sel_registerName("constraintEqualToAnchor:"), b);
        if (!c) continue;
        /* the bottom pin yields rather than fighting a height the table has
           already decided */
        if (i == 3)
            ((void (*)(id, SEL, float))objc_msgSend)(c,
                sel_registerName("setPriority:"), 999.0f);
        ((void (*)(id, SEL, bool))objc_msgSend)(c, sel_registerName("setActive:"), 1);
    }
}

void shim_cfg_init(void *fn, void *ctx, const void *content_md, const void *view_wt,
                   void **out);
void shim_cfg_init(void *fn, void *ctx, const void *content_md, const void *view_wt,
                   void **out)
{
    void *cell;
    hostcfg_setup();
    if (!out || ((uintptr_t)out & 7)) {
        shim_log("hostcfg-init", "bad-sret");
        return;
    }
    cell = cfg_cell_new(fn, ctx, content_md, view_wt);
    g_pending = cell;
    *out = 0;
    shim_log("hostcfg-init", cell ? "hosted" : "empty");
}

void shim_cfg_copyself(void **out, void **self);
void shim_cfg_copyself(void **out, void **self)
{
    hostcfg_setup();
    if (!out || ((uintptr_t)out & 7)) {
        shim_log("hostcfg-copyself", "bad-sret");
        return;
    }
    *out = (self && !((uintptr_t)self & 7)) ? *self : 0;
}

typedef struct { void *obj; const void *wt; } ClassExist;

ClassExist shim_cfg_makeview_c(void *self);
ClassExist shim_cfg_makeview_c(void *self)
{
    ClassExist r;
    CfgCell *cell;
    Class c;
    id v = 0;
    hostcfg_setup();
    cell = (CfgCell *)g_pending;
    g_pending = 0;
    (void)self;
    c = shim_view_class();
    if (c) {
        v = msg0((id)c, "alloc");
        v = msg0(v, "init");
    }
    if (v && cell) cfg_attach(v, msg0((id)cell->hc, "view"));
    r.obj = (void *)v;
    r.wt = (const void *)shim_wt_view;
    shim_log("hostcfg-makeview", v ? (cell ? "hosted" : "empty") : "FAILED");
    return r;
}

void shim_view_cfg_get(void *out, id self);
void shim_view_cfg_get(void *out, id self)
{
    uint64_t *w = (uint64_t *)out;
    (void)self;
    hostcfg_setup();
    if (!out || ((uintptr_t)out & 7)) return;
    w[0] = 0; w[1] = 0; w[2] = 0;
    w[3] = (uint64_t)(uintptr_t)hostcfg_md();
    w[4] = (uint64_t)(uintptr_t)shim_wt_cfg;
    shim_log("hostcfg-view-get", "empty-cfg");
}

void shim_view_cfg_set(const void *in, id self);
void shim_view_cfg_set(const void *in, id self)
{
    (void)in; (void)self;
    shim_log("hostcfg-view-set", "ignored");
}

void shim_hc_safearea_setter(void)
    __asm__("_$s7SwiftUI19UIHostingControllerC15safeAreaRegionsAA04SafefG0Vvs");
void shim_hc_safearea_setter(void)
{
    shim_log("hostcfg-hc-safearea", "noop");
}

uint64_t unit_hours(void)
    __asm__("_$ss8DurationV10FoundationE16UnitsFormatStyleV4UnitV5hoursAGvgZ");
uint64_t unit_hours(void)
{
    return 0;
}

uint64_t unit_minutes(void)
    __asm__("_$ss8DurationV10FoundationE16UnitsFormatStyleV4UnitV7minutesAGvgZ");
uint64_t unit_minutes(void)
{
    return 1;
}

uint64_t unit_seconds(void)
    __asm__("_$ss8DurationV10FoundationE16UnitsFormatStyleV4UnitV7secondsAGvgZ");
uint64_t unit_seconds(void)
{
    return 2;
}

uint64_t hc_sizing_intrinsic(void)
    __asm__("_$s7SwiftUI32UIHostingControllerSizingOptionsV20intrinsicContentSizeACvgZ");
uint64_t hc_sizing_intrinsic(void)
{
    return 1;
}

void hc_set_sizing(uint64_t opts, void *self)
    __asm__("_$s7SwiftUI19UIHostingControllerC13sizingOptionsAA0cd6SizingF0VvsTj");
void hc_set_sizing(uint64_t opts, void *self)
{
    static int once;
    if (!once) {
        once = 1;
        shim_log("hc-sizingoptions", "noop");
    }
}


MetaResponse tfs_metadata(uint64_t request)
    __asm__("_$ss8DurationV10FoundationE15TimeFormatStyleVMa");
MetaResponse tfs_metadata(uint64_t request)
{
    MetaResponse r = { (void *)&word_pod_meta.kind, 0 };
    return r;
}

MetaResponse tfs_pattern_metadata(uint64_t request)
    __asm__("_$ss8DurationV10FoundationE15TimeFormatStyleV7PatternVMa");
MetaResponse tfs_pattern_metadata(uint64_t request)
{
    MetaResponse r = { (void *)&word_pod_meta.kind, 0 };
    return r;
}

uint64_t tfs_pattern_ms(void)
    __asm__("_$ss8DurationV10FoundationE15TimeFormatStyleV7PatternV12minuteSecondAGvgZ");
uint64_t tfs_pattern_ms(void)
{
    return 1;
}

uint64_t tfs_pattern_hms(void)
    __asm__("_$ss8DurationV10FoundationE15TimeFormatStyleV7PatternV16hourMinuteSecondAGvgZ");
uint64_t tfs_pattern_hms(void)
{
    return 2;
}

uint64_t tfs_time(uint64_t pattern)
    __asm__("_$s10Foundation11FormatStylePAAs8DurationVAAE04TimebC0VRszrlE4time7patternA2G7PatternV_tFZ");
uint64_t tfs_time(uint64_t pattern)
{
    return pattern ? pattern : 1;
}

typedef Str2 (*StrFromC)(const char *);

static char *put_u(char *p, uint64_t v, int pad)
{
    char t[24];
    int n = 0;
    if (!v) t[n++] = '0';
    while (v) { t[n++] = (char)('0' + (v % 10)); v /= 10; }
    while (n < pad) t[n++] = '0';
    while (n) *p++ = t[--n];
    return p;
}

void shim_dur_formatted(uint64_t lo, int64_t hi, const uint64_t *style,
                        const void *m, const void *wt, void *out);
void shim_dur_formatted(uint64_t lo, int64_t hi, const uint64_t *style,
                        const void *m, const void *wt, void *out)
{
    static StrFromC mk;
    static int once;
    Dur d;
    DurComp c;
    char buf[48], *p = buf;
    uint64_t total, h, mi, se;
    uint64_t pat = (style && sane_ptr(style)) ? *style : 1;
    if (!out) return;
    if (!once) {
        once = 1;
        mk = (StrFromC)dlsym(RTLD_DEFAULT, "$sSS7cStringSSSPys4Int8VG_tcfC");
        shim_log("dur-formatted", mk ? "real" : "nostr");
    }
    d.low = lo;
    d.high = hi;
    c = dur_components(d);
    total = (uint64_t)(c.seconds < 0 ? -c.seconds : c.seconds);
    h = total / 3600;
    mi = (total / 60) % 60;
    se = total % 60;
    if (c.seconds < 0) *p++ = '-';
    if (pat == 2 || h) {
        p = put_u(p, h, 1);
        *p++ = ':';
        p = put_u(p, mi, 2);
    } else {
        p = put_u(p, mi, 1);
    }
    *p++ = ':';
    p = put_u(p, se, 2);
    *p = 0;
    if (mk) {
        Str2 r = mk(buf);
        ((uint64_t *)out)[0] = r.a;
        ((uint64_t *)out)[1] = r.b;
    } else {
        ((uint64_t *)out)[0] = 0;
        ((uint64_t *)out)[1] = 0;
    }
}

extern void objc_release(id);
extern void *shim_call_self0(void *selfp, void *fn);
extern void shim_call_unbridge(void *res, void *nsobj, void *fn);

static void *g_urlbr;
static void *g_urlunbr;
static void *g_arrbr;
static void *g_qimeta;
static int g_qi_init;

void shim_url_queryitems(void *res, void *arr, void *selfurl);
void shim_url_queryitems(void *res, void *arr, void *selfurl)
{
    if (!g_qi_init) {
        g_qi_init = 1;
        g_urlbr = dlsym(RTLD_DEFAULT, "$s10Foundation3URLV19_bridgeToObjectiveCSo5NSURLCyF");
        g_urlunbr = dlsym(RTLD_DEFAULT,
            "$s10Foundation3URLV36_unconditionallyBridgeFromObjectiveCyACSo5NSURLCSgFZ");
        g_arrbr = dlsym(RTLD_DEFAULT, "$sSa10FoundationE19_bridgeToObjectiveCSo7NSArrayCyF");
        g_qimeta = dlsym(RTLD_DEFAULT, "$s10Foundation12URLQueryItemVN");
        shim_log("url-queryitems",
            (g_urlbr && g_urlunbr && g_arrbr && g_qimeta) ? "ready" : "incomplete");
    }
    if (!res || !selfurl || !g_urlbr || !g_urlunbr) {
        shim_unimpl("shim_url_queryitems-unhandled");
        return;
    }

    id nsurl = (id)shim_call_self0(selfurl, g_urlbr);
    if (!nsurl) {
        shim_unimpl("shim_url_queryitems-nourl");
        return;
    }

    id items = 0;
    if (arr && g_arrbr && g_qimeta)
        items = ((id (*)(void *, void *))g_arrbr)(arr, g_qimeta);

    unsigned long n = 0;
    if (items)
        n = ((unsigned long (*)(id, SEL))objc_msgSend)(items, sel_registerName("count"));

    id out = 0;
    if (n) {
        Class cc = (Class)objc_getClass("NSURLComponents");
        id comp = cc ? ((id (*)(id, SEL, id, bool))objc_msgSend)((id)cc,
                           sel_registerName("componentsWithURL:resolvingAgainstBaseURL:"),
                           nsurl, false)
                     : 0;
        if (comp) {
            id merged = items;
            id old = ((id (*)(id, SEL))objc_msgSend)(comp, sel_registerName("queryItems"));
            if (old && ((unsigned long (*)(id, SEL))objc_msgSend)(old, sel_registerName("count")))
                merged = ((id (*)(id, SEL, id))objc_msgSend)(old,
                             sel_registerName("arrayByAddingObjectsFromArray:"), items);
            ((void (*)(id, SEL, id))objc_msgSend)(comp, sel_registerName("setQueryItems:"), merged);
            out = ((id (*)(id, SEL))objc_msgSend)(comp, sel_registerName("URL"));
        }
        if (!out)
            shim_log("url-queryitems", "components-failed");
    }

    shim_call_unbridge(res, out ? out : nsurl, g_urlunbr);
    objc_release(nsurl);
    if (items)
        objc_release(items);
}

#include "xj_gen.h"

extern const void *_dyld_get_image_header(uint32_t);
extern long _dyld_get_image_vmaddr_slide(uint32_t);
extern uint32_t _dyld_image_count(void);

typedef const void *(*GetExistMD)(unsigned long, const void *, unsigned long,
                                  const uintptr_t *);
typedef const void *(*ConformsTo)(const void *, const void *);

static int sha_streq(const char *a, const char *b)
{
    if (!a || !b) return 0;
    while (*a && *a == *b) { a++; b++; }
    return *a == *b;
}

static int sha_sect16(const char *p, const char *name)
{
    unsigned i = 0;
    for (; i < 16; i++) {
        char c = name[i];
        if (!c) break;
        if (p[i] != c) return 0;
    }
    return i == 16 || p[i] == 0;
}

static const char *sha_descname(const char *d)
{
    int32_t rel;
    __builtin_memcpy(&rel, d + 8, 4);
    if (!rel || rel > 0x4000000 || rel < -0x4000000) return 0;
    return d + 8 + rel;
}

static const void *sha_scan(const char *mh, long slide, unsigned *nrec)
{
    uint32_t ncmds, i;
    const char *lc;
    __builtin_memcpy(&ncmds, mh + 16, 4);
    lc = mh + 32;
    for (i = 0; i < ncmds && i < 4096; i++) {
        uint32_t cmd, cs;
        __builtin_memcpy(&cmd, lc, 4);
        __builtin_memcpy(&cs, lc + 4, 4);
        if (cs < 8) break;
        if (cmd == 0x19) {
            uint32_t nsects, k;
            const char *sect = lc + 72;
            __builtin_memcpy(&nsects, lc + 64, 4);
            for (k = 0; k < nsects && k < 512; k++, sect += 80) {
                uint64_t addr, size, o;
                const char *base;
                if (!sha_sect16(sect, "__swift5_protos")) continue;
                __builtin_memcpy(&addr, sect + 32, 8);
                __builtin_memcpy(&size, sect + 40, 8);
                base = (const char *)(uintptr_t)(addr + (uint64_t)slide);
                for (o = 0; o + 4 <= size; o += 4) {
                    int32_t rel;
                    const char *d;
                    __builtin_memcpy(&rel, base + o, 4);
                    if (!rel) continue;
                    d = base + o + (rel & ~1);
                    if (rel & 1) d = *(const char *const *)d;
                    if (!d || ((uintptr_t)d & 3)) continue;
                    (*nrec)++;
                    if (sha_streq(sha_descname(d), SHA_PROTO)) return d;
                }
            }
        }
        lc += cs;
    }
    return 0;
}

/* the protocol descriptor, found by name in the main executable's protocol records */
static const void *sha_proto(void)
{
    static const void *g;
    static int done;
    uint32_t n, i;
    unsigned nrec = 0;
    char b[64];
    if (done) return g;
    n = _dyld_image_count();
    for (i = 0; i < n && !g; i++) {
        const char *mh = (const char *)_dyld_get_image_header(i);
        uint32_t ft;
        if (!mh) continue;
        __builtin_memcpy(&ft, mh + 12, 4);
        if (ft != 2) continue;
        g = sha_scan(mh, _dyld_get_image_vmaddr_slide(i), &nrec);
        if (g) {
            putnum(b, ((unsigned long long)i << 56)
                      | ((unsigned long long)(uintptr_t)g - (unsigned long long)(uintptr_t)mh));
            shim_log("sha-proto img<<56|off", b);
        }
    }
    if (!g) {
        putnum(b, ((unsigned long long)n << 32) | nrec);
        shim_log("sha proto not found nimg<<32|nrec", b);
    }
    done = 1;
    return g;
}

/* the canonical flattened existential; cloned once per generalization argument */
static const void *sha_canon(void)
{
    static const void *c;
    static int done;
    const void *pd;
    GetExistMD ge;
    uintptr_t ref;
    char b[64];
    if (done) return c;
    pd = sha_proto();
    if (!pd) return 0;
    ge = (GetExistMD)dlsym(swiftcore(), "swift_getExistentialTypeMetadata");
    if (!ge) { shim_log("sha", "no getexist"); done = 1; return 0; }
    ref = (uintptr_t)pd;
    c = ge(0, 0, 1, &ref);
    if (c && *(const unsigned long *)c != 0x303) c = 0;
    if (c) {
        uint32_t fl, np;
        __builtin_memcpy(&fl, (const char *)c + 8, 4);
        __builtin_memcpy(&np, (const char *)c + 12, 4);
        putnum(b, ((unsigned long long)fl << 32) | np);
        shim_log("sha-canon flags<<32|np", b);
    } else {
        shim_log("sha", "canon failed");
    }
    done = 1;
    return c;
}

#define SHA_MAX 64

static struct { const void *arg; const void *md; } g_shatab[SHA_MAX];
static volatile unsigned g_shaclaim;

static void *sha_clone(void)
{
    const void *c = sha_canon();
    uint32_t fl, np;
    unsigned long sz;
    char *blk;
    if (!c) return 0;
    __builtin_memcpy(&fl, (const char *)c + 8, 4);
    __builtin_memcpy(&np, (const char *)c + 12, 4);
    if (np > 8) return 0;
    sz = 16 + (unsigned long)np * 8 + ((fl & 0x40000000u) ? 8 : 0);
    blk = (char *)malloc(sz + 8);
    if (!blk) return 0;
    __builtin_memcpy(blk, (const char *)c - 8, sz + 8);
    return blk + 8;
}

/* no ldapr or lse on the a10, so plain accesses with a dmb and a claimed slot */
static const void *sha_get(const void *arg0)
{
    unsigned i, n;
    const void *md;
    for (;;) {
        n = g_shaclaim;
        if (n > SHA_MAX) n = SHA_MAX;
        __sync_synchronize();
        for (i = 0; i < n; i++)
            if (g_shatab[i].md && g_shatab[i].arg == arg0) return g_shatab[i].md;
        if (n >= SHA_MAX) return 0;
        md = sha_clone();
        if (!md) return 0;
        if (__sync_bool_compare_and_swap(&g_shaclaim, n, n + 1)) {
            g_shatab[n].arg = arg0;
            __sync_synchronize();
            g_shatab[n].md = md;
            return md;
        }
    }
}

static int sha_find(const void *md, const void **arg)
{
    unsigned i, n = g_shaclaim;
    if (n > SHA_MAX) n = SHA_MAX;
    for (i = 0; i < n; i++)
        if (g_shatab[i].md == md) { *arg = g_shatab[i].arg; return 1; }
    return 0;
}

/* 1 = source State matches the recorded argument, 0 = it does not, -1 = unknown */
static int sha_check(void *src, const void *srcType, const void *want, const void **gotout)
{
    static GetAssocType ga;
    static int gadone;
    const void *pd = sha_proto();
    const void *ctype = 0;
    const void *const *wt = 0;
    const char *reqs;
    unsigned long sk;
    uint32_t nsig, rflags;
    MDResp r;

    *gotout = 0;
    if (!pd || !src || ((uintptr_t)src & 7) || !srcType || ((uintptr_t)srcType & 7)) return -1;
    sk = *(const unsigned long *)srcType;
    if (sk == 0x303) {
        uint32_t fl, np;
        __builtin_memcpy(&fl, (const char *)srcType + 8, 4);
        __builtin_memcpy(&np, (const char *)srcType + 12, 4);
        if (np != 1 || (fl & 0x3f000000u) || (fl & 0x40000000u)) return -1;
        if (fl & 0x80000000u) {
            ctype = ((const void *const *)src)[3];
            wt = (const void *const *)((const void *const *)src)[4];
        } else {
            static GetObjType got;
            const void *obj = *(const void *const *)src;
            if (!obj || ((uintptr_t)obj & 7)) return -1;
            if (!got) got = (GetObjType)dlsym(swiftcore(), "swift_getObjectType");
            if (!got) return -1;
            ctype = got(obj);
            wt = (const void *const *)((const void *const *)src)[1];
        }
    } else {
        static ConformsTo ct;
        static int ctdone;
        if (!ctdone) {
            ctdone = 1;
            ct = (ConformsTo)dlsym(swiftcore(), "swift_conformsToProtocol");
        }
        if (!ct) return -1;
        ctype = srcType;
        wt = (const void *const *)ct(srcType, pd);
    }
    if (!ctype || !wt || ((uintptr_t)ctype & 7) || ((uintptr_t)wt & 7)) return -1;
    __builtin_memcpy(&nsig, (const char *)pd + 12, 4);
    if (nsig > 64) return -1;
    reqs = (const char *)pd + 24 + 12 * nsig;
    __builtin_memcpy(&rflags, reqs, 4);
    if ((rflags & 0xf) != 7) return -1;
    if (!gadone) {
        gadone = 1;
        ga = (GetAssocType)dlsym(swiftcore(), "swift_getAssociatedTypeWitness");
    }
    if (!ga) return -1;
    r = ga(0, wt, ctype, (const void *)(reqs - 8), (const void *)reqs);
    *gotout = r.value;
    if (!r.value) return -1;
    return r.value == want ? 1 : 0;
}

MetaResponse acc_sha(uint64_t request, const void *arg0) __asm__("_$s1S1AVMa");
MetaResponse acc_sha(uint64_t request, const void *arg0)
{
    static unsigned n;
    MetaResponse r;
    (void)request;
    r.metadata = (void *)sha_get(arg0);
    r.state = 0;
    if (!r.metadata) r.metadata = (void *)sha_canon();
    if (n < 24) {
        char b[220];
        unsigned j = 0;
        n++;
        j = acat(b, j, "arg=");
        j = atypename(b, j, 120, arg0);
        j = acat(b, j, " md=");
        j = anum(b, j, (unsigned long long)(uintptr_t)r.metadata);
        b[j] = 0;
        shim_log(r.metadata ? "sha-acc" : "sha-acc-null", b);
    }
    return r;
}
