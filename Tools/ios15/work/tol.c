#define _XOPEN_SOURCE 600
#define _DARWIN_C_SOURCE
#include <signal.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <ucontext.h>
#include <mach-o/dyld.h>
#include <mach/mach.h>
#include <dlfcn.h>
#include <mach-o/getsect.h>
#include <objc/runtime.h>
#include <objc/message.h>

static int logfd = -1;
static int nfaults = 0;

static void logfault(const char *kind, uint64_t pc, uint64_t addr, uint64_t lr)
{
    if (logfd < 0) {
        const char *t = getenv("TMPDIR");
        char p[512];
        snprintf(p, sizeof p, "%s/nullfaults.log", t ? t : "/tmp");
        logfd = open(p, O_WRONLY | O_CREAT | O_APPEND, 0644);
    }
    if (logfd < 0) return;
    char b[640];
    Dl_info di, dl;
    const char *pn = "?", *ln = "?";
    uint64_t po = pc, lo = lr;
    if (dladdr((void *)pc, &di)) { pn = strrchr(di.dli_fname, '/') ? strrchr(di.dli_fname, '/') + 1 : di.dli_fname; po = pc - (uint64_t)di.dli_fbase; if (di.dli_sname) { } }
    if (dladdr((void *)lr, &dl)) { ln = strrchr(dl.dli_fname, '/') ? strrchr(dl.dli_fname, '/') + 1 : dl.dli_fname; lo = lr - (uint64_t)dl.dli_fbase; }
    int n = snprintf(b, sizeof b, "%s %s+%llx (%s) <- %s+%llx addr=%llx n=%d\n", kind, pn, (unsigned long long)po,
                     (pc > 0x10000 && dladdr((void *)pc, &di) && di.dli_sname) ? di.dli_sname : "-", ln, (unsigned long long)lo,
                     (unsigned long long)addr, nfaults);
    write(logfd, b, n);
}

static void logregs(_STRUCT_ARM_THREAD_STATE64 *ss)
{
    if (logfd < 0) return;
    char b[700];
    int n = snprintf(b, sizeof b,
        "regs x0=%llx x1=%llx x2=%llx x19=%llx x20=%llx x21=%llx x22=%llx x23=%llx x24=%llx x25=%llx x26=%llx x27=%llx x28=%llx x29=%llx lr=%llx sp=%llx\n",
        (unsigned long long)ss->__x[0], (unsigned long long)ss->__x[1], (unsigned long long)ss->__x[2],
        (unsigned long long)ss->__x[19], (unsigned long long)ss->__x[20], (unsigned long long)ss->__x[21],
        (unsigned long long)ss->__x[22], (unsigned long long)ss->__x[23], (unsigned long long)ss->__x[24],
        (unsigned long long)ss->__x[25], (unsigned long long)ss->__x[26], (unsigned long long)ss->__x[27],
        (unsigned long long)ss->__x[28], (unsigned long long)ss->__fp,
        (unsigned long long)ss->__lr, (unsigned long long)ss->__sp);
    write(logfd, b, n);
}

static int nullfd = -1;

/* copies in-kernel, so unlike a probe-then-load there is no window for another
   thread to unmap the page underneath us */
static int sread(void *dst, uint64_t p, unsigned long n)
{
    vm_size_t got = 0;
    if (p < 0x1000 || !n) return 0;
    if (vm_read_overwrite(mach_task_self(), (vm_address_t)p, (vm_size_t)n,
                          (vm_address_t)dst, &got) != KERN_SUCCESS)
        return 0;
    return got == (vm_size_t)n;
}

static uint64_t sword(uint64_t p, int *ok)
{
    uint64_t w = 0;
    int r = sread(&w, p, 8);
    if (ok) *ok = r;
    return r ? w : 0;
}

static int readable(uint64_t p, unsigned long n)
{
    char b[256];
    if (p < 0x1000 || (p & 7)) return 0;
    while (n) {
        unsigned long c = n > sizeof b ? sizeof b : n;
        if (!sread(b, p, c)) return 0;
        p += c;
        n -= c;
    }
    return 1;
}

int sh_peek(void *dst, const void *src, unsigned long n)
{
    return sread(dst, (uint64_t)src, n);
}

/* nul-terminated copy, one page-safe chunk at a time */
static int sreadstr(char *dst, unsigned long cap, uint64_t p)
{
    unsigned long i = 0;
    if (!cap) return 0;
    dst[0] = 0;
    while (i + 1 < cap) {
        char c;
        if (!sread(&c, p + i, 1)) break;
        dst[i++] = c;
        if (!c) return 1;
    }
    dst[i] = 0;
    return i > 0;
}

