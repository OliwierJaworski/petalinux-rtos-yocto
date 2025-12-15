#include "graphics.h"

void DEBUG_recvd_buffer_addr(va_list args){
    const uint32_t * regaddr = va_arg(args, const uint32_t *);
    for (int i = 0; i < 3; i++) {
        printf("transferred pointer to buffer :[%d] = %p\n", i, (void*)regaddr[i]);
    }
}