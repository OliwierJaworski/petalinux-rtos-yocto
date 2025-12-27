
#ifndef _SYSTEM_H_
#define _SYSTEM_H_
#endif

/* TCP */
#include "lwip/dhcp.h"
#include "netif/xadapter.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "../platform_config.h"
#include "lwip/sockets.h"

/* SD-CARD*/
#include "xsdps.h" /*SD device drivers*/
#include "ff.h"
#include "xil_cache.h"

/* LOGGING */
#include <utils.h>

#define THREAD_STACKSIZE 1024
#define HTTP_PORT 80
#define TCP_MAX_CLIENTS 10
#define SD_CONFIG_FILE "config.yaml"
#define SD_INDEX_HTML "index.html"

// https://elm-chan.org/fsw/ff/
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
    struct ClientHandle ch[TCP_MAX_CLIENTS]; //handle only 4 players
    u16    connIdx; // amount of connected clients +1 -> next
};

extern char Index_Http[4000];

void prvSetupHardware();
int main_thread();
void Game_thread();
void Server_thread();
void cRequestHandle_thread(void *p); // thread which handles client requests
void Game_Session_thread();
void print_echo_app_header();
void echo_application_thread(void *);
void lwip_init();

