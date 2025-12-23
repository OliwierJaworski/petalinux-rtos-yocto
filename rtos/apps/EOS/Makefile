ws = ./workspace
xsa = ../../../platform-build-files/HDMI-VDMA/hdmi_vdma.xsa
plat_name = PYNQ_Z2_ETHERNET_HDMI
domain_name = freertos_domain
app_name = FlappyBird_InC

#https://docs.amd.com/r/2023.1-English/ug1400-vitis-embedded/bsp-config
init:
	xsct -eval " setws $(ws); \
				 platform create -name $(plat_name) -hw $(xsa) -proc ps7_cortexa9_0; \
				 domain create -name $(domain_name) -os freertos10_xilinx -proc ps7_cortexa9_0; \
				 platform active $(plat_name); \
				 domain active $(domain_name); \
				 bsp setlib -name lwip220; \
				 bsp config api_mode SOCKET_API; \
				 bsp write; \
				 platform generate; \
				 app create -name $(app_name) -platform $(plat_name) -domain $(domain_name) -template {FreeRTOS lwIP Echo Server}; \
				 app build $(app_name);"

