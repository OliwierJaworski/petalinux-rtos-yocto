#ifndef UTILS_H
#define UTILS_H

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <err.h>
#include <unistd.h> 
#include "graphics.h"
#include "SDL2/SDL.h"
#include "string.h"

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

#define LOG_ORIGIN(msg) \
    ({ \
        static char buffer[100]; \
        snprintf(buffer, sizeof(buffer), "%s::%s (line %u) (message: %s)", \
                 __FILE__, __func__, __LINE__, msg); \
        buffer; \
    })

typedef  void (*var_printer_t)(va_list args);

void
TRACE(int verbosity,const char* msg, var_printer_t printer, ...);

#endif
