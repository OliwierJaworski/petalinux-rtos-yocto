#include "system_core.h"

void 
prvSetupHardware(struct SYSHandle_t *sys){
	/* vdma setup */
	u32 r;
	VDMA_CONTROL_WRITE(VDMA_CTRL_RESET);
	do{
		r =VDMA_CONTROL_READ();
	}while((r & VDMA_CTRL_RESET));

	VDMA_CONTROL_WRITE(VDMA_CTRL_START);	
	VDMA_FB1_START_ADDR_WRITE(FRAME_BUFFER_PTR1);	
	VDMA_FB2_START_ADDR_WRITE(FRAME_BUFFER_PTR1);	
	VDMA_FB3_START_ADDR_WRITE(FRAME_BUFFER_PTR1);	
	VDMA_FRAME_DELAY_STRIDE_WRITE();
	VDMA_FRAME_HSIZE_WRITE(); 
	VDMA_FRAME_VSIZE_WRITE(); //write vsize last due to possible latch arming
	
	r = VDMA_STATUS_READ();
	if(r & (VDMA_STATUS_ERR | VDMA_STATUS_DECERR | VDMA_STATUS_SLVERR) )
		LOG_UART(LOG_ERROR, ("VDMA INTERNAL ERROR OCCURED"), NULL, r);	

    
    struct GRAPHICSHandle_t *graphics = &sys->graphics;
	/* VDMA DISPLAY TEST */
	Xil_DCacheDisable(); // Disable cache for direct DDR access
    for (int fb = 0; fb < 3; fb++) {
        u8* addr8 = (u8*)graphics->frame_buffers[fb];
        for (int y = 0; y < HDMI_VSIZE; y++) {
            for (int x = 0; x < HDMI_HSIZE; x++) {
                addr8[0] = 0xFF;
                addr8[1] = 0xFF;
                addr8[2] = 0xFF;
                addr8 += 3;
            }
        }
    }
    Xil_DCacheEnable();
    struct SDhandle_t *SD = &sys->sd;
    struct HTTPhandle_t *http = &sys->http;
	/* READ index.html INTO MEMORY*/
 	SD->r = f_mount(&SD->fs, SD->volume, 0); /* mount the root directory */	
	if(SD->r != FR_OK)
		LOG_UART(LOG_ERROR, LOG_ORIGIN("SD MOUNT FAILED"), NULL, 0);

	SD->file = SD_INDEX_HTML; 
	SD->r = f_open(&SD->fp, SD->file, FA_READ);
	if(SD->r != FR_OK)
		LOG_UART(LOG_ERROR, LOG_ORIGIN("SD FAILED TO OPEN FILE"), NULL, 0);
	do{
		SD->r = f_read(&SD->fp, (void*)&http->indexPage[SD->read], f_size(&SD->fp), &SD->read);
	}while(SD->read < f_size(&SD->fp));
	http->indexPage[SD->read] = '\0';
	if (SD->r != FR_OK)
		LOG_UART(LOG_ERROR, LOG_ORIGIN("SD FAILED TO READ FROM FILE"), NULL, 0);
	f_unmount(SD->volume);	
}

int 
main_thread(void* arg){
    struct SYSHandle_t *sys = (struct SYSHandle_t*) arg;
    struct NETHandle_t *net = &sys->net;
    int mscnt = 0;

    lwip_init(); /* initialize lwIP before calling sys_thread_new */

    sys_thread_new("NW_THRD", network_thread, (void*)sys, TCP_THREAD_STACKSIZE,
                    DEFAULT_THREAD_PRIO);

    while (1) {
        // wait for free resource
        
        vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);/* wait 5 ticks */
        if (net->netif.ip_addr.addr) {
			LOG_UART(LOG_DEBUG, LOG_ORIGIN("-- DHCP SUCCESS : IP SETTINGS --"),LOG_printIPsettings,
						 &(net->netif.ip_addr), &(net->netif.netmask), &(net->netif.gw));
			sys_thread_new("TCP_Server_Thread", TCP_server_thread, (void*)sys,
					TCP_THREAD_STACKSIZE,
					DEFAULT_THREAD_PRIO);
			sys_thread_new("UDP_Server_Thread", UDP_server_thread, (void*)sys,
					UDP_THREAD_STACKSIZE,
					DEFAULT_THREAD_PRIO);
			break;
		}

        mscnt += DHCP_FINE_TIMER_MSECS;
		if (mscnt >= DHCP_COARSE_TIMER_SECS * 2000) {
			
			IP4_ADDR(&(net->netif.ip_addr), 192, 168, 1, 144);
			IP4_ADDR(&(net->netif.netmask), 255, 255, 255, 0);
			IP4_ADDR(&(net->netif.gw), 192, 168, 1, 1);
			LOG_UART(LOG_DEBUG, LOG_ORIGIN("-- DHCP ERROR : STATIC IP SETTINGS --"), LOG_printIPsettings,
					 &(net->netif.ip_addr), &(net->netif.netmask), &(net->netif.gw));
			sys_thread_new("TCP_Server Thread", TCP_server_thread, 0,
						   TCP_THREAD_STACKSIZE,
						   DEFAULT_THREAD_PRIO);
			sys_thread_new("UDP_Server_Thread", UDP_server_thread, (void*)sys,
					UDP_THREAD_STACKSIZE,
					DEFAULT_THREAD_PRIO);
			break;
		}
    }
    vTaskDelete(NULL);
    return 0;
}

