#ifndef __UTILS_H_
#define __UTILS_H_

#include "stdarg.h" // variadic functions
#include "stdio.h"
#include "errno.h"
#include "string.h"
#include "lwip/dhcp.h"
#include "xil_printf.h"

/* for semaphores and TICK timer defines */
#include "FreeRTOS.h"
#include "semphr.h"

#define LOG_ERROR  1 /*( application output | error )*/
#define LOG_TRACE  2 /*( application output | error | status updates)*/
#define LOG_DEBUG  3 /*( application output | error | status updates | verbose)*/
#define VERBOSITY 3

#define CLR_RED     "\033[31m"
#define CLR_GREEN   "\033[32m"
#define CLR_YELLOW  "\033[33m"
#define CLR_BLUE    "\033[34m"
#define CLR_MAGENTA "\033[35m"
#define CLR_CYAN    "\033[36m"
#define CLR_RESET   "\033[0m"

/* Amount of delay in ms to wait | example: UART_MS_DELAY / portTICK_RATE_MS */
#define UART_MS_DELAY 200 

#define LOG_ORIGIN(msg) log_origin(msg, __FILE__, __func__, __LINE__)
static inline const char* log_origin(const char* msg, const char* file, const char* func, unsigned line){
    static char buffer[100];
    snprintf(buffer, sizeof(buffer),
             "%s::%s (line %u) (message: %s)", file, func, line, msg);
    return buffer;
}

typedef  void (*log_func_t)(va_list vArgs);
void
LOG_UART(int verbosity,const char* msg, log_func_t printer, ...);

/* callback function types*/
void LOG_printIPsettings(va_list vargs);
void LOG_test(va_list vargs);

#endif