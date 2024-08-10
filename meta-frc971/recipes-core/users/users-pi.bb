SUMMARY = "Adds the pi user"
SECTION = "conf"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
PR = "r0"

DOTFILES_REF="5f61e185231919b89233b841841ef30c37bc63be"

SRC_URI:append = " file://home/pi/.ssh/authorized_keys"
SRC_URI:append = " https://github.com/AustinSchuh/.dotfiles/archive/${DOTFILES_REF}.zip"
SRC_URI[sha256sum] = "bf7ae9cad99461a92ae43e5e2ea9ebe4951d34f42d9004990d8ff6628bacb272"

PACKAGES =+ "${PN}-pi"

S = "${WORKDIR}"

inherit useradd

USERADD_PACKAGES = "${PN}"

USERADD_PARAM:${PN} = "-m -p \"\\\$y\\\$j9T\\\$85lzhdky63CTj.two7Zj20\\\$pVY53UR0VebErMlm8peyrEjmxeiRw/rfXfx..9.xet1\" -s /bin/bash pi;"

GROUPADD_PARAM:${PN} = "sudo; video; system-journal; dialout"

do_install() {
    mkdir -p -m755 ${D}${base_prefix}/home/pi/.ssh
    chown pi:pi ${D}${base_prefix}/home/pi/.ssh

    install -D -m0600 home/pi/.ssh/authorized_keys ${D}${base_prefix}/home/pi/.ssh/authorized_keys
    chown pi:pi ${D}${base_prefix}/home/pi/.ssh/authorized_keys

    mkdir -p ${D}${base_prefix}/home/pi/.dotfiles
    cp -rv ${WORKDIR}/.dotfiles-${DOTFILES_REF}/. ${D}${base_prefix}/home/pi/

    mkdir -p ${D}${sysconfdir}/sudoers.d/

    echo "pi ALL=(ALL) ALL" > ${D}${sysconfdir}/sudoers.d/001_pi
}

RDEPENDS:${PN} += " git bash vim"

FILES:${PN} = "${base_prefix}/home/pi/"
FILES:${PN}:append = " ${base_prefix}/etc/sudoers.d/001_pi"
