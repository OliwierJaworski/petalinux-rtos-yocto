#include "system_graphics.h"
#include "system_config.h"
#include "system_core.h"

void 
vdmaPxlSet_CB(UG_S16 x, UG_S16 y, UG_COLOR c, void* arg){
	/* Vdma uses GBR888-(msb last)type pixel data, This function calculates current pixel offset
	   fragments the u32 value using ptr arithmetic into u8* indexes; r,g,b,(a not used here),
	   and assigns the matching values by shifting the u32 by color offset.
	*/
	struct GRAPHICSHandle_t *graphics  = (struct GRAPHICSHandle_t*) arg;
	u32 offset = ((y * HDMI_HSIZE + x) * HDMI_RGB); //calculate pixel offset	
	u8* fb = (u8*)graphics->frame_buffers[0];

	fb[offset + 0] = (c >> OFFST_B) & uCHAR_MAX; 	//B
	fb[offset + 1] = (c >> OFFST_G) & uCHAR_MAX;	//G
	fb[offset + 2] = (c >> OFFST_R) & uCHAR_MAX; 	//R
}