#pragma once

#include "stdarg.h" // variadic functions
#include "string.h"
#include "stdio.h"
#include "xil_io.h"

#define LOG_ERROR  1 /*( application output | error )*/
#define LOG_TRACE  2 /*( application output | error | status updates)*/
#define LOG_DEBUG  3 /*( application output | error | status updates | verbose)*/

#define CLR_RED     "\033[31m"
#define CLR_GREEN   "\033[32m"
#define CLR_YELLOW  "\033[33m"
#define CLR_BLUE    "\033[34m"
#define CLR_MAGENTA "\033[35m"
#define CLR_CYAN    "\033[36m"
#define CLR_RESET   "\033[0m"

#define LOG_ORIGIN(msg) log_origin(msg, __FILE__, __func__, __LINE__)

typedef  void (*log_func_t)(va_list vArgs);

static inline const char* 
log_origin(const char* msg, const char* file, const char* func, unsigned line){
    static char buffer[100];
    snprintf(buffer, sizeof(buffer),
             "%s::%s (line %u) (message: %s)", file, func, line, msg);
    return buffer;
}

void LOG_UART(int verbosity,const char* msg, log_func_t printer, ...);
#define VERBOSITY 3

/* TESTING IF SYSTEM IS FUNCTIONAL
   - located in : system_tests.c
   - keeping args void* -> less dependencies
*/
void system_test_all(void *sdHandle); // function called to test all functionality
int system_test_sd(void *sdHandle);
int system_test_vdma(void *vdmaHandle);
int system_test_tcp(void *tcpHandle);
int system_test_udp(void *udpHandle);

/* USER DEFINED CALLBACKS FOR DEBUGGING
   - located in : source/system_utils_user_functions.c
*/
void LOG_printIPsettings(va_list vargs);
void LOG_test(va_list vargs);