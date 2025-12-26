#include "system.h"

/* DHCP && socket ADAPTER SETUP*/
err_t dhcp_start(struct netif *netif);
volatile int dhcp_timoutcntr;
static struct netif server_netif;
struct netif *echo_netif;
struct TCP_ServerHandle_t serverHandle;

void network_thread(void *p)
{
    struct netif *netif;
    /* the mac address of the board. this should be unique per board */
    unsigned char mac_ethernet_address[] = { 0x02, 0x12, 0x34, 0x56, 0x78, 0x9A };
    ip_addr_t ipaddr, netmask, gw;
    int mscnt = 0;

    netif = &server_netif;

	ipaddr.addr = 0;
	gw.addr = 0;
	netmask.addr = 0;

    /* Add network interface to the netif_list, and set it as default */
    if (!xemac_add(netif, &ipaddr, &netmask, &gw, mac_ethernet_address, PLATFORM_EMAC_BASEADDR)) {
		LOG_UART(LOG_ERROR,LOG_ORIGIN("Error adding N/W interface"), NULL);
	    return;
    }

    netif_set_default(netif);

    /* specify that the network if is up */
    netif_set_up(netif);

    /* start packet receive thread - required for lwIP operation */
    sys_thread_new("xemacif_input_thread", (void(*)(void*))xemacif_input_thread, netif,
            THREAD_STACKSIZE,
            DEFAULT_THREAD_PRIO);

    dhcp_start(netif);
    while (1) {
		vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);
		dhcp_fine_tmr();
		mscnt += DHCP_FINE_TIMER_MSECS;
		if (mscnt >= DHCP_COARSE_TIMER_SECS*1000) {
			dhcp_coarse_tmr();
			mscnt = 0;
		}
	}
    return;
}

int main_thread()
{
    int mscnt = 0;

    lwip_init(); /* initialize lwIP before calling sys_thread_new */

	/* any thread using lwIP should be created using sys_thread_new */
    sys_thread_new("NW_THRD", network_thread, NULL, THREAD_STACKSIZE,
                    DEFAULT_THREAD_PRIO);  

    while (1) {
	vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);/* wait 5 ticks */
		if (server_netif.ip_addr.addr) {
			LOG_UART(LOG_DEBUG, LOG_ORIGIN("-- DHCP SUCCESS : IP SETTINGS --"),LOG_printIPsettings,
						 &(server_netif.ip_addr), &(server_netif.netmask), &(server_netif.gw));
			sys_thread_new("echod", echo_application_thread, 0,
					THREAD_STACKSIZE,
					DEFAULT_THREAD_PRIO);
			break;
		}

		mscnt += DHCP_FINE_TIMER_MSECS;
		if (mscnt >= DHCP_COARSE_TIMER_SECS * 2000) {
			
			IP4_ADDR(&(server_netif.ip_addr), 192, 168, 1, 144);
			IP4_ADDR(&(server_netif.netmask), 255, 255, 255, 0);
			IP4_ADDR(&(server_netif.gw), 192, 168, 1, 1);
			LOG_UART(LOG_DEBUG, LOG_ORIGIN("-- DHCP ERROR : STATIC IP SETTINGS --"), LOG_printIPsettings,
					 &(server_netif.ip_addr), &(server_netif.netmask), &(server_netif.gw));
			sys_thread_new("echod", Server_thread, 0,
						   THREAD_STACKSIZE,
						   DEFAULT_THREAD_PRIO);
			break;
		}
	}
		
	vTaskDelete(NULL);
	return 0;
}


void Server_thread(){
	memset(&serverHandle, 0, sizoef(serverHandle) );	
	
	if ((serverHandle.sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)	{
		LOG_UART(LOG_ERROR, LOG_ORIGIN("-- FAILED TO ASSIGN SOCKET FOR SERVER HANDLE --"), NULL);
		// error logging also exit(1) so no need 
	}
	
	serverHandle.addr.sin_family = AF_INET;
	serverHandle.addr.sin_port = htons(HTTP_PORT);
	serverHandle.addr.sin_addr.s_addr = INADDR_ANY;

	if(lwip_bind(serverHandle.sock, (struct sockaddr*)&serverHandle.addr, sizeof(serverHandle.addr)) < 0)
		LOG_UART(LOG_DEBUG, LOG_ORIGIN("-- FAILED TO BIND SOCKET --"), NULL);
	
	lwip_listen(serverHandle.sock, 0);
	serverHandle.remoteSize = sizeof(serverHandle.remote);
	
	u16 *chIdx = &serverHandle.connIdx;
	while(1){
		if( (serverHandle.ch[*chIdx].sd = lwip_accept(serverHandle.sock, (struct sockaddr *)&serverHandle.remoteSize, (socklen_t*)&serverHandle.remoteSize)) > 0){
			sys_thread_new(("Client Handler"), );
			if(++*chIdx >= TCP_MAX_CLIENTS){
				break;
			}
		} 	
	}
	LOG_UART(LOG_TRACE, LOG_ORIGIN("-- TCP MAX CLIENTS REACHED --"), NULL);
}
