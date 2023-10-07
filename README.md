To build, start by making a directory.

```
mkdir yocto
```

Then, make sure you've moved into the directory
```
cd yocto
```

Then clone all the repos, making sure meta-frc971 lives inside yocto.
```
git clone git://git.yoctoproject.org/poky -b kirkstone
git clone https://github.com/OE4T/meta-tegra.git -b kirkstone
git clone https://github.com/openembedded/meta-openembedded.git -b kirkstone
git clone https://github.com/frc971/meta-frc971.git -b main
```

Then, symlink in the conf files.

```
mkdir -p build/conf
ln -s ../../meta-frc971/conf/bblayers.conf build/conf/bblayers.conf
ln -s ../../meta-frc971/conf/local.conf build/conf/local.conf
ln -s ../../meta-frc971/conf/templateconf.cfg build/conf/templateconf.cfg
```

To enter the yocto environment building for the 8g Orin NX, run:

```
export MACHINE=orin-nx-8g
. poky/oe-init-build-env build
```


To then build, run:

```
bitbake frc971-image
```

The image will live in `build/tmp/deploy/images/orin-nx-8g/frc971-image-orin-nx-8g.tegraflash.tar.gz`

To install, extract then run:
```
sudo ./initrd-flash
```

To build the SDK for the sysroot, run:
```
bitbake frc971-sysroot-image -c populate_sdk
```


To use libargus to encode and send video:
```
gst-launch-1.0 nvarguscamerasrc ! "video/x-raw(memory:NVMM),width=1920,height=1080,framerate=60/1" ! nvvidconv ! nvv4l2h264enc insert-sps-pps=1 idrinterval=15 ! h264parse ! rtph264pay ! udpsink host=10.9.71.13 port=5000 sync=0
```

And to receive it on your laptop:
```
gst-launch-1.0 udpsrc port=5000 ! 'application/x-rtp,encoding-name=H264,payload=96' ! rtph264depay ! avdec_h264 ! xvimagesink sync=0
```
