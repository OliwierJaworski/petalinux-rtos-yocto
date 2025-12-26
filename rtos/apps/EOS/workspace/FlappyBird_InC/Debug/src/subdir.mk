################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/iic_phyreset.c \
../src/main.c 

OBJS += \
./src/iic_phyreset.o \
./src/main.o 

C_DEPS += \
./src/iic_phyreset.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -DLWIP_DHCP=1 -Wall -O0 -g3 -IM:/Github/petalinux-rtos-yocto/rtos/apps/EOS/workspace/PYNQ_Z2_ETHERNET_HDMI/export/PYNQ_Z2_ETHERNET_HDMI/sw/PYNQ_Z2_ETHERNET_HDMI/freertos_domain/bspinclude/include -I"M:\Github\petalinux-rtos-yocto\rtos\apps\EOS\workspace\FlappyBird_InC\src\include" -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


