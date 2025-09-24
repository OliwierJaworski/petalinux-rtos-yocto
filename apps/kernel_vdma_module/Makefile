# path to the petalinux kernel source tree
KERNEL_DIR ?= $(KERNEL_src)
# covers armv4–armv7
ARCH ?= arm 
CROSS_COMPILE ?= arm-xilinx-linux-gnueabi-

my-driver-objs := hello_world_mod.c
my-driver-objs +=

obj-m += hello_world_mod.o

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) -C $(KERNEL_DIR) modules

clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) -C $(KERNEL_DIR) clean

# This does a normal installation...
# You could replace this by a scp command that sends the module to
# your ARM system.
#install:
#	make INSTALL_MOD_PATH=$(INSTALL_MOD_PATH) INSTALL_PATH=$(INSTALL_PATH) INSTALL_HDR_PATH=$(INSTALL_HDR_PATH) M=$(PWD) -C $(MYY_KERNEL_DIR) modules_install
#	scp my-driver.ko 10.100.0.55:/tmp
