FILESEXTRAPATHS:prepend := "${THISDIR}/linux-tegra-5.10/:"

SRCBRANCH = "tegra-5.10-austin"
SRCREV = "c244ad8d861cede034fea2219cfa21c310561aca"

SRC_REPO = "github.com/frc971/linux.git;protocol=https;branch=${SRCBRANCH}"

SRC_URI += "file://config_preempt_rt.cfg"
SRC_URI += "file://config_fan.cfg"
SRC_URI += "file://config_xfs.cfg"
SRC_URI += "file://config_global_shutter_camera.cfg"
