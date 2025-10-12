#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "SDL2/SDL.h"

int graphics_setup(uintptr_t buffptr[3]);
int graphics_draw(uint32_t* buffer);
int graphics_remove();

#endif
