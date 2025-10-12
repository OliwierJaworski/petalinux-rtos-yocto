#include "vdma.h"
#include "utils.h"

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
        TRACE(LOG_ERROR, "open", NULL);
    
    handle->vdma_crp= (uintptr_t) mmap(NULL, DEV_VDMA_SIZE + pg_offset,
            PROT_READ | PROT_WRITE, MAP_SHARED, handle->vdmafp, (off_t) handle->baseAddr);
    if( (void*) handle->vdma_crp == MAP_FAILED )
        TRACE(LOG_ERROR, "mmap1", NULL);
    
    handle->bufferfp = open("/dev/uio3", O_RDWR | O_SYNC);
    if( handle->bufferfp== -1 )
        TRACE(LOG_ERROR, "open", NULL);

    handle->bufferHandle->VirtualAddr = (uintptr_t) mmap(NULL, BUFFER_SIZE ,
            PROT_READ | PROT_WRITE, MAP_SHARED, handle->bufferfp,
            (off_t)BUFFER_OFFSET);
    if( (void*) handle->bufferHandle->VirtualAddr == MAP_FAILED )
        TRACE(LOG_ERROR, "mmap2", NULL);

    for(int i=1; i < 3; i++ ){
        handle->bufferHandle[i].PhysicalAddr= handle->bufferHandle->PhysicalAddr + (i * BUFFER_FSIZE);
        handle->bufferHandle[i].VirtualAddr = handle->bufferHandle->VirtualAddr + (i * BUFFER_FSIZE);
    }

    TRACE(LOG_DEBUG, "checking phys | virt addr assignment", DEBUG_phys_virt_bprint,
        (unsigned long)handle->bufferHandle);
    

    u_int32_t* vcrp = ((u_int32_t*)handle->vdma_crp);
    vcrp[OFFSET_VDMA_MM2S_CONTROL_REGISTER/4] = VDMA_CONTROL_REGISTER_RESET; //reset 
    vcrp[OFFSET_VDMA_MM2S_CONTROL_REGISTER/4] = VDMA_CONTROL_REGISTER_START; //start
    vcrp[OFFSET_VDMA_MM2S_FRAMEBUFFER1 /4] = handle->bufferHandle[0].PhysicalAddr;
    vcrp[OFFSET_VDMA_MM2S_FRAMEBUFFER2 /4] = handle->bufferHandle[1].PhysicalAddr;
    vcrp[OFFSET_VDMA_MM2S_FRAMEBUFFER3 /4] = handle->bufferHandle[2].PhysicalAddr;
    vcrp[OFFSET_VDMA_MM2S_FRMDLY_STRIDE /4] = width * depth;
    vcrp[OFFSET_VDMA_MM2S_HSIZE /4] = height * depth;
    vcrp[OFFSET_VDMA_MM2S_VSIZE /4] = height;
}

int 
vdma_remove(vdma_handle* handle){
    munmap((void*)handle->vdma_crp, DEV_VDMA_SIZE);
    munmap((void*)handle->bufferHandle->VirtualAddr, BUFFER_SIZE);
    close(handle->vdmafp);
    close(handle->bufferfp);
}

int 
vdma_regcheck(vdma_handle* handle){
    
    TRACE(LOG_DEBUG, "dump of first 20 VDMA control registers", DEBUG_f20_ctrl_regs_vdma,
        handle->vdma_crp);
    
    uintptr_t buffptr[3];
    for(int i =0; i<3; i++){
        buffptr[i] = handle->bufferHandle[i].VirtualAddr;
    }
    graphics_setup(buffptr);

    TRACE(LOG_DEBUG, "dump of 20 values * 3 buffers of RGB -> not aligned", DEBUG_virt_buff_dump_first_20x3,
        handle->bufferHandle);
} 