static void logmd(const char *tag, uint64_t p)
{
    char hdr[sizeof(void *) + 16];
    uint64_t vwtp;
    unsigned long long vw[10];
    int havevwt;
    if (logfd < 0) return;
    if (!sread(hdr, p - sizeof(void *), sizeof hdr)) return;
    unsigned long kind;
    memcpy(&kind, hdr + sizeof(void *), sizeof kind);
    if (kind != 0x303 && kind != 0x307 && kind != 0x200 && kind != 0x201
            && !(kind > 0x100000000ull && !(kind & 7) && readable(kind, 8)))
        return;
    unsigned int flags = 0, nproto = 0;
    memcpy(&flags, hdr + sizeof(void *) + sizeof(unsigned long), 4);
    memcpy(&nproto, hdr + sizeof(void *) + sizeof(unsigned long) + 4, 4);
    memcpy(&vwtp, hdr, 8);
    havevwt = vwtp && sread(vw, vwtp, sizeof vw);
    char b[400];
    int n = snprintf(b, sizeof b, "%s md=%llx kind=%lx flags=%x nproto=%u size=%llx stride=%llx wt=%u\n",
        tag, (unsigned long long)p, kind, flags, nproto,
        havevwt ? vw[8] : 0ull, havevwt ? vw[9] : 0ull,
        (kind == 0x303) ? (flags & 0x00ffffffu) : 0u);
    write(logfd, b, n);
    if (kind == 0x200 || kind == 0x201 || kind > 0x800) {
        uint64_t desc = 0, nameoff = 0;
        int32_t rel = 0;
        int ok = 0;
        desc = sword(p + (kind > 0x800 ? 0x40 : 0x08), &ok);
        if (ok && sread(&rel, desc + 8, 4)) {
            char nm[96];
            nameoff = desc + 8 + (int64_t)rel;
            if (sreadstr(nm, sizeof nm, nameoff)) {
                int m = snprintf(b, sizeof b, "%s ntdname=%.80s\n", tag, nm);
                write(logfd, b, m);
            }
        }
    }
}

static void loghex(const char *tag, uint64_t p, unsigned n)
{
    char copy[320];
    if (n > sizeof copy) n = sizeof copy;
    if (logfd < 0 || (p & 7) || !sread(copy, p, n)) return;
    char b[600];
    int j = snprintf(b, sizeof b, "%s @%llx", tag, (unsigned long long)p);
    for (unsigned i = 0; i < n / 8 && j < (int)sizeof b - 24; i++) {
        unsigned long long w;
        memcpy(&w, copy + i * 8, 8);
        j += snprintf(b + j, sizeof b - j, " %llx", w);
    }
    j += snprintf(b + j, sizeof b - j, "\n");
    write(logfd, b, j);
}

static void loggenerics(uint64_t obj)
{
    uint64_t md;
    int ok = 0;
    md = sword(obj, &ok);
    if (!ok) return;
    logmd("md-self", md);
    loghex("gen-self", md + 0x40, 0x48);
    for (unsigned off = 0x40; off <= 0x80; off += 8) {
        uint64_t slot = sword(md + off, &ok);
        if (!ok) continue;
        if (slot < 0x100000000ull || (slot & 7)) continue;
        char tag[32];
        snprintf(tag, sizeof tag, "md-gen+%x", off);
        logmd(tag, slot);
    }
}

static void logbt(const char *why, int code);
static void logbt_fp(const char *why, int code, uint64_t fp, uint64_t pc, uint64_t lr);

static void logstkmd(uint64_t sp, unsigned words)
{
    for (unsigned i = 0; i < words; i++) {
        int ok = 0;
        uint64_t w = sword(sp + i * 8, &ok);
        if (!ok) continue;
        if (w < 0x100000000ull || (w & 7)) continue;
        char tag[32];
        snprintf(tag, sizeof tag, "stkmd+%x", i * 8);
        logmd(tag, w);
    }
}

static void logobjs(uint64_t sp, unsigned words)
{
    for (unsigned i = 0; i < words; i++) {
        uint64_t w, isa;
        int ok = 0;
        w = sword(sp + i * 8, &ok);
        if (!ok) continue;
        if (w < 0x280000000ull || w >= 0x290000000ull || (w & 7)) continue;
        isa = sword(w, &ok);
        if (!ok) continue;
        if (isa < 0x100000000ull || (isa & 7) || !readable(isa, 8)) continue;
        const char *nm = class_getName((Class)isa);
        char nmc[128];
        if (!nm || !sreadstr(nmc, sizeof nmc, (uint64_t)nm)) continue;
        char b[200];
        int n = snprintf(b, sizeof b, "obj+%x %llx isa=%llx %.120s\n",
                         i * 8, (unsigned long long)w, (unsigned long long)isa, nmc);
        write(logfd, b, n);
    }
}

