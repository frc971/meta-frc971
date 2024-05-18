SUMMARY = "Example of how to build an external Linux kernel module"
DESCRIPTION = "${SUMMARY}"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${WORKDIR}/usr/src/canivore-usb-1.12/canivore-usb-drv.c;begineline=6;endline=14;md5=7ea7c5833705625b82e746f2748a3ef2"

inherit module

SRC_URI = "https://deb.ctr-electronics.com/tools/packages/canivore-usb_1.12_arm64.deb"
SRC_URI += "file://Makefile"
SRC_URI[sha256sum] = "e3f63b95f39fd78e9e840d4bf599ae504e219f2c3d364bce9ea76a85bb1d2fe7"

S = "${WORKDIR}/usr/src/canivore-usb-1.12"

do_setup() {
    cp "${WORKDIR}/Makefile" "${WORKDIR}/usr/src/canivore-usb-1.12"
    mv "${WORKDIR}/usr/src/canivore-usb-1.12/canivore-usb-util.10.o_shipped" \
        "${WORKDIR}/usr/src/canivore-usb-1.12/canivore-usb-util.o_shipped"

    rm "${WORKDIR}/usr/src/canivore-usb-1.12/caniv"
    rm "${WORKDIR}/usr/src/canivore-usb-1.12/canivore_setup"
    rm "${WORKDIR}/usr/src/canivore-usb-1.12/canivore_shutdown"
    rm "${WORKDIR}/usr/src/canivore-usb-1.12/dkms.conf"
}

addtask do_setup after do_fetch before do_compile

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.

RPROVIDES:${PN} += "kernel-module-canivore-usb"
