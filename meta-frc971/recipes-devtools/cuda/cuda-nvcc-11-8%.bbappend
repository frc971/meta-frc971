SRC_URI += "http://http.us.debian.org/debian/pool/non-free/n/nvidia-cuda-toolkit/nvidia-cuda-toolkit_11.8.89~11.8.0-5~deb12u1_amd64.deb"

SRC_URI[sha256sum] = "d1156026e704fe0368b6295db06224ed00a727966fb13454f34e3f3102db0457"

do_install:append() {
    cp ../usr/bin/ptxas ${D}${prefix}/local/cuda-${CUDA_VERSION}/bin/ptxas
    cp ../usr/bin/fatbinary ${D}${prefix}/local/cuda-${CUDA_VERSION}/bin/fatbinary
}

FILES:${PN} += "${base_prefix}/usr/local/cuda-${CUDA_VERSION}/bin/ptxas"
FILES:${PN} += "${base_prefix}/usr/local/cuda-${CUDA_VERSION}/bin/fatbinary"

INSANE_SKIP:${PN} = "already-stripped arch file-rdeps ldflags dev-so libdir"
