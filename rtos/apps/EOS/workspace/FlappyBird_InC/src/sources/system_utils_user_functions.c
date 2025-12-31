#include "system_utils.h"
#include "lwip/dhcp.h"

void
LOG_test(va_list vArgs){
    char* msg = va_arg(vArgs, char*);
    xil_printf( msg);
}

/*internal function for LOG_printIpsettings function*/
void
print_ip(char *msg, ip_addr_t *ip)
{
	xil_printf(msg);
	xil_printf("%d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip),
			ip4_addr3(ip), ip4_addr4(ip));
}

void
LOG_printIPsettings(va_list vArgs)
{
    ip_addr_t *ip = va_arg(vArgs, ip_addr_t*);
    ip_addr_t *mask = va_arg(vArgs, ip_addr_t*); 
    ip_addr_t *gw = va_arg(vArgs, ip_addr_t*);
	print_ip("Board IP: ", ip);
	print_ip("Netmask : ", mask);
	print_ip("Gateway : ", gw);
}
