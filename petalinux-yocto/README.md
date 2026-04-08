## GPIO control using embedded linux 

This project demonstrates controlling the onboard LEDs of the PYNQ-Z2 via a custom AXI GPIO design. It includes steps for hardware design, UIO driver configuration in the device tree, and a user-space application that maps the GPIO memory to control LED patterns.
[tutorial](./docs/gpio/Tutorial_Gpio_Control_Embedded_Linux_v3.pdf)
![LED demo](./docs/assets/PYNQLED.gif)

## PYNQ-Z2 Embedded graphics

This project provides a step-by-step guide for setting up a custom PetaLinux image on the PYNQ-Z2 development board and implementing a Video Direct Memory Access (VDMA) pipeline using loadable kernel modules.

The PYNQ-Z2 is a Xilinx Zynq-7000 SoC-based board that combines a dual-core ARM Cortex-A9 processor with programmable FPGA logic, making it well suited for embedded video processing applications. This guide covers the full development flow — from Vivado hardware design and PetaLinux image creation, through to packaging, flashing, and deploying a custom VDMA kernel module on the target hardware.

[tutorial](./docs/vdma/tutorial_pynqz2_embedded_graphics.pdf)