void 
network_thread(void *arg){
    struct SYSHandle_t *sys = (struct SYSHandle_t*) arg;
    struct NETHandle_t *net = &sys->net;
    int mscnt = 0;

    memcpy(net->mac_ethernet_address,(unsigned char[]){0x02,0x12,0x34,0x56,0x78,0x9A},6);
    net->ipaddr.addr = 0;
	net->gw.addr = 0;
	net->netmask.addr = 0;

    /* Add network interface to the netif_list, and set it as default */
    if (!xemac_add(&net->netif, &net->ipaddr, &net->netmask, &net->gw, net->mac_ethernet_address, PLATFORM_EMAC_BASEADDR)) {
		LOG_UART(LOG_ERROR,LOG_ORIGIN("Error adding N/W interface"), NULL, 0);
	    return;
    }

    netif_set_default(&net->netif);
    netif_set_up(&net->netif);

    sys_thread_new("xemacif_input_thread", (void(*)(void*))xemacif_input_thread, &net->netif,
            TCP_THREAD_STACKSIZE,
            DEFAULT_THREAD_PRIO);

    dhcp_start(&net->netif);

    while (1) {
		vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);
		dhcp_fine_tmr();
		mscnt += DHCP_FINE_TIMER_MSECS;
		if (mscnt >= DHCP_COARSE_TIMER_SECS*1000) {
			dhcp_coarse_tmr();
			mscnt = 0;
		}
	}
    vTaskDelete(NULL);
}

void 
TCP_server_thread(void *arg){
    struct SYSHandle_t *sys = (struct SYSHandle_t*) arg;
    struct SOCKHandle_t *tcp = &sys->tcpServer;

    if ((tcp->sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)	{
        LOG_UART(LOG_ERROR, LOG_ORIGIN("-- FAILED TO ASSIGN SOCKET FOR SERVER HANDLE --"), NULL, 0);
    }
    
    tcp->addr.sin_family = AF_INET;
	tcp->addr.sin_port = htons(TCP_HTTP_PORT);
	tcp->addr.sin_addr.s_addr = INADDR_ANY;

    if(lwip_bind(tcp->sock, (struct sockaddr*)&tcp->addr, sizeof(tcp->addr)) < 0)
		LOG_UART(LOG_DEBUG, LOG_ORIGIN("-- FAILED TO BIND SOCKET --"), NULL);

    lwip_listen(tcp->sock, 0);

    if( (tcp->sd = lwip_accept(tcp->sock, 
						(struct sockaddr *)&tcp->remote, (socklen_t*)&tcp->remoteSize)) > 0){

				LOG_UART(LOG_TRACE, "-- NEW CLIENT ADDED --", NULL);
				// sys_thread_new(("Client_Handler"), cRequestHandle_thread, (void*)&(serverHandle.sd), THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
	}
    vTaskDelete(NULL);
}

void 
UDP_server_thread(void *arg){
    struct SYSHandle_t *sys = (struct SYSHandle_t*) arg;
    struct SOCKHandle_t *udp = &sys->udpServer;
	s32_t recv_id = 0;
	char* data = "hello from udp server!";
    if ((udp->sock = lwip_socket(AF_INET, SOCK_DGRAM, 0)) < 0)	{
        LOG_UART(LOG_ERROR, LOG_ORIGIN("-- FAILED TO ASSIGN SOCKET FOR SERVER HANDLE --"), NULL, 0);
    }
    
    udp->addr.sin_family = AF_INET;
	udp->addr.sin_port = htons(UDP_PORT);
	udp->addr.sin_addr.s_addr = INADDR_ANY;
	udp->remoteSize = sizeof(udp->remote);

    if(lwip_bind(udp->sock, (struct sockaddr*)&udp->addr, sizeof(udp->addr)) < 0)
		LOG_UART(LOG_DEBUG, LOG_ORIGIN("-- FAILED TO BIND SOCKET --"), NULL);

	while(1){
		if(lwip_recvfrom(udp->sock, udp->buffer, sizeof(udp->buffer), 0, (struct sockaddr*)&udp->remote, (socklen_t*)&udp->remoteSize) <=0){
			LOG_UART(LOG_ERROR,LOG_ORIGIN("UDP recv something went wrong"),NULL,0);
			continue;
		}
		LOG_UART(LOG_TRACE, "-- NEW UDP DATA RECEIVED --", NULL);
		/* in case the user wants ordered datagrams it must be implemented separately.
			- meaning: send a message and prepend the message number eg
			- recv_id = ntohl(*((int*)udp->buffer));	// check whether datagram is recvd in order
			- not implemented here
		*/

		if(lwip_sendto(udp->sock, data, strlen(data), 0, (struct sockaddr*)&udp->remote,udp->remoteSize ) < 0){
			LOG_UART(LOG_ERROR,"FAILED TO SEND MESSAGE OVER UDP", NULL, 0);	
		}
	}
	
    vTaskDelete(NULL);
}

