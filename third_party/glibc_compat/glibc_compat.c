/*
 * Glibc 2.38+ <stdlib.h>, when _GNU_SOURCE is defined, asm-renames calls to
 * strtol/strtoll/strtoul/sscanf into __isoc23_* variants (for ISO C23 binary
 * literal parsing).  libevent's evconfig-private.h defines _GNU_SOURCE
 * unconditionally, and its build runs under the host compiler (Ubuntu 24.04
 * with glibc 2.39 headers), so its objects carry references to __isoc23_*.
 *
 * The final binary, however, links against a hermetic glibc 2.27 sysroot
 * that has no __isoc23_* symbols — link fails with "undefined symbol".
 *
 * This file provides those symbols as forwards to the plain libc entry
 * points.  It is compiled by the same hermetic toolchain that does the
 * final link (sysroot glibc 2.27), whose <stdlib.h> emits no redirects, so
 * the inner strtol() etc. resolve directly.  We use bare extern declarations
 * here as belt-and-suspenders against any future toolchain header that
 * might re-add the redirect.
 */

#include <stdarg.h>
#include <stddef.h>

extern long strtol(const char *, char **, int);
extern long long strtoll(const char *, char **, int);
extern unsigned long strtoul(const char *, char **, int);
extern int vsscanf(const char *, const char *, va_list);

long __isoc23_strtol(const char *nptr, char **endptr, int base) {
    return strtol(nptr, endptr, base);
}

long long __isoc23_strtoll(const char *nptr, char **endptr, int base) {
    return strtoll(nptr, endptr, base);
}

unsigned long __isoc23_strtoul(const char *nptr, char **endptr, int base) {
    return strtoul(nptr, endptr, base);
}

int __isoc23_sscanf(const char *str, const char *format, ...) {
    va_list ap;
    int ret;
    va_start(ap, format);
    ret = vsscanf(str, format, ap);
    va_end(ap);
    return ret;
}
