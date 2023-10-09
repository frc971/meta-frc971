LICENSE = "OPEN"

SUMMARY = "Image for NX 8GB"

inherit core-image

IMAGE_FEATURES += "splash ssh-server-openssh"

IMAGE_INSTALL = "\
    packagegroup-core-boot \
    packagegroup-core-full-cmdline \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    "

IMAGE_INSTALL:append = "\
    kernel-modules \
    cuda-samples \
    argus-samples \
    gstreamer1.0-plugins-nvarguscamerasrc \
    tegra-argus-daemon \
    tegra-nvphs \
    gstreamer1.0-plugins-tegra \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-nvdrmvideosink \
    gstreamer1.0-plugins-nveglgles \
    gstreamer1.0-plugins-nvjpeg \
    gstreamer1.0-plugins-nvv4l2camerasrc \
    gstreamer1.0-plugins-nvvidconv \
    gstreamer1.0-plugins-nvvideo4linux2 \
    gstreamer1.0-rtsp-server \
    rsync \
    vim \
    strace \
    media-ctl \
    gstreamer1.0-plugins-bad \
    can-utils \
    frc971-dotfiles \
    bash-completion \
    linux-firmware-rtl8168 \
    e2fsprogs-tune2fs \
    e2fsprogs-resize2fs \
    v4l-utils \
"

#    nv-imx296-mod 
#    tensorrt-plugins-prebuilt 
#    tensorrt-core 

ROOT_HOME="/root"
VOLATILE_LOG_DIR="no"