static void logobj(const char *tag, uint64_t w)
{
    uint64_t isa, bits;
    const char *nm;
    char b[220], nmc[160];
    int n, ok = 0;
    if (logfd < 0) return;
    if (w < 0x100000000ull || (w & 7)) return;
    isa = sword(w, &ok);
    if (!ok) return;
    if (isa < 0x100000000ull || (isa & 7) || !readable(isa, 0x28)) return;
    bits = sword(isa + 0x20, &ok);
    if (!ok || !bits || !readable(bits & 0x00007ffffffffff8ull, 8)) return;
    nm = class_getName((Class)isa);
    if (!nm || !sreadstr(nmc, sizeof nmc, (uint64_t)nm)) return;
    n = snprintf(b, sizeof b, "%s %llx isa=%llx %.140s\n",
                 tag, (unsigned long long)w, (unsigned long long)isa, nmc);
    write(logfd, b, n);
}

static void logderef(const char *tag, uint64_t w, unsigned n)
{
    uint64_t t;
    int ok = 0;
    if (w < 0x100000000ull || (w & 7)) return;
    t = sword(w, &ok);
    if (!ok) return;
    loghex(tag, t, n);
}

static void logslot(const char *tag, uint64_t addr, unsigned n)
{
    int ok = 0;
    uint64_t v = sword(addr, &ok);
    if (!ok) return;
    logderef(tag, v, n);
}

static void logaddr(const char *tag, uint64_t ptr)
{
    Dl_info di;
    char b[300];
    int n;
    const char *nm;
    if (logfd < 0 || !ptr) return;
    if (!dladdr((void *)ptr, &di) || !di.dli_fname) {
        n = snprintf(b, sizeof b, "%s %llx <no image>\n", tag, (unsigned long long)ptr);
        write(logfd, b, n);
        return;
    }
    nm = strrchr(di.dli_fname, '/') ? strrchr(di.dli_fname, '/') + 1 : di.dli_fname;
    n = snprintf(b, sizeof b, "%s %llx = %s+%llx %s\n", tag, (unsigned long long)ptr, nm,
                 (unsigned long long)(ptr - (uint64_t)di.dli_fbase),
                 di.dli_sname ? di.dli_sname : "-");
    write(logfd, b, n);
}

static void logslotaddr(const char *tag, uint64_t addr)
{
    int ok = 0;
    uint64_t v = sword(addr, &ok);
    if (!ok) return;
    logaddr(tag, v);
}

static volatile int in_handler;
static volatile int nreenter;

/* step over a faulting load, zeroing its destination */
static int fixup_load(_STRUCT_ARM_THREAD_STATE64 *ss, uint64_t pc)
{
    uint32_t ins;
    if (!sread(&ins, pc, 4)) return 0;
    uint32_t top = (ins >> 27) & 7;
    int v = (ins >> 26) & 1;
    int rn = (ins >> 5) & 31;
    if (top == 7 && ((ins >> 24) & 3) == 0 && ((ins >> 21) & 1) == 0 && ((ins >> 10) & 1) && rn != 31) {
        int64_t imm = (int64_t)(((int32_t)(ins << 11)) >> 23);
        ss->__x[rn] += imm;
    }
    if (top == 5 && ((ins >> 23) & 1) && rn != 31) {
        int scale = ((ins >> 30) & 3) == 2 ? 3 : 2;
        int64_t imm = (int64_t)(((int32_t)(ins << 11)) >> 25) << scale;
        ss->__x[rn] += imm;
    }
    if (top == 7 && !v && ((ins >> 22) & 3) != 0 && ((ins >> 24) & 3) != 2) {
        int rt = ins & 31;
        if (rt != 31) ss->__x[rt] = 0;
    } else if (top == 5 && !v && ((ins >> 22) & 1)) {
        int rt = ins & 31, rt2 = (ins >> 10) & 31;
        if (rt != 31) ss->__x[rt] = 0;
        if (rt2 != 31) ss->__x[rt2] = 0;
    } else if (top == 3 && !v && ((ins >> 22) & 1)) {
        int rt = ins & 31;
        if (rt != 31) ss->__x[rt] = 0;
    }
    ss->__pc = pc + 4;
    return 1;
}

