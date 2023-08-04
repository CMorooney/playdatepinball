#include "constants.h"
#include "flipper.h"
#include "input.h"
#include "math.h"
#include <stdbool.h>

PlaydateAPI* pd;

#define FLIPPER_IMAGE_COUNT 8
float flipper_angles[FLIPPER_IMAGE_COUNT] = { 26.6f, 18.8f, 10.0f, 0.0f, -6.4f, -14.9f, -22.3f, -29.1f };
float flipper_compliment_angles[FLIPPER_IMAGE_COUNT] = { 63.4f, 71.2, 80.0f, 90.0f, -83.6, 75.1, 67.7, 60.9 };

LCDBitmapTable* flipper_bmp_table;
LCDSprite* flipper_r_sprite;
LCDSprite* flipper_l_sprite;

int current_flipper_frame_r = 0;
int current_flipper_frame_l = 0;
bool activating_flipper_r;
bool activating_flipper_l;

void init_flippers(PlaydateAPI* playdate) {
  playdate = pd;

  const char* outerr = NULL;

  flipper_bmp_table = pd->graphics->newBitmapTable(FLIPPER_IMAGE_COUNT, 100, 100);
  pd->graphics->loadIntoBitmapTable("image/flipper2", flipper_bmp_table, &outerr);

  //right flipper
  flipper_r_sprite = pd->sprite->newSprite();
  pd->sprite->setImage(flipper_r_sprite, pd->graphics->getTableBitmap(flipper_bmp_table, current_flipper_frame_r), kBitmapUnflipped);
  pd->sprite->moveTo(flipper_r_sprite, SCREEN_MID_X + 50, SCREEN_MID_Y+60);
  PDRect f_cr = { .x=0, .y=0, .width=100, .height=100 };
  pd->sprite->setCollideRect(flipper_r_sprite, f_cr);
  pd->sprite->addSprite(flipper_r_sprite);

  //left flipper
  flipper_l_sprite = pd->sprite->newSprite();
  pd->sprite->moveTo(flipper_l_sprite, SCREEN_MID_X-50, SCREEN_MID_Y+60);
  pd->sprite->setCollideRect(flipper_l_sprite, (PDRect){ .x=0, .y=0, .width=100, .height=100 });
  pd->sprite->setImage(flipper_l_sprite, pd->graphics->getTableBitmap(flipper_bmp_table, current_flipper_frame_l), kBitmapFlippedX);
  pd->sprite->setImageFlip(flipper_l_sprite, kBitmapFlippedX);
  pd->sprite->addSprite(flipper_l_sprite);
}

void activate_flipper(enum flipper f) {
  bool left = f == Left;

  int frame = left ? current_flipper_frame_l + 1 : current_flipper_frame_r + 1;

  if(frame > FLIPPER_IMAGE_COUNT - 1) {
    if(left) {
      activating_flipper_l = false;
    } else {
      activating_flipper_r = false;
    }
    return;
  }

  if(left) {
    current_flipper_frame_l = frame;
  } else {
    current_flipper_frame_r = frame;
  }

  pd->sprite->setImage(left ? flipper_l_sprite : flipper_r_sprite,
                       pd->graphics->getTableBitmap(flipper_bmp_table, frame),
                       left ? kBitmapFlippedX : kBitmapUnflipped);
}

void deactivate_flipper(enum flipper f) {
  bool left = f == Left;
  int frame = left ? current_flipper_frame_l-1 : current_flipper_frame_r-1;
  if(frame < 0) return;
  if(left) {
    current_flipper_frame_l = frame;
  } else {
    current_flipper_frame_r = frame;
  }

  pd->sprite->setImage(left ? flipper_l_sprite : flipper_r_sprite,
                       pd->graphics->getTableBitmap(flipper_bmp_table, frame),
                       left ? kBitmapFlippedX : kBitmapUnflipped);
}

void update_flippers(void) {
  if(is_button_pressed(kButtonLeft) || activating_flipper_l) {
    activate_flipper(Left);
  } else if(!is_button_held(kButtonLeft) && !activating_flipper_l) {
    deactivate_flipper(Left);
  }

  if(is_button_pressed(kButtonA) || activating_flipper_r) {
    activate_flipper(Right);
  } else if(!is_button_held(kButtonA) && !activating_flipper_r) {
    deactivate_flipper(Right);
  }
}

float current_flipper_angle_l(void) {
  return flipper_angles[current_flipper_frame_l];
}

float current_flipper_angle_r(void) {
  return flipper_angles[current_flipper_frame_r];
}

Vector current_flipper_normal_l(void) {
  float a = flipper_angles[current_flipper_frame_l];
  float b = flipper_compliment_angles[current_flipper_frame_l];
  return get_normal_for_right_triangle(a, b);
}

Vector current_flipper_normal_r(void) {
  float a = flipper_angles[current_flipper_frame_r];
  float b = flipper_angles[current_flipper_frame_r];
  return get_normal_for_right_triangle(a, b);
}

