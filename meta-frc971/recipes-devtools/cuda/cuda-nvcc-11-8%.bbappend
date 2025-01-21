SRC_URI += "http://http.us.debian.org/debian/pool/non-free/n/nvidia-cuda-toolkit/nvidia-cuda-toolkit_11.8.89~11.8.0-5~deb12u1_amd64.deb"

SRC_URI[sha256sum] = "d1156026e704fe0368b6295db06224ed00a727966fb13454f34e3f3102db0457"

# The reason why we do this is because bazel requires an x86 version of these two binaries
# in the sysroot because it runs them out of sandbox.
# There is definitely a better way to do this but it works for now and doesn't cause any immediate issues.
do_install:append() {
    cp ../usr/bin/ptxas ${D}${prefix}/local/cuda-${CUDA_VERSION}/bin/ptxas
    cp ../usr/bin/fatbinary ${D}${prefix}/local/cuda-${CUDA_VERSION}/bin/fatbinary
}

FILES:${PN} += "${base_prefix}/usr/local/cuda-${CUDA_VERSION}/bin/ptxas"
FILES:${PN} += "${base_prefix}/usr/local/cuda-${CUDA_VERSION}/bin/fatbinary"

INSANE_SKIP:${PN} = "already-stripped arch file-rdeps ldflags dev-so libdir"
