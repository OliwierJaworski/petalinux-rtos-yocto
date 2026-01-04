#pragma once

#include "../UGUI/ugui.h"
#include "system_core.h"

void vdmaPxlSet_CB(UG_S16 x, UG_S16 y, UG_COLOR c, void* arg);
void DrawPlayer(struct PLAYER_t *p, struct GRAPHICSHandle_t *g);    
void TranslatePlayer(int x, int y, struct PLAYER_t* p, struct GRAPHICSHandle_t *g); //player movement
int PlayerCheckBounds(struct PLAYER_t *p);