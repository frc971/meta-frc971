FILESEXTRAPATHS:prepend := "${THISDIR}/linux-tegra-5.10/:"

SRCBRANCH = "tegra-5.10-austin-rebase"
SRCREV = "d49fdb78d0d2b23d6ae85f204835322888a8c008"

SRC_REPO = "github.com/frc971/linux.git;protocol=https;branch=${SRCBRANCH}"
#SRC_REPO = "file:///home/austin/local/linux-tegra-5.10/.git;protocol=file;branch=${SRCBRANCH}"

SRC_URI += "file://config_preempt_rt.cfg"
SRC_URI += "file://config_fan.cfg"
SRC_URI += "file://config_xfs.cfg"
SRC_URI += "file://config_global_shutter_camera.cfg"
SRC_URI += "file://config_sctp.cfg"
