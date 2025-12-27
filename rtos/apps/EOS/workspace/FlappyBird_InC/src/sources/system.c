#include "system.h"

/* DHCP && socket ADAPTER SETUP*/
err_t dhcp_start(struct netif *netif);
volatile int dhcp_timoutcntr;
static struct netif server_netif;
struct netif *echo_netif;
struct TCP_ServerHandle_t serverHandle;
struct SDhandle SD = {.path = SD_DEFAULT_VOLUME};
TaskHandle_t xGameHandle = NULL;
BaseType_t xReturned;
char indexHtml[4000];
UINTPTR frame_buffers[3] = {FRAME_BUFFER_PTR1, FRAME_BUFFER_PTR2, FRAME_BUFFER_PTR3};

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

	xReturned = xTaskCreate(Game_thread,"Game_Thread", THREAD_STACKSIZE, NULL, 
					DEFAULT_THREAD_PRIO, &xGameHandle);
	if(xReturned != pdPASS)
		LOG_UART(LOG_ERROR,LOG_ORIGIN("TASK CREATION FAILED"), NULL);

    while (1) {
	vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);/* wait 5 ticks */
		if (server_netif.ip_addr.addr) {
			LOG_UART(LOG_DEBUG, LOG_ORIGIN("-- DHCP SUCCESS : IP SETTINGS --"),LOG_printIPsettings,
						 &(server_netif.ip_addr), &(server_netif.netmask), &(server_netif.gw));
			sys_thread_new("Server_Thread", Server_thread, 0,
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
			sys_thread_new("Server Thread", Server_thread, 0,
						   THREAD_STACKSIZE,
						   DEFAULT_THREAD_PRIO);
			break;
		}
	}

	vTaskDelete(NULL);
	return 0;
}

UG_WINDOW MainCtx;
UG_BMP FbirdSprite;
UG_GUI PYNQ_GUI;
static void vdmaPxlSet(UG_S16 x, UG_S16 y, UG_COLOR c){
	/* Vdma uses RGB888-type pixel data, This function calculates current pixel offset
	   fragments the u32 value using ptr arithmetic into u8* indexes; r,g,b,(a not used here),
	   and assigns the matching values by shifting the u32 by color offset.
	*/
	u32 offset = ((y * HDMI_HSIZE + x) * HDMI_RGB); //calculate pixel offset	
	u8* fb = (u8*)frame_buffers[0];

	fb[offset + 0] = (c >> 8) & 0xFF; 	//B
	fb[offset + 1] = (c >> 0) & 0xFF;	//G
	fb[offset + 2] = (c >> 16) & 0xFF; 	//R
	
	/* increment framebuffer so the next frame is written to the next
	   in a circular pattern
	*/
/*	if(offset == ( (HDMI_HSIZE*HDMI_VSIZE-3)* HDMI_RGB)){
		if(++fb_idx == 3)
			fb_idx =0;
	}*/	
}

TickType_t xLastwake;
const TickType_t xFrequency = pdMS_TO_TICKS(1000);

void 
Game_thread(void *pvParams){
	UG_Init(&PYNQ_GUI, vdmaPxlSet, HDMI_HSIZE, HDMI_VSIZE);

	SD.r = f_mount(&SD.fs,SD.path,0);
	if(SD.r != FR_OK)
		LOG_UART(LOG_ERROR, LOG_ORIGIN("SD MOUNT FAILED"), NULL);

	f_unmount(SD.path);

	Xil_DCacheDisable();
    UG_FillScreen(C_BEIGE);
    Xil_DCacheEnable();

    /* square properties */
    const int sz = 80;
    int x = (HDMI_HSIZE/2) - (sz/2);
    int y = (HDMI_VSIZE/2) - (sz/2);
    int vx = 6;
    int vy = 6;

    TickType_t xLastWake = xTaskGetTickCount();
    const TickType_t period = pdMS_TO_TICKS(100);   /* ~60 FPS */

    while (1)
    {
        xTaskDelayUntil(&xLastWake, period);

        /* erase previous square */
        Xil_DCacheDisable();
        UG_FillFrame(x, y, x+sz, y+sz, C_BEIGE);

        /* update position */
        x += vx;
        y += vy;

        if (x <= 0 || x + sz >= HDMI_HSIZE)
            vx = -vx;

        if (y <= 0 || y + sz >= HDMI_VSIZE)
            vy = -vy;

        /* draw new square */
        UG_FillFrame(x, y, x+sz, y+sz, C_BLACK);
        Xil_DCacheEnable();
    }
}

void 
Server_thread(){
	memset(&serverHandle, 0, sizeof(serverHandle) );	
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
		if( (serverHandle.ch[*chIdx].sd = 
				lwip_accept(serverHandle.sock, 
					(struct sockaddr *)&serverHandle.remote, (socklen_t*)&serverHandle.remoteSize)) > 0){

			LOG_UART(LOG_TRACE, "-- NEW CLIENT ADDED --", NULL);

			sys_thread_new(("Client_Handler"), cRequestHandle_thread,
				(void*)&(serverHandle.ch[*chIdx].sd),
				THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);

			if(++*chIdx >= TCP_MAX_CLIENTS){
				break;
			}
		} 	
	}
	LOG_UART(LOG_TRACE, LOG_ORIGIN("-- TCP MAX CLIENTS REACHED --"), NULL);
	vTaskSuspend(NULL);
}

void cRequestHandle_thread(void *p)
{
    int sd = *(int*)p;
    char recv_buf[2048];
    int n;

    /* Read (and ignore) the HTTP request */
    n = read(sd, recv_buf, sizeof(recv_buf));
    if (n <= 0) {
        close(sd);
        vTaskDelete(NULL);
    }
	int body_len = sizeof(indexHtml)-1; 
	char header[256];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n",
        body_len);

    /* send header */
    if (header_len > 0)
        write(sd, header, header_len);

    /* send body */
    write(sd, indexHtml, body_len);

    /* close connection */
    close(sd);
    vTaskDelete(NULL);
}

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

// Frame buffer addresses
void prvSetupHardware(){
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
		LOG_UART(LOG_ERROR, ("VDMA INTERNAL ERROR OCCURED"), NULL);	

	/* VDMA DISPLAY TEST */
	Xil_DCacheDisable(); // Disable cache for direct DDR access
    for (int fb = 0; fb < 3; fb++) {
        u8* addr8 = (u8*)frame_buffers[fb];
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                addr8[0] = 0xFF;
                addr8[1] = 0xFF;
                addr8[2] = 0xFF;
                addr8 += 3;
            }
        }
    }
    Xil_DCacheEnable();

	/* READ index.html INTO MEMORY*/
 	SD.r = f_mount(&SD.fs, SD.path, 0); /* mount the root directory */	
	if(SD.r != FR_OK)
		LOG_UART(LOG_ERROR, LOG_ORIGIN("SD MOUNT FAILED"), NULL);

	SD.fname = SD_INDEX_HTML; 
	SD.r = f_open(&SD.fp, SD.fname, FA_READ);
	if(SD.r != FR_OK)
		LOG_UART(LOG_ERROR, LOG_ORIGIN("SD FAILED TO OPEN FILE"), NULL);
	do{
		SD.r = f_read(&SD.fp, (void*)&indexHtml[SD.bRead], f_size(&SD.fp), &SD.bRead);
	}while(SD.bRead < f_size(&SD.fp));
	indexHtml[SD.bRead] = '\0';
	if (SD.r != FR_OK)
		LOG_UART(LOG_ERROR, LOG_ORIGIN("SD FAILED TO READ FROM FILE"), NULL);
	f_unmount(SD.path);	
}