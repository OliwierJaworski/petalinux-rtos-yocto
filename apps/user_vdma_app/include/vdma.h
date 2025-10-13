#ifndef VDMA_H
#define VDMA_H

#include "utils.h"

/*( physical vdma properties )*/
#define DEV_VDMA_ADDR                           0x43000000
#define DEV_VDMA_SIZE                           0x10000
#define DEV_VDMA_OFFSET                         0x0

/*( physical buffer properties )*/
#define BUFFER_ADDR                             0x10000000
#define BUFFER_SIZE                             0x01400000
#define BUFFER_FSIZE                            0x5EEC00 //1920x1080x3
#define BUFFER_OFFSET                           0x0 

/*( vdma used control registers )*/
#define OFFSET_VDMA_MM2S_CONTROL_REGISTER       0x00
#define OFFSET_VDMA_MM2S_STATUS_REGISTER        0x04
#define OFFSET_VDMA_MM2S_VSIZE                  0x50
#define OFFSET_VDMA_MM2S_HSIZE                  0x54
#define OFFSET_VDMA_MM2S_FRMDLY_STRIDE          0x58
#define OFFSET_VDMA_MM2S_FRAMEBUFFER1           0x5c
#define OFFSET_VDMA_MM2S_FRAMEBUFFER2           0x60
#define OFFSET_VDMA_MM2S_FRAMEBUFFER3           0x64
#define OFFSET_VDMA_MM2S_FRAMEBUFFER4           0x68

/*( start | reset of vdma)*/
#define VDMA_CONTROL_REGISTER_START             0x3
#define VDMA_CONTROL_REGISTER_CIRCULAR_PARK     0x2
#define VDMA_CONTROL_REGISTER_RESET             0x4
#define VDMA_CONTROL_STATE_STOP                 0x0

typedef struct{
    uintptr_t VirtualAddr;
    uintptr_t PhysicalAddr; 
}buffer_t;

typedef struct{
    int baseAddr;
    int vdmafp;
    int bufferfp;
    int width;
    int height;
    int pixelDepth;
    int fbLenght;
    buffer_t bufferHandle[3];
    uintptr_t vdma_crp;
}vdma_handle;

int vdma_setup(vdma_handle* handle);
int vdma_remove(vdma_handle* handle);
int vdma_regcheck(vdma_handle* handle);

/*( for verbose debugging)*/
void DEBUG_phys_virt_bprint(va_list args);
void DEBUG_f20_ctrl_regs_vdma(va_list args);
void DEBUG_virt_buff_dump_first_20x3(va_list args);

#endif