#include "system_utils.h"
/* SD-CARD*/
#include "xsdps.h" /*SD device drivers*/
#include "ff.h"
void system_test(void *sdHandle){
    system_test_sd(sdHandle);
}
int system_test_sd(void *sdHandle){
    LOG_UART(LOG_TRACE,"== SD - TEST ==", NULL);
    if(!sdHandle){
        LOG_UART(LOG_ERROR,"-> no sd handle provided", NULL, 0);
        return 1;
    }


}