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

#define LOG_ERROR  1 /*( application output | error )*/
#define LOG_TRACE  2 /*( application output | error | status updates)*/
#define LOG_DEBUG  3 /*( application output | error | status updates | verbose)*/
#ifndef VERBOSITY
#define VERBOSITY  3 /*( User can establish verbosity level )*/
#endif

#define CLR_RED     "\033[31m"
#define CLR_GREEN   "\033[32m"
#define CLR_YELLOW  "\033[33m"
#define CLR_BLUE    "\033[34m"
#define CLR_MAGENTA "\033[35m"
#define CLR_CYAN    "\033[36m"
#define CLR_RESET   "\033[0m"

typedef  void (*var_printer_t)(va_list args);

void
TRACE(int verbosity,const char* msg, var_printer_t printer, ...);

#endif
