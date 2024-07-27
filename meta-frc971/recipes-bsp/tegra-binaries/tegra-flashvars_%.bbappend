FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:append = " file://tegra234-mb1-bct-pinmux-p3767-frc971.dtsi \
 file://tegra234-mb1-bct-gpio-p3767-frc971.dtsi \
"

do_install:append() {
    install -d ${D}${datadir}/tegraflash
    install -m 0644 ${S}/*.dtsi ${D}${datadir}/tegraflash/
}
