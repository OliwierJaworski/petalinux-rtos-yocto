#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <err.h>
#include <unistd.h> 
#include "graphics.h"

//device tree specific
#define DEV_VDMA_ADDR                           0x43000000
#define DEV_VDMA_SIZE                           0x10000
#define DEV_VDMA_OFFSET                         0x0

#define BUFFER_ADDR                             0x10000000
#define BUFFER_SIZE                             0x01400000
#define BUFFER_FSIZE                            0x5EEC00 //1920x1080x3
#define BUFFER_OFFSET                           0x0 

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
        fprintf(stderr,"Error in: %s, %s\n", str_arg, strerror(errno));\
        exit(1);            \
    }while(0)

typedef struct{
    uintptr_t VirtualAddr;
    uintptr_t PhysicalAddr; 
}buffer_t;

typedef struct{
    unsigned int baseAddr;
    int vdmafp;
    int bufferfp;
    int width;
    int height;
    int pixelDepth;
    int fbLenght;
    buffer_t bufferHandle[3];
    uintptr_t vdma_crp;
}vdma_handle;

int 
vdma_setup(vdma_handle* handle){
    uint32_t width = 1920, height = 1080, depth = 3;
    int pg_offset=0;

    handle->baseAddr = DEV_VDMA_ADDR;
    handle->bufferHandle->PhysicalAddr = BUFFER_ADDR;
    handle->width = width;
    handle->height = height;
    handle->pixelDepth = depth;
    handle->fbLenght = width * height * depth;

    handle->vdmafp = open("/dev/mem", O_RDWR | O_SYNC);
    if( handle->vdmafp == -1 )
        log_error("open");
    
    handle->vdma_crp= (uintptr_t) mmap(NULL, DEV_VDMA_SIZE + pg_offset,
            PROT_READ | PROT_WRITE, MAP_SHARED, handle->vdmafp, (off_t) handle->baseAddr);
    if( (void*) handle->vdma_crp == MAP_FAILED )
        log_error("mmap1");
    
    handle->bufferfp = open("/dev/uio3", O_RDWR | O_SYNC);
    if( handle->bufferfp== -1 )
        log_error("open");

    handle->bufferHandle->VirtualAddr = (uintptr_t) mmap(NULL, BUFFER_SIZE ,
            PROT_READ | PROT_WRITE, MAP_SHARED, handle->bufferfp,
            (off_t)BUFFER_OFFSET);
    if( (void*) handle->bufferHandle->VirtualAddr == MAP_FAILED )
        log_error("mmap2");

    for(int i=1; i < 3; i++ ){
        handle->bufferHandle[i].PhysicalAddr= handle->bufferHandle->PhysicalAddr + (i * BUFFER_FSIZE);
        handle->bufferHandle[i].VirtualAddr = handle->bufferHandle->VirtualAddr + (i * BUFFER_FSIZE);
    }
    for (int i = 0; i < 3; i++) {
        printf("BufferHandle[%d]:\n", i);
        printf("  PhysicalAddr: 0x%08lX\n", (unsigned long)handle->bufferHandle[i].PhysicalAddr);
        printf("  VirtualAddr : 0x%08lX\n", (unsigned long)handle->bufferHandle[i].VirtualAddr);
        printf("---------------------------------------------\n");
    }

    u_int32_t* vcrp = ((u_int32_t*)handle->vdma_crp);
    vcrp[OFFSET_VDMA_MM2S_CONTROL_REGISTER/4] = 0x03;
    vcrp[OFFSET_VDMA_MM2S_FRAMEBUFFER1 /4] = handle->bufferHandle[0].PhysicalAddr;
    vcrp[OFFSET_VDMA_MM2S_FRAMEBUFFER2 /4] = handle->bufferHandle[1].PhysicalAddr;
    vcrp[OFFSET_VDMA_MM2S_FRAMEBUFFER3 /4] = handle->bufferHandle[2].PhysicalAddr;
    vcrp[OFFSET_VDMA_MM2S_FRMDLY_STRIDE /4] = 1920 * 3;
    vcrp[OFFSET_VDMA_MM2S_HSIZE /4] = 1920 * 3;
    vcrp[OFFSET_VDMA_MM2S_VSIZE /4] = 1080;
}

int 
vdma_remove(vdma_handle* handle){
    munmap((void*)handle->vdma_crp, DEV_VDMA_SIZE);
    munmap((void*)handle->bufferHandle->VirtualAddr, BUFFER_SIZE);
    close(handle->vdmafp);
    close(handle->bufferfp);
}

int 
register_check(vdma_handle* handle){
    for(int i=0; i<20; i++){
        printf("value:0x%08x at index %d \n",((u_int32_t*)handle->vdma_crp)[i],i);
    }
    printf("=== Dumping first %d elements from VDMA Virtual Buffers ===\n", 20);
    
    /*
    uint8_t* pixel;

    while(1){
        // black
        for (int fb = 0; fb < 3; fb++) {
            pixel = (u_int8_t*)handle->bufferHandle[fb].VirtualAddr;
            for(int h = 0; h < 1080; h++){
                for(int w = 0; w < 1920; w++){
                    pixel[(h*1920 + w)*3 + 0] = 0x00;
                    pixel[(h*1920 + w)*3 + 1] = 0x00;
                    pixel[(h*1920 + w)*3 + 2] = 0x00;
                }
            }
        }
        sleep(3);

        // white
        for (int fb = 0; fb < 3; fb++) {
            pixel = (u_int8_t*)handle->bufferHandle[fb].VirtualAddr;
            for(int h = 0; h < 1080; h++){
                for(int w = 0; w < 1920; w++){
                    pixel[(h*1920 + w)*3 + 0] = 0xFF;
                    pixel[(h*1920 + w)*3 + 1] = 0xFF;
                    pixel[(h*1920 + w)*3 + 2] = 0xFF;
                }
            }
        }
        sleep(3);
    }*/
    uintptr_t buffptr[3];
    for(int i =0; i<3; i++){
        buffptr[i] = handle->bufferHandle[i].VirtualAddr;
    }
    graphics_setup(buffptr);

    for (int fb = 0; fb < 3; fb++) {
        uint32_t *ptr = (uint32_t *)handle->bufferHandle[fb].VirtualAddr;

        printf("\nBufferHandle[%d]:\n", fb);
        for (int i = 0; i < 20; i++) {
            printf("0x%08X ", ptr[i]);
            if ((i + 1) % 8 == 0)
                printf("\n");
        }
        printf("\n---------------------------------------------\n");
    }
} 


int 
main(int argc, char* argv[]){
    vdma_handle vh;
    vdma_setup(&vh);

    register_check(&vh);

    vdma_remove(&vh);

    return 0;
}

