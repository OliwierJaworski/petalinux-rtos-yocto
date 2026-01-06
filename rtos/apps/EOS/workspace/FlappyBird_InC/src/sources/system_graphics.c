#include "system_graphics.h"
#include "system_config.h"

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

void DrawPlayer(struct PLAYER_t *p, struct GRAPHICSHandle_t *g){
	Xil_DCacheDisable();
    UG_FillFrame(p->x, p->y, p->x + p->xw, p->y + p->yw, p->pColor);
    Xil_DCacheEnable();
}   
void DrawObj(struct GOBJECT_t*obj, struct GRAPHICSHandle_t *g){
	Xil_DCacheDisable();
    UG_FillFrame(obj->x, obj->y, obj->x + obj->xw, obj->y + obj->yw, obj->Color);
    Xil_DCacheEnable();
} 

int PlayerCheckBounds(struct PLAYER_t *p){
	//above or under out of bound
	if( (p->y + p->yw + p->vy) > HDMI_VSIZE || (p->y + p->vy) < 0)
		return 0;
	// left or right out of bound
	if( (p->x + p->xw + p->vx) > HDMI_HSIZE || (p->x + p->vx) < 0)
		return 0;
	return 1;
}

void TranslatePlayer(int x, int y, struct PLAYER_t* p, struct GRAPHICSHandle_t *g){
	
	Xil_DCacheDisable();
	if(y < 0){ //up
    	UG_FillFrame(p->x, p->y + p->yw + y, p->x + p->xw -x, p->y + p->yw, g->Background);
	}else if(y > 0){ //down
    	UG_FillFrame(p->x, p->y, p->x + p->xw -x, p->y + y, g->Background);
	}
	p->x +=x; 
	p->y +=y;
    DrawPlayer(p, g);
    Xil_DCacheEnable();
}

void TranslateObj(int x, int y, struct GOBJECT_t* obj, struct GRAPHICSHandle_t *g){
	Xil_DCacheDisable();
	if(x < 0){ //left
    	UG_FillFrame(obj->x+obj->xw+x, obj->y, obj->x+obj->xw , obj->y+obj->yw , C_LIGHT_BLUE);
	}else if(x > 0){ //right
    	UG_FillFrame(obj->x, obj->y, obj->x + x, obj->y + obj->yw, C_LIGHT_BLUE);
	}
	obj->x +=x; 
	obj->y +=y;
	DrawObj(obj,g);
    Xil_DCacheEnable();	
}

int PlayerCollisionCheck(struct PLAYER_t *p, struct GAMEHandle_t *g){
	int PRightCollide =(p->x+p->xw);
	int PLowerCollide =(p->y+p->yw);

	for(int i=0; i<2;i++){
		struct GOBJECT_t *obj = &g->gameObj[i];
		int ObjLeftCollide =obj->x;
		int ObjLowerCollide =obj->y+obj->yw;
		int ObjHigherCollide =obj->y;
		if(obj->y > HDMI_VSIZE/2){ //check highercollider
			if(  PRightCollide >= ObjLeftCollide && (PLowerCollide >= ObjHigherCollide) ){
				LOG_UART(LOG_DEBUG,"COLLIDED WITH LOWER BAR", NULL);
				exit(1);
			}	
		} else{
			if(  PRightCollide >= ObjLeftCollide && (p->y <= (obj->y+obj->yw)) ){
				LOG_UART(LOG_DEBUG,"COLLIDED WITH LOWER BAR", NULL);
				exit(1);
			}
		}
	}
}