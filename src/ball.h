#ifndef BALL_HEADER
#define BALL_HEADER

#include "types.h"
#include "pd_api.h"

void init_ball(PlaydateAPI* playdate);
void reset_ball_pos(void);
void update_ball(int frame);

void get_current_ball_pos(float* x, float* y);

LCDBitmap* ball_bmp;
LCDSprite* ball_sprite;

#endif
