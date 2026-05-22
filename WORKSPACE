workspace(name = "tf_serving")

# TODO(b/269515133): We temporarily remove remote_predict from our builds for
# 2.12 due to a breakage caused by
# github.com/tensorflow/tensorflow/commit/6147c03eb9af1e5d2ae155045b33e909ef96944e
# This will be removed in a subsequent release.
local_repository(
    name = "ignore_remote_predict",
    path = "tensorflow_serving/experimental/tensorflow/ops/remote_predict/",
)

# ===== TensorFlow dependency =====
#
# TensorFlow is imported here instead of in tf_serving_workspace() because
# existing automation scripts that bump the TF commit hash expect it here.
#
# To update TensorFlow to a new revision.
# 1. Update the 'git_commit' args below to include the new git hash.
# 2. Get the sha256 hash of the archive with a command such as...
#    curl -L https://github.com/tensorflow/tensorflow/archive/<git hash>.tar.gz | sha256sum
#    and update the 'sha256' arg with the result.
# 3. Request the new archive to be mirrored on mirror.bazel.build for more
#    reliable downloads.
load("//tensorflow_serving:repo.bzl", "tensorflow_http_archive")
tensorflow_http_archive(
    name = "org_tensorflow",
    sha256 = "6438396f3b19af5d7ad787cf041f857af7505916dc08092e20b07d1b1f8df492",
    git_commit = "a481b10260dfdf833a1b16007eead49c1d7febf3",
    patch = "//third_party/tensorflow:tensorflow.patch",
    patch_cmds = [
        "sed -i 's/__attribute__((aligned(sizeof(T))))/__attribute__((aligned(8)))/g' tensorflow/core/kernels/concat_lib_gpu_impl.cu.cc",
        # NcclSymmetricMemory uses %v (requires AbslStringify) but only implements ToString(); use it directly.
        "sed -i 's/absl::StrFormat(\"Destroy %v\", \\*this)/\"Destroy \" + ToString()/g' third_party/xla/xla/backends/gpu/collectives/nccl_symmetric_memory.cc",
        # NcclSymmetricMemory requires ncclWindow_t (NCCL >= 2.28.0 / version code 22800).
        # The hermetic NCCL for CUDA 12.8.1 is 2.26.5 (code 22605). Guard identically to nccl_communicator.h.
        "sed -i 's|^class NcclSymmetricMemory final|#if NCCL_VERSION_CODE >= 22800\\nclass NcclSymmetricMemory final|' third_party/xla/xla/backends/gpu/collectives/nccl_symmetric_memory.h",
        "sed -i 's|^}  // namespace xla::gpu|#endif  // NCCL_VERSION_CODE >= 22800\\n}  // namespace xla::gpu|' third_party/xla/xla/backends/gpu/collectives/nccl_symmetric_memory.h",
        "sed -i 's|^namespace xla::gpu {|namespace xla::gpu {\\n#if NCCL_VERSION_CODE >= 22800|' third_party/xla/xla/backends/gpu/collectives/nccl_symmetric_memory.cc",
        "sed -i 's|^}  // namespace xla::gpu|#endif  // NCCL_VERSION_CODE >= 22800\\n}  // namespace xla::gpu|' third_party/xla/xla/backends/gpu/collectives/nccl_symmetric_memory.cc",
        "sed -i '/cc_library = _cc_library/d' tensorflow/core/platform/rules_cc.bzl",
        "echo -e \"\\ndef cc_library_oss(deps=[], **kwargs):\\n    if kwargs.get(\\\"name\\\") == \\\"lib_internal_impl\\\" or \\\"protobuf\\\" in kwargs.get(\\\"name\\\", \\\"\\\"):\\n        _cc_library(deps = deps, **kwargs)\\n        return\\n    if type(deps) == \\\"list\\\":\\n        if \\\"@com_google_protobuf//:protobuf\\\" not in deps:\\n            deps = deps + [\\\"@com_google_protobuf//:protobuf\\\"]\\n    else:\\n        deps = deps + [\\\"@com_google_protobuf//:protobuf\\\"]\\n    _cc_library(deps = deps, **kwargs)\\ncc_library = cc_library_oss\" >> tensorflow/core/platform/rules_cc.bzl",
        "sed -i 's#deps = \\[op_gen\\] + deps#deps = [op_gen] + deps + [clean_dep(\"//tensorflow/core/framework:kernel_shape_util\"), clean_dep(\"//tensorflow/core/framework:full_type_util\")]#' tensorflow/tensorflow.bzl",
        "sed -i '/name = \"kernel_shape_util\",/a \\    visibility = [\"//visibility:public\"],' tensorflow/core/framework/BUILD",
        "echo -e '\\nalias(name = \"tensorflow_libtensorflow_framework\", actual = \"//tensorflow/core:tensorflow\", visibility = [\"//visibility:public\"])' >> BUILD",
        "echo -e '\\nalias(name = \"tensorflow_tf_header_lib\", actual = \"//tensorflow/core:tensorflow\", visibility = [\"//visibility:public\"])' >> BUILD",
    ],
)

