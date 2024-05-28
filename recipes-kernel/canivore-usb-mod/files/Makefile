obj-m := canivore-usb.o
ccflags-y := -O2 -Wall -Wimplicit-fallthrough=3 -Werror
canivore-usb-y := canivore-usb-drv.o canivore-usb-util.o

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
