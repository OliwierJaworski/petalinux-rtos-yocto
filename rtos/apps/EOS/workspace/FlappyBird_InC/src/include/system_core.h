#pragma once
#include "../platform_config.h"
#include "xil_types.h"
/* RTOS */
#include "FreeRTOS.h"
#include "semphr.h"
#include "lwip/dhcp.h"
#include "lwip/sockets.h"
#include "netif/xadapter.h"
//#include "lwip/inet.h"
/* SD-CARD */
#include "xsdps.h" /*SD device drivers*/
#include "ff.h"
/* USER DEFINED */
#include "system_config.h"
#include "system_utils.h"

struct SDhandle_t{
    SemaphoreHandle_t xSDsemphr;
    const TCHAR *volume; /* volume path -> set to 0:/ */
    FATFS fs; /* filesys handle */
    FIL fp; /* File descriptor */
    char* file; /* File name */
    FRESULT r; /* result from file operations */
    UINT read; /* number of bytes read */
    FILINFO Finfo; /* possibly retrieve more info about file */
    DIR dir; /* for custom ls command maybe later*/
};

struct HTTPhandle_t{
    char indexPage[TCP_MAX_HTML_SIZE]; //page served on "/" request
};

struct GRAPHICSHandle_t{
    UINTPTR frame_buffers[3];
};

struct NETHandle_t{
    struct netif netif;
    unsigned char mac_ethernet_address[6];
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
};

struct Umessage_t{ //user message type
    u8 id;
    char cmd[SMALL_MSG_SIZE];
};

struct SOCKHandle_t{
    s16 sock;
    socklen_t remoteSize;
    struct sockaddr_in addr;
    struct sockaddr_in remote;
    SemaphoreHandle_t xClientsemphrCounting;
    int sd;
    char buffer[NET_BUFFER_SIZE]; // 10kb buffer
    QueueSetHandle_t mQueue; // message queue
};

struct SYSHandle_t{
    struct SDhandle_t sd;
    struct HTTPhandle_t http;
    struct GRAPHICSHandle_t graphics;
    struct NETHandle_t net;
    struct SOCKHandle_t tcpServer;
    struct SOCKHandle_t udpServer;
};

void prvSetupHardware(struct SYSHandle_t *sys);
int main_thread(void* arg);
void network_thread(void *arg);
void TCP_server_thread(void *arg);
void UDP_server_thread(void *arg);
int messageParseData(const char* in, struct Umessage_t* m);
//void cReqHandle_TCP(void* arg); //not important for right now



/* MM2S VDMA CONTROL REGISTER */
#define VDMA_CONTROL_WRITE(cmd) Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + VDMA_CTRL_REG, (cmd))
#define VDMA_CONTROL_READ() Xil_In32(XPAR_AXI_VDMA_0_BASEADDR + VDMA_CTRL_REG);

/* MM2S VDMA STATUS REGISTER */
#define VDMA_STATUS_READ() Xil_In32(XPAR_AXI_VDMA_0_BASEADDR + VDMA_STATUS_REG)

/* MM2S VDMA BUFFER REGISTER POINTERS */
#define VDMA_FB1_START_ADDR_WRITE(Bptr) Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + VDMA_FB1_REG, (Bptr))
#define VDMA_FB2_START_ADDR_WRITE(Bptr) Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + VDMA_FB2_REG, (Bptr))
#define VDMA_FB3_START_ADDR_WRITE(Bptr) Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + VDMA_FB3_REG, (Bptr))

/* MM2S VDMA FRAME AND STRIDE REGISTER */
#define VDMA_FRAME_DELAY_STRIDE_WRITE() Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + VDMA_FRAME_STRIDE_REG, (HDMI_HSIZE * HDMI_RGB))

/* MM2S VDMA VERTICAL SIZE */
#define VDMA_FRAME_VSIZE_WRITE() Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + VDMA_VSIZE_REG, HDMI_VSIZE)

/* MM2S VDMA HORIZONTAL SIZE */
#define VDMA_FRAME_HSIZE_WRITE() Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + VDMA_HSIZE_REG, (HDMI_HSIZE * HDMI_RGB))
