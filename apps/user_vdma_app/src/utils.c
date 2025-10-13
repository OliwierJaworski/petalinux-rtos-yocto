#include "utils.h"

void
TRACE(int verbosity,const char* msg, var_printer_t printer, ...){

    va_list args;
    va_start(args, printer);

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

