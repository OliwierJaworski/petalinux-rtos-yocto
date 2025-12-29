
#ifndef _SYSTEM_H_
#define _SYSTEM_H_

/* TCP */
#include "lwip/dhcp.h"
#include "netif/xadapter.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "../platform_config.h"
#include "lwip/sockets.h"
#include "lwip/tcpip.h"

/* SD-CARD*/
#include "xsdps.h" /*SD device drivers*/
#include "ff.h"
#include "xil_cache.h"
/* LOGGING && CONFIG*/
#include "sysconf.h"
#include <utils.h>
/* VDMA */
#include "xil_cache.h"
#include "../UGUI/ugui.h"

// fops info -> https://elm-chan.org/fsw/ff/
struct SDhandle{
    SemaphoreHandle_t xSDsemaphore;
    FATFS fs; /* filesys handle */
    FIL fp; /* File descriptor */
    char* fname; /* File name */
    FRESULT r; /* result from file operations */
    UINT bRead; /* number of bytes read */
    FILINFO Finfo; /* possibly retrieve more info about file */
    const TCHAR *path; /* volume path -> set to 0:/ */
    DIR dir; /* for custom ls command maybe later*/
};

struct ClientHandle{
   int sd; 
};

struct TCP_ServerHandle_t{
    s16 sock;
    s16 remoteSize;
    struct sockaddr_in addr;
    struct sockaddr_in remote;
    struct ClientHandle ch[TCP_MAX_CLIENTS]; 
    u16    connIdx; // connected clients +1 
};

static enum HTTP_METHOD{
    REQ_GET,
    REQ_POST,
    REQ_PUT,
    REQ_UNKNOWN
};

struct HttpQuery{
    char key[50]; 
    char value[100];
    struct HttpQuery *next;
    struct HttpHeader *prev;
};

struct HttpHeader{
    enum HTTP_METHOD ReqType;
    char ReqDir [100]; // /, /logs, etc...
    char ReqHost [100]; // 102.168...
    char ReqStatus[50]; // keep-alive ,close, ...
    char ReqData [10240]; // max 10kb of data
    size_t datasize;
};

struct HttpRequest_t{
    struct HttpHeader header;
    struct HttpQuery queries;
    size_t QueryAmount;
    char data[10240];
};

extern char Index_Http[4000];

void prvSetupHardware();
int main_thread();
void Game_thread(void *pvParams);
void Server_thread();
void cRequestHandle_thread(void *p); // thread which handles client requests
void Game_Session_thread();
void print_echo_app_header();
void echo_application_thread(void *);

/* HTTP requests */
void ProcessRequest(const char* HttpReq, struct HttpRequest_t *r);

#define VDMA_CONTROL_READ() Xil_In32(XPAR_AXI_VDMA_0_BASEADDR + VDMA_CTRL_REG);
/* MM2S VDMA CONTROL REGISTER */
#define VDMA_CONTROL_WRITE(cmd) Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + VDMA_CTRL_REG, (cmd))

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

#endif