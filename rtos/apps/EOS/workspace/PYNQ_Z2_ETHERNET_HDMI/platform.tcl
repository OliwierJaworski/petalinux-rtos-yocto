platform generate -domains 
platform generate -domains 
platform generate
platform generate
platform generate
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform active {PYNQ_Z2_ETHERNET_HDMI}
domain active {zynq_fsbl}
bsp reload
domain active {freertos_domain}
bsp reload
bsp config lwip_dhcp "true"
bsp write
bsp reload
catch {bsp regenerate}
platform generate -domains freertos_domain 
