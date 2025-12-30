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
bsp config use_lfn "3"
bsp write
bsp reload
catch {bsp regenerate}
platform generate -domains freertos_domain 
platform active {PYNQ_Z2_ETHERNET_HDMI}
platform generate -domains freertos_domain,zynq_fsbl 
platform active {PYNQ_Z2_ETHERNET_HDMI}
bsp reload
bsp reload
domain active {zynq_fsbl}
bsp reload
bsp reload
platform generate
platform active {PYNQ_Z2_ETHERNET_HDMI}
domain active {freertos_domain}
bsp reload
bsp reload
platform generate -domains freertos_domain,zynq_fsbl 
platform clean
platform generate
platform clean
platform generate
platform active {PYNQ_Z2_ETHERNET_HDMI}
platform generate -domains 
platform clean
platform generate
platform clean
platform generate
platform clean
platform clean
platform generate
platform clean
platform generate
platform active {PYNQ_Z2_ETHERNET_HDMI}
bsp reload
bsp reload
platform active {PYNQ_Z2_ETHERNET_HDMI}
platform generate
platform clean
platform generate