static void handler(int sig, siginfo_t *si, void *uctx)
{
    ucontext_t *u = (ucontext_t *)uctx;
    _STRUCT_ARM_THREAD_STATE64 *ss = &u->uc_mcontext->__ss;
    uint64_t pc = ss->__pc;
    uint64_t lr = ss->__lr;
    uint64_t addr = (uint64_t)si->si_addr;
    int64_t sa = (int64_t)addr;
    if (in_handler) {
        int n = nreenter++;
        if (logfd >= 0 && n < 40) {
            char rb[160];
            int rn = snprintf(rb, sizeof rb, "REENTER sig=%d pc=%llx addr=%llx n=%d\n",
                              sig, (unsigned long long)pc, (unsigned long long)addr, n);
            write(logfd, rb, rn);
        }
        /* recover rather than die, so the report already in progress finishes */
        if ((sig == SIGSEGV || sig == SIGBUS) && n < 2000 && pc >= 0x10000
                && fixup_load(ss, pc))
            return;
        _exit(79);
    }
    if (sig != SIGSEGV && sig != SIGBUS) {
        logfault("hard", pc, addr, lr);
        in_handler = 1;
        logregs(ss);
        logbt_fp("FATALBT", sig, ss->__fp, pc, lr);
        logmd("md-x1", ss->__x[1]);
        logmd("md-x23", ss->__x[23]);
        loghex("buf-x24", ss->__x[24], 64);
        loghex("buf-x0", ss->__x[0], 64);
        loghex("stk-sp", ss->__sp, 256);
        logslot("mdself-fp", ss->__fp - 0x10, 0x80);
        logslot("mdself-sp", ss->__sp + 0x60, 0x80);
        loghex("wt-x26", ss->__x[26], 0x40);
        loghex("wt-x27", ss->__x[27], 0x50);
        loghex("wt-x22", ss->__x[22], 0x40);
        logslotaddr("producer-x26+10", ss->__x[26] + 0x10);
        logslotaddr("wt0-x26", ss->__x[26]);
        logslotaddr("destroy-x27+8", ss->__x[27] + 8);
        logaddr("lr", ss->__lr);
        logaddr("x28", ss->__x[28]);
        logaddr("x21", ss->__x[21]);
        logderef("cls-x19", ss->__x[19], 0x50);
        logderef("cls-x20", ss->__x[20], 0x50);
        logderef("cls-x22", ss->__x[22], 0x50);
        logderef("cls-x26", ss->__x[26], 0x50);
        logbt("FATAL", sig);
        _exit(77);
    }
    if (sa >= 0x10000 || sa <= -0x10000 || ++nfaults > 200000) {
        logfault("far", pc, addr, lr);
        in_handler = 1;
        logregs(ss);
        logbt_fp("FATALBT", sig, ss->__fp, pc, lr);
        logmd("md-x1", ss->__x[1]);
        logmd("md-x23", ss->__x[23]);
        loghex("buf-x24", ss->__x[24], 64);
        loghex("buf-x0", ss->__x[0], 64);
        loghex("stk-sp", ss->__sp, 256);
        logslot("mdself-fp", ss->__fp - 0x10, 0x80);
        logslot("mdself-sp", ss->__sp + 0x60, 0x80);
        loghex("wt-x26", ss->__x[26], 0x40);
        loghex("wt-x27", ss->__x[27], 0x50);
        loghex("wt-x22", ss->__x[22], 0x40);
        logslotaddr("producer-x26+10", ss->__x[26] + 0x10);
        logslotaddr("wt0-x26", ss->__x[26]);
        logslotaddr("destroy-x27+8", ss->__x[27] + 8);
        logaddr("lr", ss->__lr);
        logaddr("x28", ss->__x[28]);
        logaddr("x21", ss->__x[21]);
        logderef("cls-x19", ss->__x[19], 0x50);
        logderef("cls-x20", ss->__x[20], 0x50);
        logderef("cls-x22", ss->__x[22], 0x50);
        logderef("cls-x26", ss->__x[26], 0x50);
        logbt("FATAL", sig);
        _exit(78);
    }
    if (pc < 0x10000) {
        logfault("call0", pc, addr, lr);
        ss->__x[0] = 0;
        ss->__pc = lr;
        return;
    }
    logfault("load", pc, addr, lr);
    if (!fixup_load(ss, pc)) ss->__pc = pc + 4;
}

static void logslide(void)
{
    Dl_info di;
    if (logfd < 0) {
        logfault("slidewarm", 0, 0, 0);
        if (logfd < 0) return;
    }
    char b[200];
    unsigned long long slide = (unsigned long long)_dyld_get_image_vmaddr_slide(0);
    unsigned long long shimbase = 0;
    if (dladdr((void *)(uintptr_t)&logslide, &di)) shimbase = (unsigned long long)di.dli_fbase;
    int n = snprintf(b, sizeof b, "slide main=%llx shim=%llx\n", slide, shimbase);
    write(logfd, b, n);
}

static void logmsg(const char *kind, const char *a, const char *b);

