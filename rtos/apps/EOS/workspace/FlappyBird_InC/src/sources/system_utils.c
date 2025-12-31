#include "system_utils.h"
#include "FreeRTOS.h"
#include "semphr.h"

/*semaphore for resource exclusion*/
static SemaphoreHandle_t xUartSemaphore;

void
LOG_UART(int verbosity,const char* msg, log_func_t printer, ...){

    va_list args;

    if(xUartSemaphore == NULL){
        xUartSemaphore = xSemaphoreCreateBinary();
        xSemaphoreGive(xUartSemaphore);
    }

    if(xUartSemaphore  == NULL)
        return; /* If nomem then deadlocks might happen so better just stop */

    xSemaphoreTake(xUartSemaphore, portMAX_DELAY); 
                                                                      
    switch (verbosity) {
        #if VERBOSITY >= 1
        case LOG_ERROR:
            va_start(args, printer);
            int err = va_arg(args, int);
            fprintf(stderr,
                CLR_RED "\n[ERROR]" CLR_RESET "%s, error code: %d\n",msg, err);
            va_end(args); 
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