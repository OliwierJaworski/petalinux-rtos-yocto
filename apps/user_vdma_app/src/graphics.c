#include "graphics.h"
#include "utils.h"
#include <SDL2/SDL_pixels.h>
#include <stdlib.h>

typedef struct {
    int width;
    int height;
    int depth;
    int pitch;
    uint32_t pixel_format;
}video_info_t;

typedef struct {
    video_info_t vinfo;
    SDL_Renderer *renderer;
    SDL_Surface* framebuffers[3];
    uintptr_t buff_ptr[3];
}vdma_graphics_t;

vdma_graphics_t* VG_handler;

int 
vGraphics_setup(uintptr_t buffptr[3]){
    TRACE(LOG_TRACE, LOG_ORIGIN("begin graphics_setup"), NULL);
    
    VG_handler = calloc(1, sizeof(vdma_graphics_t));
    if(!VG_handler)
        TRACE(LOG_ERROR, LOG_ORIGIN("calloc"), NULL);
    

    *VG_handler = (vdma_graphics_t){ .vinfo = (video_info_t){.width = 1920,
                                                             .height = 1080,
                                                             .depth = 24,
                                                             .pitch = 3,
                                                             .pixel_format = SDL_PIXELFORMAT_RGB24}// in reality RBG!
                                    };
    
    memcpy((void*)VG_handler->buff_ptr, (void*)buffptr, sizeof(VG_handler->buff_ptr) );
        TRACE(LOG_DEBUG, LOG_ORIGIN("memcpy, register dump"), DEBUG_recvd_buffer_addr, VG_handler->buff_ptr);
    
    SDL_SetHint(SDL_HINT_VIDEODRIVER, "offscreen");
    
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        TRACE(LOG_ERROR, LOG_ORIGIN(SDL_GetError()), NULL);

    video_info_t * vinfo_tmp = {&VG_handler->vinfo};

    for(int i = 0; i<3; i++){
        VG_handler->framebuffers[i] = SDL_CreateRGBSurfaceWithFormat(0, vinfo_tmp->width,
             vinfo_tmp->height, vinfo_tmp->depth, vinfo_tmp->pixel_format);
        if (!VG_handler->framebuffers[i])
            TRACE(LOG_ERROR, LOG_ORIGIN(SDL_GetError()), NULL);
            
        
        if (SDL_FillRect(VG_handler->framebuffers[i], NULL,
            SDL_MapRGB(VG_handler->framebuffers[i]->format, 255, 0, 0)) != 0) 
            TRACE(LOG_ERROR, LOG_ORIGIN(SDL_GetError()), NULL);

        memcpy((void*)buffptr[i], VG_handler->framebuffers[i]->pixels, VG_handler->framebuffers[i]->h * VG_handler->framebuffers[i]->pitch);
    }

    TRACE(LOG_TRACE, LOG_ORIGIN("end graphics_setup"), NULL);
}

int 
graphics_draw(uint32_t* buffer){
    
    //SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    //SDL_RenderClear(renderer);
    //SDL_RenderPresent(renderer);
}

int
vGraphics_remove(){
    TRACE(LOG_TRACE, LOG_ORIGIN("begin graphics_remove"), NULL);
    for(int i = 0; i<3; i++)
        SDL_FreeSurface(VG_handler->framebuffers[i]);
    SDL_DestroyRenderer(VG_handler->renderer);
    free(VG_handler);
    TRACE(LOG_TRACE, LOG_ORIGIN("end graphics_remove"), NULL);
}

//SDL_CreateRGBSurfaceWithFormatFrom(
