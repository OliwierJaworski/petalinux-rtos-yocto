#include "graphics.h"
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <string.h>

SDL_Renderer *renderer = NULL;
SDL_Surface* surf[3];
uintptr_t Ibuffptr[3];

#define WIDTH 1920
#define HEIGHT 1080 
int width = 1920;
int height = 1080;
int depth = 24;
int pitch = 3*WIDTH; // 3 bytes per pixel * pixels per row
uint32_t pixel_format = SDL_PIXELFORMAT_RGB24;

int 
graphics_setup(uintptr_t buffptr[3]){
    TRACE(LOG_TRACE, LOG_ORIGIN("begin graphics_setup"), NULL);
    SDL_SetHint(SDL_HINT_VIDEODRIVER, "offscreen");
    
    //putenv("SDL_VIDEODRIVER=dummy");
    memcpy((void*)Ibuffptr, (void*)buffptr, sizeof(Ibuffptr) );
    TRACE(LOG_DEBUG, LOG_ORIGIN("memcpy, register dump"), DEBUG_recvd_buffer_addr, Ibuffptr);

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 3;
    }

    /*for(int i = 0; i<3; i++){
        surf[i] = SDL_CreateRGBSurfaceWithFormatFrom((void*)buffptr[i], WIDTH, HEIGHT, depth, pitch, pixel_format);
        if (surf[i] == NULL) {
            SDL_Log("Creating surface failed: %s", SDL_GetError());
            exit(1);
        }

        SDL_FillRect(surf[i], NULL, SDL_MapRGB(surf[i]->format, 0, 255, 0));
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                         "SDL_FillRect failed (buffer %d): %s", i, SDL_GetError());
    }*/ 
    SDL_Surface* tmp = SDL_CreateRGBSurfaceWithFormat(0, WIDTH, HEIGHT, depth, pixel_format);
    if (!tmp) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "tmp surface creation failed: %s", SDL_GetError());
        return -4;
    }

    Uint32 green = SDL_MapRGB(tmp->format, 0, 255, 0);
    if (SDL_FillRect(tmp, NULL, green) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "tmp fill failed: %s", SDL_GetError());
        SDL_FreeSurface(tmp);
        return -5;
    }

    for (int i = 0; i < 3; i++) {
        memcpy((void*)buffptr[i], tmp->pixels, tmp->h * tmp->pitch);
    }
    TRACE(LOG_TRACE, LOG_ORIGIN("end graphics_setup"), NULL);
}

int 
graphics_draw(uint32_t* buffer){
    
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

int
graphics_remove(){
    SDL_DestroyRenderer(renderer);
}
