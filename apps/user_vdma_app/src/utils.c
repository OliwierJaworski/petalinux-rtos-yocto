#include "utils.h"

void
TRACE(int verbosity,const char* msg, var_printer_t printer, ...){

    va_list args;
    va_start(args, printer);

    switch (verbosity) {
        #if VERBOSITY >= 1
        case LOG_ERROR:
            fprintf(stderr,
                CLR_RED "[ERROR]" CLR_RESET " %s::%s (line %u)\n"
                "  cause: %s (%s)\n",
                __FILE__, __func__, __LINE__,
                msg, strerror(errno));
            break;
        #if VERBOSITY >= 2
        case LOG_TRACE:
            printf(CLR_YELLOW "[TRACE]" CLR_RESET " %s::%s (line %u) — %s\n",
               __FILE__, __func__, __LINE__, msg);
            break;
        #if VERBOSITY >= 3
        case LOG_DEBUG:
            printf(CLR_CYAN "[DEBUG]" CLR_RESET " %s::%s (line %u) (message: %s)\n",
                __FILE__, __func__, __LINE__, msg);
            if (printer) {
                printer(args);   // pass the va_list to the custom printer
            }
            break;
        default: 
            break;
        #endif
        #endif
        #endif
    }
}

