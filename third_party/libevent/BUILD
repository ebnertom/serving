# libevent (libevent.org) library.
# from https://github.com/libevent/libevent

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])  # BSD

exports_files(["LICENSE"])

include_files = [
    "libevent/include/evdns.h",
    "libevent/include/event.h",
    "libevent/include/evhttp.h",
    "libevent/include/evrpc.h",
    "libevent/include/evutil.h",
    "libevent/include/event2/buffer.h",
    "libevent/include/event2/bufferevent_struct.h",
    "libevent/include/event2/event.h",
    "libevent/include/event2/http_struct.h",
    "libevent/include/event2/rpc_struct.h",
    "libevent/include/event2/buffer_compat.h",
    "libevent/include/event2/dns.h",
    "libevent/include/event2/event_compat.h",
    "libevent/include/event2/keyvalq_struct.h",
    "libevent/include/event2/tag.h",
    "libevent/include/event2/bufferevent.h",
    "libevent/include/event2/dns_compat.h",
    "libevent/include/event2/event_struct.h",
    "libevent/include/event2/listener.h",
    "libevent/include/event2/tag_compat.h",
    "libevent/include/event2/bufferevent_compat.h",
    "libevent/include/event2/dns_struct.h",
    "libevent/include/event2/http.h",
    "libevent/include/event2/rpc.h",
    "libevent/include/event2/thread.h",
    "libevent/include/event2/event-config.h",
    "libevent/include/event2/http_compat.h",
    "libevent/include/event2/rpc_compat.h",
    "libevent/include/event2/util.h",
    "libevent/include/event2/visibility.h",
]

lib_files = [
    "libevent/lib/libevent.a",
    "libevent/lib/libevent_core.a",
    "libevent/lib/libevent_extra.a",
    "libevent/lib/libevent_pthreads.a",
]

genrule(
    name = "libevent-srcs",
    outs = include_files + lib_files,
    cmd = "\n".join([
        "export INSTALL_DIR=$$(pwd)/$(@D)/libevent",
        "export TMP_DIR=$$(mktemp -d -t libevent.XXXXXX)",
        "mkdir -p $$TMP_DIR",
        "cp -R $$(pwd)/external/com_github_libevent_libevent/* $$TMP_DIR",
        "cd $$TMP_DIR",
        "./autogen.sh",
        # We compile on Ubuntu 24.04 (glibc 2.39 headers) but link against a
        # hermetic glibc 2.27 sysroot, so any symbol newer than 2.27 fails to link.
        # The ac_cv_func_*=no cache vars force libevent to use its own
        # implementations of arc4random / arc4random_buf / strlcpy instead of
        # binding to glibc's (and they pair with the ARC4RANDOM_EXPORT patch
        # applied in workspace.bzl, which makes arc4random_buf extern so it
        # doesn't conflict with the prototype that glibc 2.36+ still emits in
        # <stdlib.h> under _GNU_SOURCE).
        "./configure --prefix=$$INSTALL_DIR CFLAGS='-fPIC -std=gnu99' CXXFLAGS=-fPIC --enable-shared=no --disable-openssl ac_cv_func_arc4random=no ac_cv_func_arc4random_buf=no ac_cv_func_strlcpy=no",
        "make install",
        "rm -rf $$TMP_DIR",
    ]),
)

cc_library(
    name = "libevent",
    srcs = [
        "libevent/lib/libevent.a",
        "libevent/lib/libevent_pthreads.a",
    ],
    hdrs = include_files,
    includes = ["libevent/include"],
    linkopts = ["-lpthread"],
    linkstatic = 1,
    # libevent objects (built on Ubuntu 24.04 host with _GNU_SOURCE) reference
    # __isoc23_strtol / strtoll / strtoul / sscanf, which the hermetic glibc
    # 2.27 sysroot doesn't provide.  See third_party/glibc_compat for the shim.
    deps = ["@//third_party/glibc_compat:glibc_compat"],
)

filegroup(
    name = "libevent-files",
    srcs = include_files + lib_files,
)
