FILESEXTRAPATHS:append := ":${THISDIR}/${PN}/"

SRC_URI:append = "file://eth0.network"

PACKAGECONFIG = ""

do_install:append() {
    install -D -m0644 ${WORKDIR}/eth0.network ${D}${base_prefix}/etc/systemd/network/eth0.network
}

FILES:${PN}:append = "\
    ${base_prefix}/etc/systemd/network/ \
"