__attribute__((constructor)) static void tol_init(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO | SA_NODEFER;
    sigaction(SIGSEGV, &sa, 0);
    sigaction(SIGBUS, &sa, 0);
    sigaction(SIGILL, &sa, 0);
    sigaction(SIGTRAP, &sa, 0);
    sigaction(SIGFPE, &sa, 0);
    sigaction(SIGABRT, &sa, 0);
    logslide();
    /* smoke test for the abort path; marker file only, never set under springboard */
    if (access("/var/root/.shim_abort_test", F_OK) == 0) {
        logmsg("aborttest", "raising", "");
        raise(SIGABRT);
    }
}


static void logmsg(const char *kind, const char *a, const char *b)
{
    if (logfd < 0) {
        const char *t = getenv("TMPDIR");
        char p[512];
        snprintf(p, sizeof p, "%s/nullfaults.log", t ? t : "/tmp");
        logfd = open(p, O_WRONLY | O_CREAT | O_APPEND, 0644);
    }
    if (logfd < 0) return;
    char buf[400];
    int n = snprintf(buf, sizeof buf, "%s %s %s\n", kind, a ? a : "", b ? b : "");
    write(logfd, buf, n);
}

static IMP orig_msig_i, orig_msig_c;

static void compat_fwd(id self, SEL _cmd, id inv);

static id compat_msig(id self, SEL _cmd, SEL sel)
{
    int isc = class_isMetaClass(object_getClass(self));
    id r = ((id (*)(id, SEL, SEL))(isc ? orig_msig_c : orig_msig_i))(self, _cmd, sel);
    if (r) return r;
    const char *sn = sel_getName(sel);
    char types[80];
    int n = 0, i;
    Class rc = object_getClass(self);
    IMP ownfw = rc ? class_getMethodImplementation(rc, sel_registerName("forwardInvocation:")) : 0;
    if (ownfw && ownfw != (IMP)compat_fwd) {
        logmsg("unrec-own", class_getName(isc ? (Class)self : rc), sn);
        return 0;
    }
    logmsg("unrec", class_getName(isc ? (Class)self : object_getClass(self)), sn);
    for (i = 0; sn[i]; i++) if (sn[i] == ':') n++;
    if (n > 60) n = 60;
    types[0] = '@'; types[1] = '@'; types[2] = ':';
    for (i = 0; i < n; i++) types[3 + i] = '@';
    types[3 + n] = 0;
    return ((id (*)(id, SEL, const char *))objc_msgSend)((id)objc_getClass("NSMethodSignature"), sel_registerName("signatureWithObjCTypes:"), types);
}

static void compat_fwd(id self, SEL _cmd, id inv)
{
}

static id cl_group_repeating(id self, SEL _cmd, id size, id subitem, long count)
{
    const char *n = sel_getName(_cmd);
    SEL real = (n[0] == 'h') ? sel_registerName("horizontalGroupWithLayoutSize:subitem:count:")
                             : sel_registerName("verticalGroupWithLayoutSize:subitem:count:");
    return ((id (*)(id, SEL, id, id, long))objc_msgSend)(self, real, size, subitem, count);
}

static void compat_nop_v(id self, SEL _cmd)
{
}

static void compat_nop_i(id self, SEL _cmd, long v)
{
}

static long compat_zero_q(id self, SEL _cmd)
{
    return 0;
}

static id compat_nil(id self, SEL _cmd)
{
    return 0;
}

/* ios 16 exposed Detent.identifier. ios 15's UIKitCore already holds the value
   in an _identifier ivar and carries all three identifier strings
   (com.apple.UIKit.medium / .large / .unspecified) in its __cstring, so this
   returns the real one rather than the nil the forwarder was handing back. */
static id detent_identifier(id self, SEL _cmd)
{
    static int logged;
    SEL under = sel_registerName("_identifier");
    Class c = object_getClass(self);
    Ivar iv;
    id r = 0;
    const char *how = "none";
    if (c && class_respondsToSelector(c, under)) {
        r = ((id (*)(id, SEL))objc_msgSend)(self, under);
        how = "sel";
    } else if (c && (iv = class_getInstanceVariable(c, "_identifier")) != 0) {
        r = object_getIvar(self, iv);
        how = "ivar";
    }
    /* nil here is the same silent wrong answer the forwarder gave, so say
       which branch won and what came back. */
    if (!logged) {
        logged = 1;
        logmsg("detent-id", how, r ? ((const char *(*)(id, SEL))objc_msgSend)(
                   r, sel_registerName("UTF8String")) : "nil");
    }
    return r;
}