# Import all of TensorFlow Serving's external dependencies.
# Downstream projects (projects importing TensorFlow Serving) need to
# duplicate all code below in their WORKSPACE file in order to also initialize
# those external dependencies.
load("//tensorflow_serving:workspace.bzl", "tf_serving_workspace")
tf_serving_workspace()

# Check bazel version requirement, which is stricter than TensorFlow's.
load("@bazel_skylib//lib:versions.bzl", "versions")
versions.check("7.4.1")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "bazel_skylib",
    sha256 = "fa01292859726603e3cd3a0f3f29625e68f4d2b165647c72908045027473e933",
    urls = [
        "https://storage.googleapis.com/mirror.tensorflow.org/github.com/bazelbuild/bazel-skylib/releases/download/1.8.0/bazel-skylib-1.8.0.tar.gz",
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.8.0/bazel-skylib-1.8.0.tar.gz",
    ],
)

http_archive(
    name = "rules_cc",
    sha256 = "b8b918a85f9144c01f6cfe0f45e4f2838c7413961a8ff23bc0c6cdf8bb07a3b6",
    strip_prefix = "rules_cc-0.1.5",
    url = "https://github.com/bazelbuild/rules_cc/releases/download/0.1.5/rules_cc-0.1.5.tar.gz",
)

# Initialize TensorFlow's external dependencies.
# tf_workspace3 must come first: it defines @xla (via tf_vendored), which is
# required by python_init_rules for its patch file references.
load("@org_tensorflow//tensorflow:workspace3.bzl", "tf_workspace3")
tf_workspace3()

# Initialize hermetic Python (must follow tf_workspace3 so @xla is defined)
load("@org_tensorflow//third_party/py:python_init_rules.bzl", "python_init_rules")
python_init_rules()

load("@org_tensorflow//third_party/py:python_init_repositories.bzl", "python_init_repositories")
python_init_repositories(
    default_python_version = "system",
    requirements = {
        "3.10": "@org_tensorflow//:requirements_lock_3_10.txt",
        "3.11": "@org_tensorflow//:requirements_lock_3_11.txt",
        "3.12": "@org_tensorflow//:requirements_lock_3_12.txt",
        "3.13": "@org_tensorflow//:requirements_lock_3_13.txt",
        "3.14": "@org_tensorflow//:requirements_lock_3_14.txt",
    },
)

load("@org_tensorflow//third_party/py:python_init_toolchains.bzl", "python_init_toolchains")
python_init_toolchains()

load("@org_tensorflow//third_party/py:python_init_pip.bzl", "python_init_pip")
python_init_pip()

load("@pypi//:requirements.bzl", "install_deps")
install_deps()

# Toolchains for ML projects hermetic builds.
# Details: https://github.com/google-ml-infra/rules_ml_toolchain
http_archive(
    name = "rules_ml_toolchain",
    sha256 = "54c1a357f71f611efdb4891ebd4bcbe4aeb6dfa7e473f14fd7ecad5062096616",
    strip_prefix = "rules_ml_toolchain-d8cb9c2c168cd64000eaa6eda0781a9615a26ffe",
    urls = [
        "https://github.com/google-ml-infra/rules_ml_toolchain/archive/d8cb9c2c168cd64000eaa6eda0781a9615a26ffe.tar.gz",
    ],
)

