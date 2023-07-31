DESCRIPTION = "Dotfiles"
SECTION = "conf"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
PR = "r0"

SRC_URI:append = " file://root/bashrc"
SRC_URI:append = " file://camera_overrides.isp"

S = "${WORKDIR}"

do_compile() {
}

do_install() {
   install -D -m0755 root/bashrc ${D}${base_prefix}${ROOT_HOME}/.bashrc
   install -D -m0755 root/bashrc ${D}${base_prefix}${ROOT_HOME}/.bash_profile
   install -D -m0755 ${WORKDIR}/camera_overrides.isp ${D}${base_prefix}/var/nvidia/nvcam/settings/camera_overrides.isp
}

RDEPENDS:${PN} += " git"
RDEPENDS:${PN} += " bash-completion git-bash-completion"

FILES:${PN} = "${base_prefix}${ROOT_HOME}/.bashrc"
FILES:${PN}:append = " ${base_prefix}${ROOT_HOME}/.bash_profile"
FILES:${PN}:append = " ${base_prefix}/var/nvidia/nvcam/settings/camera_overrides.isp"
