To build, start by making a directory.

```
mkdir yocto
```

Then clone all the repos.
```
git clone git://git.yoctoproject.org/poky -b kirkstone
git clone https://github.com/OE4T/meta-tegra.git -b kirkstone
git clone https://github.com/openembedded/meta-openembedded.git -b kirkstone
git clone https://github.com/frc971/meta-frc971.git -b main
```

Then, symlink in the conf files.

```
mkdir build/conf
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
bitbake core-image-full-cmdline
```

The image will live in `build/tmp/deploy/images/orin-nx-8g/core-image-full-cmdline-orin-nx-8g.tegraflash.tar.gz`

To install, extract then run:
```
sudo ./initrd-flash
```
