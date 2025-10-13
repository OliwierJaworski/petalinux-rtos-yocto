#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "utils.h"

int vGraphics_setup(uintptr_t buffptr[3]);
int vGraphics_draw();
int vGraphics_remove();


void DEBUG_recvd_buffer_addr(va_list args);
#endif