static void un_set_badge(id self, SEL _cmd, long count, id completion)
{
    Class ua = objc_getClass("UIApplication");
    if (ua) {
        id app = ((id (*)(id, SEL))objc_msgSend)((id)ua, sel_registerName("sharedApplication"));
        if (app)
            ((void (*)(id, SEL, long))objc_msgSend)(app, sel_registerName("setApplicationIconBadgeNumber:"), count);
    }
    if (completion) {
        void *blk = (void *)completion;
        void (*inv)(void *, id) = *(void (**)(void *, id))((char *)blk + 16);
        if (inv) inv(blk, 0);
    }
}

static void add_compat_methods(void)
{
    Class g = objc_getClass("NSCollectionLayoutGroup");
    if (g) {
        Class mg = object_getClass((id)g);
        class_addMethod(mg, sel_registerName("horizontalGroupWithLayoutSize:repeatingSubitem:count:"),
                        (IMP)cl_group_repeating, "@@:@@q");
        class_addMethod(mg, sel_registerName("verticalGroupWithLayoutSize:repeatingSubitem:count:"),
                        (IMP)cl_group_repeating, "@@:@@q");
    }
    Class cv = objc_getClass("UICollectionView");
    if (cv) {
        class_addMethod(cv, sel_registerName("setSelfSizingInvalidation:"), (IMP)compat_nop_i, "v@:q");
        class_addMethod(cv, sel_registerName("selfSizingInvalidation"), (IMP)compat_zero_q, "q@:");
    }
    Class vc = objc_getClass("UIViewController");
    if (vc) {
        class_addMethod(vc, sel_registerName("setNeedsUpdateOfSupportedInterfaceOrientations"),
                        (IMP)compat_nop_v, "v@:");
        class_addMethod(vc, sel_registerName("activePresentationController"), (IMP)compat_nil, "@@:");
    }
    Class ls = objc_getClass("NSCollectionLayoutSection");
    if (ls) class_addMethod(ls, sel_registerName("setSupplementaryContentInsetsReference:"),
                            (IMP)compat_nop_i, "v@:q");
    Class sp = objc_getClass("UISheetPresentationController");
    if (sp) class_addMethod(sp, sel_registerName("invalidateDetents"), (IMP)compat_nop_v, "v@:");
    Class dt = objc_getClass("UISheetPresentationControllerDetent");
    if (dt) class_addMethod(dt, sel_registerName("identifier"), (IMP)detent_identifier, "@@:");
    /* ios 15 behaves as .disabled, which is the old invalidation behaviour.
       already done for UICollectionView above. */
    Class tv = objc_getClass("UITableView");
    if (tv) {
        class_addMethod(tv, sel_registerName("setSelfSizingInvalidation:"), (IMP)compat_nop_i, "v@:q");
        class_addMethod(tv, sel_registerName("selfSizingInvalidation"), (IMP)compat_zero_q, "q@:");
    }
    Class un = objc_getClass("UNUserNotificationCenter");
    if (un) class_addMethod(un, sel_registerName("setBadgeCount:withCompletionHandler:"),
                            (IMP)un_set_badge, "v@:q@?");
}

__attribute__((constructor(101))) static void compat_init(void)
{
    Class nso = objc_getClass("NSObject");
    SEL ms = sel_registerName("methodSignatureForSelector:");
    SEL fw = sel_registerName("forwardInvocation:");
    Method m = class_getInstanceMethod(nso, ms);
    if (m) orig_msig_i = method_setImplementation(m, (IMP)compat_msig);
    Method mc = class_getClassMethod(nso, ms);
    if (mc) {
        orig_msig_c = method_getImplementation(mc);
        if (mc != m) method_setImplementation(mc, (IMP)compat_msig);
        else orig_msig_c = orig_msig_i;
    }
    Method f = class_getInstanceMethod(nso, fw);
    if (f) method_setImplementation(f, (IMP)compat_fwd);
    Method fc = class_getClassMethod(nso, fw);
    if (fc && fc != f) method_setImplementation(fc, (IMP)compat_fwd);
    add_compat_methods();
    {
        const char *t = getenv("TMPDIR");
        char path[512];
        int fd;
        snprintf(path, sizeof path, "%s/nullfaults.log", t ? t : "/tmp");
        fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd >= 0) {
            dup2(fd, 2);
            if (fd != 2) close(fd);
        }
    }
}

static int my_sigaction(int sig, const struct sigaction *a, struct sigaction *o)
{
    if (sig == SIGSEGV || sig == SIGBUS || sig == SIGILL || sig == SIGTRAP || sig == SIGFPE || sig == SIGABRT) {
        if (o) memset(o, 0, sizeof *o);
        return 0;
    }
    return sigaction(sig, a, o);
}

static kern_return_t my_tsep(task_t t, exception_mask_t m, mach_port_t p, exception_behavior_t b, thread_state_flavor_t f)
{
    return KERN_SUCCESS;
}

