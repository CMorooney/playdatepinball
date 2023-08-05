#include "constants.h"
#include "flipper.h"
#include "input.h"
#include "math.h"
#include <stdbool.h>

PlaydateAPI* pd;

#define FLIPPER_IMAGE_COUNT 8
float flipper_normals_r[FLIPPER_IMAGE_COUNT*2] = { -0.5f, -1,
                                                   -0.329372f, -0.9442f,
                                                   -0.20601f, -0.97855f,
                                                   0, -1,
                                                   0.182089f, -0.983282f,
                                                   0.304776f, -0.952424f,
                                                   0.413803, -0.910366f,
                                                   0.514496f, -0.857493f
};

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

Vector current_flipper_normal_l(void) {
  int i = current_flipper_frame_l*2;
  return (Vector) {
    .x = -flipper_normals_r[i],
    .y = flipper_normals_r[i+ 1]
  };
}

Vector current_flipper_normal_r(void) {
  int i = current_flipper_frame_r*2;
  return (Vector) {
    .x = flipper_normals_r[i],
    .y = flipper_normals_r[i + 1]
  };
}

