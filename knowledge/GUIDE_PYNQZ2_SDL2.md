
## instructions
### create a petalinux project 
for pynq z2 no bsp is provided, we will build one from a xsa file.
- petalinux-create project -n PYNQZ2_SDL2 --template zynq --force
this will create the petalinux project and assume a zynq cpu architecture design
- [used libsdl](https://layers.openembedded.org/layerindex/recipe/54064/)
petalinux-config -> Image Packaging Configuration -> Root filesystem type
-> petalinux-config -c rootfs -> Image Features -> debug-tweaks
petalinux-build
[libsdl2 has minor error](https://layers.openembedded.org/layerindex/recipe/5881/)
- in the pathss provided by the bbappend file s should be : S = "${WORKDIR}/SDL2-${PV}" instead of S = "${UNPACKDIR}/SDL2-${PV}"
go 2 dirs back otherways the tar rootfs tarfile wont be found. 
execute -> petalinux-package wic "BOOT.BIN, uImage, boot.scr"
petalinux-package boot --fsbl zynq_fsbl.elf --fpga system.bit --u-boot u-boot.elf
petalinux-package wic -b "BOOT.BIN, uImage, boot.scr"
### adding hardware description using xsa
next xsa for hardware description
- petalinux-config --get-hw-description=${path_to_xsa}/hdmi_vdma.xsa --silentconfig
- dtc -I fs /sys/firmware/devicetree/base
a way to view the device tree used by the kernel
### 

### cross compiling app for platform using host machine
to cross compile the app there are 2 ways 1 i have to test and the other 1 i already tested.
#### 1st way:
- go into your petalinux project folder
- run: petalinux-build --sdk(this will build the sdk you can later find in image/linux/sdk.sh)
    !note: "the build process can take a significant amount of time and resources!"
- run the sdk.sh, it will ask for a path just provide the path where you want to store the rootfs with the crosscompiler.
- once finished you will be prompted with info on how to setup the cross compiler, meaning it will tell you which file to source.
- once you've sourced the env. you can do "echo $CC" it will give you the name of the cross-compilation toolchain.
    info: most likely it will be called: "arm-linux-gnueabihf-gcc" if you are using an arm processor
- a example Makefile can be found in one of the dirs for a "hello-arm example"
- once you've acquired an .elf file(executable) you can scp the file over via ssh to the device :" scp :/filedir host@ip:/filedir "
- and now you can run the program!
#### 2nd way:
not yet done but good guide:[fpgabe: Include Custom C Application in Petalinux (embedded linux) kernel on AMD Zynq Z2 board.](https://www.youtube.com/watch?v=6aZSQN7_94Q) 
or you can follow a hackster tutorial: [Custom Application Creation in PetaLinux on the Zynqberry](https://www.hackster.io/news/custom-application-creation-in-petalinux-on-the-zynqberry-c946ec2f32f5)
from what i saw the easiest method is nr1 due to the fact that you just scp the file to the system and can test it that way or use a maybe use a debugger on the host have to look into that.
method 2 is more cumbersome in my opinion.

### accessing memory mapped io for linux
[The Userspace I/O HOWTO](https://www.kernel.org/doc/html/latest/driver-api/uio-howto.html)
[How could /dev/mem Linux directory be used in order to control the peripherals (MM/IO) ?](https://www.reddit.com/r/embeddedlinux/comments/s45cen/how_could_devmem_linux_directory_be_used_in_order/)



## used information
- [petalinux tools documentation](https://docs.amd.com/r/en-US/ug1144-petalinux-tools-reference-guide/petalinux-create-Command-Line-Options)
