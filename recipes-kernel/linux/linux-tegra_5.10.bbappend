FILESEXTRAPATHS:prepend := "${THISDIR}/linux-tegra-5.10/:"

SRCBRANCH = "tegra-5.10-austin-rebase"
#SRCREV = "dd798a08b905f76b75560b87c9ed1ab807c28dbc"
SRCREV = "46341252e0fcad7100436b925eebd1e53e94db97"

#SRC_REPO = "github.com/frc971/linux.git;protocol=https;branch=${SRCBRANCH}"
SRC_REPO = "file:///home/austin/local/linux-tegra-5.10/.git;protocol=file;branch=${SRCBRANCH}"

SRC_URI += "file://config_preempt_rt.cfg"
SRC_URI += "file://config_fan.cfg"
SRC_URI += "file://config_xfs.cfg"
SRC_URI += "file://config_global_shutter_camera.cfg"
