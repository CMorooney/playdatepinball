#ifndef FLIPPER_HEADER
#define FLIPPER_HEADER

#include "types.h"
#include "pd_api.h"

enum flipper { Left, Right };

void init_flippers(PlaydateAPI* playdate);
void update_flippers(void);

float current_flipper_angle_l(void);
float current_flipper_angle_r(void);

Vector current_flipper_normal_l(void);
Vector current_flipper_normal_r(void);

#endif
