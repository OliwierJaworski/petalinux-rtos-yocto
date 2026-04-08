obj-m := vdma-overlay.o

MY_CFLAGS += -g -DDEBUG
ccflags-y += ${MY_CFLAGS} 
KBUILD_CFLAGS_MODULE += -O1

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
