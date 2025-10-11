#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <err.h>

//device tree specific
#define DEV_VDMA_ADDR                           0x43000000
#define DEV_VDMA_SIZE                           0x01400000
#define DEV_VDMA_OFFSET                         0x0
#define DEV_VDMA_FSIZE                          0x5EEC00

#define OFFSET_VDMA_MM2S_CONTROL_REGISTER       0x00
#define OFFSET_VDMA_MM2S_STATUS_REGISTER        0x04
#define OFFSET_VDMA_MM2S_VSIZE                  0x50
#define OFFSET_VDMA_MM2S_HSIZE                  0x54
#define OFFSET_VDMA_MM2S_FRMDLY_STRIDE          0x58
#define OFFSET_VDMA_MM2S_FRAMEBUFFER1           0x5c
#define OFFSET_VDMA_MM2S_FRAMEBUFFER2           0x60
#define OFFSET_VDMA_MM2S_FRAMEBUFFER3           0x64
#define OFFSET_VDMA_MM2S_FRAMEBUFFER4           0x68

#define OFFSET_VDMA_S2MM_CONTROL_REGISTER       0x30
#define OFFSET_VDMA_S2MM_STATUS_REGISTER        0x34
#define OFFSET_VDMA_S2MM_IRQ_MASK               0x3c
#define OFFSET_VDMA_S2MM_REG_INDEX              0x44
#define OFFSET_VDMA_S2MM_VSIZE                  0xa0
#define OFFSET_VDMA_S2MM_HSIZE                  0xa4
#define OFFSET_VDMA_S2MM_FRMDLY_STRIDE          0xa8
#define OFFSET_VDMA_S2MM_FRAMEBUFFER1           0xac
#define OFFSET_VDMA_S2MM_FRAMEBUFFER2           0xb0
#define OFFSET_VDMA_S2MM_FRAMEBUFFER3           0xb4
#define OFFSET_VDMA_S2MM_FRAMEBUFFER4           0xb8

#define VDMA_CONTROL_REGISTER_START             0x1
#define VDMA_CONTROL_REGISTER_CIRCULAR_PARK     0x2
#define VDMA_CONTROL_REGISTER_RESET             0x4

#define log_error(str_arg)  \
    do {                    \
        perror("str_arg")   \
        exit(1);            \
    }while(0)

typedef struct{
    unsigned int baseAddr;
    int vdmaHandler;
    int width;
    int height;
    int pixelDepth;
    int fbLenght;
    uintptr_t vdmaVirtAddr; 
    uintptr_t fb1VirtualAddr; 
    uintptr_t fb2VirtualAddr; 
    uintptr_t fb3VirtualAddr; 
    uintptr_t fb1PhysicalAddr; 
    uintptr_t fb2PhysicalAddr; 
    uintptr_t fb3PhysicalAddr; 

}vdma_handle;

int vdma_setup(vdma_handle* handle){
    uint width = 1920, height = 1080, depth = 3;
    handle->baseAddr = DEV_VDMA_ADDR;
    handle->width = width;
    handle->height = height;
    handle->pixelDepth = depth;
    handle->fbLenght = width * height * depth;
    handle->vdmaHandler = open("/dev/mem", O_RDWR | O_SYNC);
    handle->vdmaVirtAddr = (uintptr_t) mmap(NULL, DEV_VDMA_SIZE,
                PROT_READ | PROT_WRITE, MAP_SHARED, handle->vdmaHandler, (off_t) handle->baseAddr);
    if( handle->vdmaVirtAddr == MAP_FAILED )
            write_error("mmap");
}

int 
main(int argc, char* argv[]){
    vdma_handle vh;
    vdma_setup(&vh);

    return 0;
}

