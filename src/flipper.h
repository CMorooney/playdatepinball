#ifndef FLIPPER_HEADER
#define FLIPPER_HEADER

#include "pd_api.h"

#define FLIPPER_IMAGE_COUNT 8

enum flipper { Left, Right };

LCDBitmapTable* flipper_bmp_table;
LCDSprite* flipper_r_sprite;
LCDSprite* flipper_l_sprite;

void init_flippers(PlaydateAPI* playdate);
void update_flippers(void);

#endif