struct interpose { const void *r, *o; };
__attribute__((used, section("__DATA,__interpose"))) static struct interpose ip1 = {(const void *)my_sigaction, (const void *)sigaction};
__attribute__((used, section("__DATA,__interpose"))) static struct interpose ip2 = {(const void *)my_tsep, (const void *)task_set_exception_ports};

static const char *seen[400];
static int nseen = 0;

void shim_unimpl(const char *name)
{
    for (int i = 0; i < nseen; i++) if (seen[i] == name) return;
    if (nseen < 400) seen[nseen++] = name;
    logmsg("unimpl", name, "");
}

static void btframe(const char *what, uint64_t pc)
{
    char b[200];
    Dl_info di;
    const char *nm = "?";
    uint64_t off = pc;
    int n;
    if (logfd < 0 || !pc) return;
    if (dladdr((void *)pc, &di)) {
        nm = strrchr(di.dli_fname, '/') ? strrchr(di.dli_fname, '/') + 1 : di.dli_fname;
        off = pc - (uint64_t)di.dli_fbase;
    }
    n = snprintf(b, sizeof b, "   %s %s+%llx %s\n", what, nm, (unsigned long long)off,
                 (dladdr((void *)pc, &di) && di.dli_sname) ? di.dli_sname : "");
    write(logfd, b, n);
}

/* walk the interrupted thread's own frames, not the handler's */
static void logbt_fp(const char *why, int code, uint64_t fp, uint64_t pc, uint64_t lr)
{
    char b[160];
    int n = snprintf(b, sizeof b, "%s code=%d\n", why, code);
    if (logfd < 0) return;
    write(logfd, b, n);
    btframe("bt0", pc);
    btframe("bt1", lr);
    for (int i = 0; i < 28 && fp > 0x1000; i++) {
        uint64_t fr[2];
        if (!sread(fr, fp, 16)) break;
        if (!fr[1]) break;
        btframe("bt", fr[1]);
        if (fr[0] <= fp) break;
        fp = fr[0];
    }
}

static void logbt(const char *why, int code)
{
    char b[160];
    int n = snprintf(b, sizeof b, "%s code=%d\n", why, code);
    if (logfd >= 0) write(logfd, b, n);
    else { logmsg(why, "", ""); }
    uint64_t fp = (uint64_t)__builtin_frame_address(0);
    for (int i = 0; i < 24 && fp > 0x1000; i++) {
        uint64_t fr[2], nfp, lr;
        if (!sread(fr, fp, 16)) break;
        nfp = fr[0];
        lr = fr[1];
        if (!lr) break;
        Dl_info di;
        const char *nm = "?";
        uint64_t off = lr;
        if (dladdr((void *)lr, &di)) {
            nm = strrchr(di.dli_fname, '/') ? strrchr(di.dli_fname, '/') + 1 : di.dli_fname;
            off = lr - (uint64_t)di.dli_fbase;
        }
        n = snprintf(b, sizeof b, "   bt %s+%llx %s\n", nm, (unsigned long long)off,
                     (dladdr((void *)lr, &di) && di.dli_sname) ? di.dli_sname : "");
        if (logfd >= 0) write(logfd, b, n);
        if (nfp <= fp) break;
        fp = nfp;
    }
}

static int in_exit = 0;

static void my_exit(int code)
{
    if (!in_exit++) logbt("EXIT", code);
    _exit(code);
}

static void my_abort(void)
{
    if (!in_exit++) logbt("ABORT", 0);
    _exit(134);
}

static kern_return_t my_tswap(task_t t, exception_mask_t m, mach_port_t p,
                              exception_behavior_t b, thread_state_flavor_t f,
                              exception_mask_t *masks, mach_msg_type_number_t *nmasks,
                              mach_port_t *handlers, exception_behavior_t *behaviors,
                              thread_state_flavor_t *flavors)
{
    if (nmasks) *nmasks = 0;
    return KERN_SUCCESS;
}

__attribute__((used, section("__DATA,__interpose"))) static struct interpose ip3 = {(const void *)my_exit, (const void *)exit};
__attribute__((used, section("__DATA,__interpose"))) static struct interpose ip4 = {(const void *)my_abort, (const void *)abort};
__attribute__((used, section("__DATA,__interpose"))) static struct interpose ip5 = {(const void *)my_tswap, (const void *)task_swap_exception_ports};


static void my__exit(int code)
{
    if (!in_exit++) logbt("UEXIT", code);
    syscall(1, code);
}

static int my_kill(int pid, int sig)
{
    logbt("KILL", sig);
    return 0;
}

