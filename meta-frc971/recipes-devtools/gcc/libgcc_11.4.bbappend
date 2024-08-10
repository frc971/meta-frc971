ROOT = "${D}${base_prefix}"

do_install:append() {
    mkdir -p ${ROOT}/usr/lib/gcc/${TARGET_SYS}/11.4.0/
    cp ${ROOT}/usr/lib/${TARGET_SYS}/11.4.0/*.o ${ROOT}/usr/lib/gcc/${TARGET_SYS}/11.4.0/
}

FILES:${PN}:append = " ${base_prefix}/usr/lib/gcc/${TARGET_SYS}/11.4.0/"
