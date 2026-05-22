package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

# Shim providing __isoc23_strtol / strtoll / strtoul / sscanf.
#
# libevent objects are compiled by the host gcc on Ubuntu 24.04 (glibc 2.39)
# with _GNU_SOURCE set, so glibc's <stdlib.h> asm-renames their strtol-family
# calls to __isoc23_* (for ISO C23 binary-literal parsing).  We link the
# resulting libevent.a into a binary that uses a hermetic glibc 2.27 sysroot,
# which has no __isoc23_* symbols.
#
# This library is compiled by the hermetic toolchain (sysroot glibc 2.27 — no
# asm-renames in those headers), so its inner strtol() calls resolve directly
# to the plain libc entry points instead of recursing.
cc_library(
    name = "glibc_compat",
    srcs = ["glibc_compat.c"],
    linkstatic = 1,
    alwayslink = 1,
)
