#ifndef FLIPPER_HEADER
#define FLIPPER_HEADER

#include "pd_api.h"

enum flipper { Left, Right };

void init_flippers(PlaydateAPI* playdate);
void update_flippers(void);

float current_flipper_angle_l(void);
float current_flipper_angle_r(void);

void current_flipper_normal_l(float* normal_x, float* normal_y);
void current_flipper_normal_r(float* normal_x, float* normal_y);

#endif
