
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

### 









## used information
- [petalinux tools documentation](https://docs.amd.com/r/en-US/ug1144-petalinux-tools-reference-guide/petalinux-create-Command-Line-Options)
