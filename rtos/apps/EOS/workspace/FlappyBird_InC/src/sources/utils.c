#include "utils.h"
/*semaphore for exclusive uart control to prevent deadlocks*/
SemaphoreHandle_t xUartSemaphore;

void
LOG_UART(int verbosity,const char* msg, log_func_t printer, ...){

    va_list args;

    if(xUartSemaphore == NULL){
        xUartSemaphore = xSemaphoreCreateBinary();
        xSemaphoreGive(xUartSemaphore);
    }

    if(xUartSemaphore  == NULL)
        exit(1); /* If nomem then deadlocks might happen so better just stop */

    xSemaphoreTake(xUartSemaphore, portMAX_DELAY); 
                                                                      
    switch (verbosity) {
        #if VERBOSITY >= 1
        case LOG_ERROR:
            fprintf(stderr,
                CLR_RED "\n[ERROR]" CLR_RESET "%s, errno: %s\n",msg, strerror(errno));
                if(!errno)
                    exit(1); // in case its a runtime error which cant be found by errno
            break;
        #if VERBOSITY >= 2
        case LOG_TRACE:
            printf(CLR_YELLOW "\n[TRACE]" CLR_RESET "%s\n", msg);
            break;
        #if VERBOSITY >= 3
        case LOG_DEBUG:
            printf(CLR_CYAN "\n[DEBUG]" CLR_RESET "%s\n",msg);
            va_start(args, printer); // not required to call start but we will leave it so for now
            if (printer) 
                printer(args); // pass the va_list to the custom printer
            va_end(args);                 
            
            break;
        default: 
            break;
        #endif
        #endif
        #endif
    }
    xSemaphoreGive(xUartSemaphore);
}

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
    