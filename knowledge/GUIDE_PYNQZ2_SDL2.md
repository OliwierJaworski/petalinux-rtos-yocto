
## instructions
### create a petalinux project 
for pynq z2 no bsp is provided, we will build one from a xsa file.
- petalinux-create project -n PYNQZ2_SDL2 --template zynq --force
this will create the petalinux project and assume a zynq cpu architecture design

### adding hardware description using xsa
next xsa for hardware description
- petalinux-config --get-hw-description=${path_to_xsa}/hdmi_vdma.xsa --silentconfig

### 









## used information
- [petalinux tools documentation](https://docs.amd.com/r/en-US/ug1144-petalinux-tools-reference-guide/petalinux-create-Command-Line-Options)
