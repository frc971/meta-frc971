SUMMARY = "BCC"
DESCRIPTION = "${SUMMARY}"
LICENSE = "Apache-2.0-only"
LIC_FILES_CHKSUM="file://${WORKDIR}/bcc/LICENSE.txt;md5=e3fc50a88d0a364313df4b21ef20c29e"

inherit module

SRC_URI = "https://github.com/iovisor/bcc/releases/download/v0.30.0/bcc-src-with-submodule.tar.gz"
SRC_URI[sha256sum] = "c74ed8055a72625a5df341e067c67e02ac3d94a2e765505a17b723ffe6d77aca"

S="${WORKDIR}"

DEPENDS = "cmake-native bison-native flex-native llvm clang"
