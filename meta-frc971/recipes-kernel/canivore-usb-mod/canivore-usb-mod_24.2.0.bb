SUMMARY = "Example of how to build an external Linux kernel module"
DESCRIPTION = "${SUMMARY}"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${WORKDIR}/usr/src/canivore-usb-1.12/canivore-usb-drv.c;begineline=6;endline=14;md5=7ea7c5833705625b82e746f2748a3ef2"

inherit module

SRC_URI = "https://deb.ctr-electronics.com/tools/packages/canivore-usb_1.12_arm64.deb"
SRC_URI += "file://Makefile"
SRC_URI[sha256sum] = "e3f63b95f39fd78e9e840d4bf599ae504e219f2c3d364bce9ea76a85bb1d2fe7"

S = "${WORKDIR}/usr/src/canivore-usb-1.12"
ROOT = "${D}${base_prefix}"

install_content() {
    install -D -m"$1" "$2" "$3"
}

do_setup() {
    cp "${WORKDIR}/Makefile" "${S}"

    mv "${WORKDIR}/usr/src/canivore-usb-1.12/canivore-usb-util.10.o_shipped" \
        "${WORKDIR}/usr/src/canivore-usb-1.12/canivore-usb-util.o_shipped"
}

addtask do_setup after do_fetch before do_compile

do_install:prepend() {
    mkdir -p ${ROOT}/usr/bin/
    install_content 0777 "${S}/caniv" "${ROOT}/usr/bin/caniv"
    install_content 0777 "${S}/canivore_setup" "${ROOT}/usr/bin/canivore_setup"
    install_content 0777 "${S}/canivore_shutdown" "${ROOT}/usr/bin/canivore_shutdown"
}

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.

RPROVIDES:${PN} += "kernel-module-canivore-usb"

FILES:${PN}:append = " ${base_prefix}/usr/bin/caniv"
FILES:${PN}:append = " ${base_prefix}/usr/bin/canivore_setup"
FILES:${PN}:append = " ${base_prefix}/usr/bin/canivore_shutdown"


INSANE_SKIP:${PN} = "already-stripped "