load(
    "@rules_ml_toolchain//cc/deps:cc_toolchain_deps.bzl",
    "cc_toolchain_deps",
)

cc_toolchain_deps()

register_toolchains("@rules_ml_toolchain//cc:linux_x86_64_linux_x86_64")
register_toolchains("@rules_ml_toolchain//cc:linux_x86_64_linux_x86_64_cuda")
# register_toolchains("@rules_ml_toolchain//cc:linux_aarch64_linux_aarch64")
# register_toolchains("@rules_ml_toolchain//cc:linux_aarch64_linux_aarch64_cuda")

load("@org_tensorflow//tensorflow:workspace2.bzl", "tf_workspace2")
tf_workspace2()
load("@org_tensorflow//tensorflow:workspace1.bzl", "tf_workspace1")
tf_workspace1()
load("@org_tensorflow//tensorflow:workspace0.bzl", "tf_workspace0")
tf_workspace0()

# Initialize bazel package rules' external dependencies.
load("@rules_pkg//:deps.bzl", "rules_pkg_dependencies")
rules_pkg_dependencies()

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

http_archive(
    name = "rules_shell",
    sha256 = "0d0c56d01c3c40420bf7bf14d73113f8a92fbd9f5cd13205a3b89f72078f0321",
    strip_prefix = "rules_shell-0.1.1",
    urls = [
        "https://github.com/bazelbuild/rules_shell/releases/download/v0.1.1/rules_shell-v0.1.1.tar.gz",
    ],
)

load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies")

rules_proto_dependencies()

load(
    "@xla//third_party/py:python_wheel.bzl",
    "python_wheel_version_suffix_repository",
)

python_wheel_version_suffix_repository(name = "tf_wheel_version_suffix")

load(
    "@rules_ml_toolchain//gpu/cuda:cuda_json_init_repository.bzl",
    "cuda_json_init_repository",
)

cuda_json_init_repository()

load(
    "@cuda_redist_json//:distributions.bzl",
    "CUDA_REDISTRIBUTIONS",
    "CUDNN_REDISTRIBUTIONS",
)
load(
    "@rules_ml_toolchain//gpu/cuda:cuda_redist_init_repositories.bzl",
    "cuda_redist_init_repositories",
    "cudnn_redist_init_repository",
)
load(
    "@rules_ml_toolchain//gpu/cuda:cuda_redist_versions.bzl",
    "REDIST_VERSIONS_TO_BUILD_TEMPLATES",
)
load("@xla//third_party/cccl:workspace.bzl", "CCCL_DIST_DICT", "CCCL_GITHUB_VERSIONS_TO_BUILD_TEMPLATES")

cuda_redist_init_repositories(
    cuda_redistributions = CUDA_REDISTRIBUTIONS | CCCL_DIST_DICT,
    redist_versions_to_build_templates = REDIST_VERSIONS_TO_BUILD_TEMPLATES | CCCL_GITHUB_VERSIONS_TO_BUILD_TEMPLATES,
)

cudnn_redist_init_repository(
    cudnn_redistributions = CUDNN_REDISTRIBUTIONS,
)

load(
    "@rules_ml_toolchain//gpu/cuda:cuda_configure.bzl",
    "cuda_configure",
)

cuda_configure(name = "local_config_cuda")

load(
    "@rules_ml_toolchain//gpu/nccl:nccl_redist_init_repository.bzl",
    "nccl_redist_init_repository",
)

nccl_redist_init_repository()

load(
    "@rules_ml_toolchain//gpu/nccl:nccl_configure.bzl",
    "nccl_configure",
)

nccl_configure(name = "local_config_nccl")

load(
    "@rules_ml_toolchain//gpu/nvshmem:nvshmem_json_init_repository.bzl",
    "nvshmem_json_init_repository",
)

nvshmem_json_init_repository()

load(
    "@nvshmem_redist_json//:distributions.bzl",
    "NVSHMEM_REDISTRIBUTIONS",
)
load(
    "@rules_ml_toolchain//gpu/nvshmem:nvshmem_redist_init_repository.bzl",
    "nvshmem_redist_init_repository",
)

nvshmem_redist_init_repository(
    nvshmem_redistributions = NVSHMEM_REDISTRIBUTIONS,
)

