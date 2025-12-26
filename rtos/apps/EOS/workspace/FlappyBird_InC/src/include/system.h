
#ifndef _SYSTEM_H_
#define _SYSTEM_H_
#endif

#include "lwip/dhcp.h"
#include "netif/xadapter.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "../platform_config.h"
#include "lwip/sockets.h"

#include <utils.h>

#define THREAD_STACKSIZE 1024
#define HTTP_PORT 80
#define TCP_MAX_CLIENTS 4

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



int main_thread();
void Game_thread();
void Server_thread();
void Game_Session_thread();
void print_echo_app_header();
void echo_application_thread(void *);
void lwip_init();

