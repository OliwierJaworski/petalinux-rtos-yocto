
#include "vdma.h"
#include "graphics.h"

int 
main(int argc, char* argv[]){
    vdma_handle vh;
    
    vdma_setup(&vh);
    if(VERBOSITY == 3){
        vdma_regcheck(&vh);
    }

    uintptr_t buffptr[3];
    for(int i =0; i<3; i++){
        buffptr[i] = vh.bufferHandle[i].VirtualAddr;
    }
    vGraphics_setup(buffptr);
    
    vGraphics_remove();
    vdma_remove(&vh);

    return 0;
}

