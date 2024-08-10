DESCRIPTION = "Dotfiles"
SECTION = "conf"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
PR = "r0"

DOTFILES_REF="5f61e185231919b89233b841841ef30c37bc63be"

SRC_URI:append = " file://root/"
SRC_URI:append = " file://camera_overrides.isp"
SRC_URI:append = " file://contents/"
SRC_URI:append = " https://github.com/AustinSchuh/.dotfiles/archive/${DOTFILES_REF}.zip"
SRC_URI[sha256sum] = "bf7ae9cad99461a92ae43e5e2ea9ebe4951d34f42d9004990d8ff6628bacb272"

S = "${WORKDIR}"

SYSTEMD_AUTO_ENABLE:${PN} = "enable"
SYSTEMD_SERVICE:${PN} = "grow-rootfs jetson-clocks frc971 frc971chrt hwclock"

ROOT = "${D}${base_prefix}"

install_content() {
    install -D -m"$1" "contents/$2" "${ROOT}/$2"
    chown "$3" "${ROOT}/$2"
}

do_install() {
    install -D -m0755 root/bashrc ${ROOT}${ROOT_HOME}/.bashrc
    install -D -m0755 root/bashrc ${ROOT}${ROOT_HOME}/.bash_profile
    install -D -m0755 root/trace.sh ${ROOT}${ROOT_HOME}/trace.sh
    install -D -m0755 ${WORKDIR}/camera_overrides.isp ${ROOT}/var/nvidia/nvcam/settings/camera_overrides.isp

    install_content 0644 "etc/sysctl.d/sctp.conf" "root:root"
    install_content 0644 "etc/systemd/logind.conf" "root:root"
    install_content 0555 "etc/bash_completion.d/aos_dump_autocomplete" "root:root"
    install_content 0644 "etc/security/limits.d/rt.conf" "root:root"
    install_content 0644 "etc/systemd/system/usb-mount@.service" "root:root"
    install_content 0644 "etc/chrony/chrony.conf" "root:root"

    mkdir -p -m700 ${ROOT}/root/bin
    chown root:root ${ROOT}/root/bin

    install_content 0644 "etc/systemd/system/grow-rootfs.service" "root:root"
    install_content 0644 "etc/systemd/system/frc971.service" "root:root"
    install_content 0644 "etc/systemd/system/frc971chrt.service" "root:root"
    install_content 0644 "etc/systemd/system/jetson-clocks.service" "root:root"
    install_content 0500 "root/bin/change_hostname.sh" "root:root"
    install_content 0500 "root/bin/chrt.sh" "root:root"
    install_content 0700 "root/trace.sh" "root:root"
    install_content 0440 "etc/sudoers" "root:root"
    install_content 0644 "etc/fstab" "root:root"
    install_content 0644 "etc/modprobe.d/audio.conf" "root:root"
    install_content 0644 "etc/modprobe.d/can.conf" "root:root"
    install_content 0644 "var/nvidia/nvcam/settings/camera_overrides.isp" "root:root" 
    install_content 0644 "etc/ld.so.conf.d/yocto.conf" "root:root"
    install_content 0644 "etc/systemd/network/eth0.network" "root:root"
    install_content 0644 "etc/systemd/network/80-cana.network" "root:root"
    install_content 0644 "etc/systemd/network/80-canb.network" "root:root"
    install_content 0644 "etc/systemd/network/80-canc.network" "root:root"
    install_content 0644 "etc/udev/rules.d/nvidia.rules" "root:root"
    install_content 0644 "etc/udev/rules.d/can.rules" "root:root"

    # Setup hwclock
    install_content 0644 "etc/systemd/system/hwclock.service" "root:root"

    cp -rv ${WORKDIR}/.dotfiles-${DOTFILES_REF}/. ${ROOT}${ROOT_HOME}
}

RDEPENDS:${PN} += " git"
RDEPENDS:${PN} += " bash-completion git-bash-completion"
RDEPENDS:${PN} += " bash"

FILES:${PN} = "${base_prefix}${ROOT_HOME}/"
FILES:${PN}:append = " ${base_prefix}${ROOT_HOME}/.bash_profile"
FILES:${PN}:append = " ${base_prefix}${ROOT_HOME}/trace.sh"
FILES:${PN}:append = " ${base_prefix}/var/nvidia/nvcam/settings/camera_overrides.isp"
FILES:${PN}:append = " ${base_prefix}/etc/sysctl.d/sctp.conf"
FILES:${PN}:append = " ${base_prefix}/etc/systemd/logind.conf"
FILES:${PN}:append = " ${base_prefix}/etc/bash_completion.d/aos_dump_autocomplete"
FILES:${PN}:append = " ${base_prefix}/etc/security/limits.d/rt.conf"
FILES:${PN}:append = " ${base_prefix}/etc/systemd/system/usb-mount@.service"
FILES:${PN}:append = " ${base_prefix}/etc/chrony/chrony.conf"
FILES:${PN}:append = " ${base_prefix}/root/bin"
FILES:${PN}:append = " ${base_prefix}/etc/systemd/system/grow-rootfs.service"
FILES:${PN}:append = " ${base_prefix}/etc/systemd/system/frc971.service"
FILES:${PN}:append = " ${base_prefix}/etc/systemd/system/frc971chrt.service"
FILES:${PN}:append = " ${base_prefix}/etc/systemd/system/jetson-clocks.service"
FILES:${PN}:append = " ${base_prefix}/root/bin/change_hostname.sh"
FILES:${PN}:append = " ${base_prefix}/root/bin/chrt.sh"
FILES:${PN}:append = " ${base_prefix}/root/trace.sh"
FILES:${PN}:append = " ${base_prefix}/etc/sudoers"
FILES:${PN}:append = " ${base_prefix}/etc/fstab"
FILES:${PN}:append = " ${base_prefix}/etc/modprobe.d/audio.conf"
FILES:${PN}:append = " ${base_prefix}/etc/modprobe.d/can.conf"
FILES:${PN}:append = " ${base_prefix}/var/nvidia/nvcam/settings/camera_overrides.isp"
FILES:${PN}:append = " ${base_prefix}/etc/ld.so.conf.d/yocto.conf"
FILES:${PN}:append = " ${base_prefix}/etc/systemd/network"
FILES:${PN}:append = " ${base_prefix}/etc/systemd/network/eth0.network"
FILES:${PN}:append = " ${base_prefix}/etc/systemd/network/80-cana.network"
FILES:${PN}:append = " ${base_prefix}/etc/systemd/network/80-canb.network"
FILES:${PN}:append = " ${base_prefix}/etc/systemd/network/80-canc.network"
FILES:${PN}:append = " ${base_prefix}/etc/udev/rules.d/nvidia.rules"
FILES:${PN}:append = " ${base_prefix}/etc/udev/rules.d/can.rules"
FILES:${PN}:append = " ${base_prefix}/lib/systemd/system/nvargus-daemon.service"
FILES:${PN}:append = " ${base_prefix}/etc/systemd/system/hwclock.service"
