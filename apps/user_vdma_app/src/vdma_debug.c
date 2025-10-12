#include "vdma.h"

/*( for verbose debugging)*/
void DEBUG_phys_virt_bprint(va_list args) {
    const buffer_t *buffer = va_arg(args, const buffer_t *);
    for (int i = 0; i < 3; i++) {
        printf("BufferHandle[%d]:\n", i);
        printf("  PhysicalAddr: 0x%08lX\n", (unsigned long)buffer[i].PhysicalAddr);
        printf("  VirtualAddr : 0x%08lX\n", (unsigned long)buffer[i].VirtualAddr);
        printf("---------------------------------------------\n");
    }
}

void DEBUG_f20_ctrl_regs_vdma(va_list args) {
    const uint32_t *vdma_crp = va_arg(args, const uint32_t *);
    for(int i=0; i<20; i++){
        printf("value:0x%08x at index %d \n",vdma_crp[i],i);
    }
}

void DEBUG_virt_buff_dump_first_20x3(va_list ap) {
    const buffer_t *buffer = va_arg(ap, const buffer_t *);
    for (int fb = 0; fb < 3; fb++) {
        const uint32_t *p = (const uint32_t *)buffer[fb].VirtualAddr;
        printf("\nBufferHandle[%d]:\n", fb);
        for (int i = 0; i < 20; i++) {
            printf("0x%08X ", p[i]);
            if ((i + 1) % 8 == 0) puts("");
        }
        puts("\n---------------------------------------------");
    }
}