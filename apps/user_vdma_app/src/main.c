

#include "utils.h"
#include "vdma.h"
#include "graphics.h"

int 
main(int argc, char* argv[]){
    vdma_handle vh;
    
    vdma_setup(&vh);
    vdma_regcheck(&vh);
    vdma_remove(&vh);

    return 0;
}