__attribute__((used, section("__DATA,__interpose"))) static struct interpose ip6 = {(const void *)my__exit, (const void *)_exit};
__attribute__((used, section("__DATA,__interpose"))) static struct interpose ip7 = {(const void *)my_kill, (const void *)kill};

extern void swift_assertfail(const char *, uint64_t, uint64_t, uint64_t, uint64_t,
                             const char *, uint64_t, uint64_t, uint64_t, uint32_t)
    __asm__("_$ss17_assertionFailure__4file4line5flagss5NeverOs12StaticStringV_SSAHSus6UInt32VtF");

static void logstr(const char *tag, const char *p, uint64_t n)
{
    char b[300];
    if (!p || n > 200) { logmsg(tag, "?", ""); return; }
    uint64_t i = 0;
    for (; i < n && i < sizeof b - 1; i++) b[i] = p[i];
    b[i] = 0;
    logmsg(tag, b, "");
}

static void my_assertfail(const char *pfx, uint64_t pfxn, uint64_t pfxf,
                          uint64_t mcf, uint64_t mobj,
                          const char *file, uint64_t filen, uint64_t filef,
                          uint64_t line, uint32_t flags)
{
    char num[64];
    logstr("ASSERT-pfx", pfx, pfxn);
    if (mobj & 0x8000000000000000ULL)
        logstr("ASSERT-msg", (const char *)(mobj & 0x0000ffffffffffffULL), mcf & 0xffffffffffffULL);
    else
        logstr("ASSERT-msg", (const char *)&mcf, 8);
    logstr("ASSERT-file", file, filen);
    snprintf(num, sizeof num, "%llu", (unsigned long long)line);
    logmsg("ASSERT-line", num, "");
    logbt("ASSERT", (int)line);
    _exit(79);
}

__attribute__((used, section("__DATA,__interpose"))) static struct interpose ip8 = {(const void *)my_assertfail, (const void *)swift_assertfail};

void shim_log(const char *a, const char *b)
{
    logmsg("sh", a, b);
}

extern void objc_exception_throw(void *);

static void my_objc_throw(void *exc);
__attribute__((used, section("__DATA,__interpose"))) static struct interpose ip9 = {(const void *)my_objc_throw, (const void *)objc_exception_throw};

static void my_objc_throw(void *exc)
{
    const char *cls = "?";
    const char *rs = "";
    if (exc) {
        cls = class_getName(object_getClass((id)exc));
        id r = ((id (*)(id, SEL))objc_msgSend)((id)exc, sel_registerName("reason"));
        if (r) rs = ((const char *(*)(id, SEL))objc_msgSend)(r, sel_registerName("UTF8String"));
    }
    logmsg("THROW", cls, rs ? rs : "");
    logbt("THROW", 0);
    ((void (*)(void *))ip9.o)(exc);
}

extern void __cxa_throw(void *, void *, void (*)(void *));

static void my_cxa_throw(void *exc, void *ti, void (*dest)(void *));
__attribute__((used, section("__DATA,__interpose"))) static struct interpose ip10 = {(const void *)my_cxa_throw, (const void *)__cxa_throw};

static int sane_obj(const void *p)
{
    uintptr_t v = (uintptr_t)p;
    if (!v || (v & 7)) return 0;
    if (v < 0x100000000ULL || v > 0x0000800000000000ULL) return 0;
    return 1;
}

static unsigned n_cxa;

static void my_cxa_throw(void *exc, void *ti, void (*dest)(void *))
{
    char tbuf[80];
    unsigned k = 0;
    if (sane_obj(ti) && readable((uint64_t)ti, 16)) {
        const char *cand = ((const char *const *)ti)[1];
        unsigned span = 0, try_;
        for (try_ = sizeof tbuf - 1; try_ >= 8; try_ >>= 1) {
            if (cand && readable((uint64_t)cand, try_)) { span = try_; break; }
        }
        for (; k < span; k++) {
            char c = cand[k];
            if (!c) break;
            tbuf[k] = (c >= 0x20 && c < 0x7f) ? c : '.';
        }
    }
    tbuf[k] = 0;
    if (n_cxa < 200) {
        n_cxa++;
        logmsg("CXATHROW", k ? tbuf : "<noti>", "");
        if (n_cxa <= 8) logbt("CXATHROW-bt", 0);
    }
    ((void (*)(void *, void *, void (*)(void *)))ip10.o)(exc, ti, dest);
}

void shim_logobj(const char *tag, void *obj)
{
    if (!sane_obj(obj)) { logmsg(tag, "<bad>", ""); return; }
    Class oc = object_getClass((id)obj);
    if (!sane_obj(oc)) { logmsg(tag, "<bad-isa>", ""); return; }
    const char *n = class_getName(oc);
    logmsg(tag, n ? n : "?", "");
}
