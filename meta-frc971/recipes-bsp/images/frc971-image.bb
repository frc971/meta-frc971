LICENSE = "OPEN"

SUMMARY = "Image for NX 8GB"

inherit core-image

IMAGE_FEATURES += "splash ssh-server-openssh x11"

IMAGE_INSTALL = "\
    packagegroup-core-boot \
    packagegroup-core-full-cmdline-utils \
    packagegroup-core-full-cmdline-dev-utils \
    packagegroup-core-full-cmdline-multiuser \
    packagegroup-core-full-cmdline-initscripts \
    packagegroup-core-full-cmdline-sys-services \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    "

inherit extrausers

GROUPADD_PARAM:${PN} = "debug; crypto; trusty"

IMAGE_INSTALL:append = "\
    kernel-modules \
    cuda-samples-11-8 \
    argus-samples \
    gstreamer1.0-plugins-nvarguscamerasrc \
    tegra-argus-daemon \
    tegra-libraries-camera \
    tegra-libraries-core \
    tegra-libraries-multimedia \
    tegra-mmapi-dev \
    libffi \
    argus-samples \
    opencv \
    opencv-dev \
    libvdpau \
    libva \
    libopencv-core-dev \
    libopencv-imgproc-dev \
    libopencv-highgui-dev \
    libopencv-videoio-dev \
    libopencv-imgcodecs-dev \
    tegra-nvphs \
    gstreamer1.0-plugins-tegra-dev \
    gstreamer1.0-plugins-good-dev \
    gstreamer1.0-plugins-base-dev \
    gstreamer1.0-plugins-nvdrmvideosink-dev \
    gstreamer1.0-plugins-nveglgles-dev \
    gstreamer1.0-plugins-nvjpeg-dev \
    gstreamer1.0-plugins-nvv4l2camerasrc-dev \
    gstreamer1.0-plugins-nvvidconv-dev \
    gstreamer1.0-plugins-nvvideo4linux2-dev \
    gstreamer1.0-rtsp-server-dev \
    rsync \
    vim \
    strace \
    media-ctl \
    gstreamer1.0-plugins-bad-dev \
    alsa-lib-dev \
    can-utils \
    bash-completion \
    linux-firmware-rtl8168 \
    e2fsprogs-tune2fs \
    e2fsprogs-resize2fs \
    v4l-utils \
    nv-imx296-mod \
    trace-cmd \
    sysstat \
    gdb \
    libcurand-11-8-dev \
    libnpp-11-8-dev \
    cuda-cudart-11-8-dev \
    cuda-target-environment \
    cuda-nvcc-11-8 \
    cuda-nvcc-headers-11-8 \
    tegra-tools-jetson-clocks \
    nsight-systems-cli \
    nsight-systems-cli-qdstrmimporter \
    sudo \
    python3 \
    git \
    cpufrequtils \
    openssh \
    rsync \
    vim \
    libnice-dev \
    feh \
    usbutils \
    trace-cmd \
    clinfo \
    jq \
    strace \
    sysstat \
    can-utils \
    net-tools \
    parted \
    file \
    gdb \
    smartmontools \
    nvme-cli \
    psmisc \
    chrony \
    lmsensors \
    xfsprogs \
    bridge-utils \
    glibc-utils \
    localedef \
    libxkbfile \
    autossh \
    gtk+3 \
    libsrtp \
    libglvnd \
    glib-2.0-dev \
    libxcb-dev \
    libxcb-glx \
    zlib \
    wayland \
    wayland-dev \
    systemd \
    wget \
    gnupg \
    users-pi \
    gcc \
    libstdc++ \
    libgcc \
    frc971-dotfiles \
    cuda-toolkit-11-8 \
"

PACKAGE_EXCLUDE = "dwarfsrcfiles elfutils libmicrohttpd curl openssl"

# TODO: libglib-2.0-0
# TODO: gstreamer1.0-nice

#    nv-imx296-mod 
#    tensorrt-plugins-prebuilt 
#    tensorrt-core 

ROOT_HOME="/root"
VOLATILE_LOG_DIR="no"
