#ifndef BALL_HEADER
#define BALL_HEADER

#include "pd_api.h"

void init_ball(PlaydateAPI* playdate);
void reset_ball_pos(void);
void update_ball(void);

LCDBitmap* ball_bmp;
LCDSprite* ball_sprite;

#endif
