oe_mkxfs() {
    dd if=/dev/zero of=${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.xfs seek=${ROOTFS_SIZE} count=0 bs=1k

    /sbin/mkfs.xfs ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.xfs 

    PWD=$(pwd)
    cd ${IMAGE_ROOTFS}
    echo ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.xfs
    /sbin/losetup -fP ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.xfs
    find ${IMAGE_ROOTFS} -depth -print0 | cpio --null -p --make-directories --preserve-modification-time --sparse ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.xfs
	# Error codes 0-3 indicate successfull operation of fsck (no errors or errors corrected)
	fsck.vfat -pvfV ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.xfs
    cd $PWD
}

