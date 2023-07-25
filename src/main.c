#include "pd_api.h"
#include "constants.h"
#include <stdbool.h>
#include <math.h>

static int update(void *userdata);

PlaydateAPI* pd;

LCDBitmap* ball_bmp;
LCDBitmapTable* flipper_bmp_table;

LCDSprite* ball_sprite;
LCDSprite* flipper_r_sprite;

PDButtons buttons_pressed;
PDButtons buttons_held;
PDButtons buttons_released;

int current_flipper_frame;

static void init(void);

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg) {
  (void)arg; // silence warning

  if (event == kEventInit) {
    pd = playdate;
    init();
    pd->display->setRefreshRate(0);
    pd->system->setUpdateCallback(update, playdate);
  }

  return 0;
}

SpriteCollisionResponseType ball_collider_handler(LCDSprite* sprite, LCDSprite* other) {
  return kCollisionTypeOverlap;
}

void reset_ball_pos(bool left) {
  pd->sprite->moveTo(ball_sprite, SCREEN_MID_X - (left ? 45 : -45), SCREEN_MID_Y-100);
}

void init(void) {
  const char* outerr = NULL;

  flipper_bmp_table = pd->graphics->newBitmapTable(7, 100, 100);
  pd->graphics->loadIntoBitmapTable("image/flipper", flipper_bmp_table, &outerr);
  flipper_r_sprite = pd->sprite->newSprite();
  pd->sprite->setImage(flipper_r_sprite, pd->graphics->getTableBitmap(flipper_bmp_table, current_flipper_frame), kBitmapUnflipped);
  pd->sprite->moveTo(flipper_r_sprite, SCREEN_MID_X-50, SCREEN_MID_Y+80);
  PDRect f_cr = { .x=0, .y=0, .width=100, .height=100 };
  pd->sprite->setCollideRect(flipper_r_sprite, f_cr);
  pd->sprite->addSprite(flipper_r_sprite);

  //ball
  ball_bmp = pd->graphics->loadBitmap("image/ball.png", &outerr);
  ball_sprite = pd->sprite->newSprite();
  pd->sprite->setImage(ball_sprite, ball_bmp, kBitmapUnflipped);
  reset_ball_pos(false);
  PDRect b_cr = { .x=0, .y=0, .width=20, .height=20 };
  pd->sprite->setCollideRect(ball_sprite, b_cr);
  pd->sprite->setCollisionResponseFunction(ball_sprite, ball_collider_handler);
  pd->sprite->addSprite(ball_sprite);

  //test block
  LCDBitmap* block_bmp = pd->graphics->loadBitmap("image/block", &outerr);
  LCDSprite* block_sprite = pd->sprite->newSprite();
  pd->sprite->setImage(block_sprite, block_bmp, kBitmapUnflipped);
  PDRect tb_cr = { .x=0, .y=0, .width=100, .height=100 };
  pd->sprite->moveTo(block_sprite, SCREEN_MID_X+50, SCREEN_MID_Y+80);
  pd->sprite->setCollideRect(block_sprite, tb_cr);
  pd->sprite->addSprite(block_sprite);
}

int checkMaskCollision(LCDBitmap* bitmap1, int x1, int y1, LCDBitmap* bitmap2, int x2, int y2) {
  int bitmap1_width, bitmap1_height,
      bitmap2_width, bitmap2_height,
      bitmap1_rowbytes, bitmap2_rowbytes;

  uint8_t* bitmap1_data;
  uint8_t* bitmap2_data;
  pd->graphics->getBitmapData(bitmap1, &bitmap1_width, &bitmap1_height, &bitmap1_rowbytes, NULL, &bitmap1_data);
  pd->graphics->getBitmapData(bitmap2, &bitmap2_width, &bitmap2_height, &bitmap2_rowbytes, NULL, &bitmap2_data);

  float xMin = fminf(x1, x2);
  float xMax = fmaxf(x1 + bitmap1_width, x2 + bitmap2_width);
  float yMin = fminf(y1, y2);
  float yMax = fmaxf(y1 + bitmap1_height, y2 + bitmap2_width);

  //loop through tightened space to check any overlap
  for(int i = xMin; i < xMax; i++) {
    for (int k = yMin; k < yMax; k++) {

      if(i < x1+bitmap1_width && i > x1       // within x bounds of bmp1
        && i < x2+bitmap2_width && i > x2 &&  // within x bounds of bmp2
        k < y1+bitmap1_height && k > y1       // within y bounds of bmp1
        && k < y2+bitmap2_height && k > y2) { // within y bounds of bmp2

        LCDSolidColor bitmap1Color = samplepixel(bitmap1_data, i-x1, k-y1, bitmap1_rowbytes);
        LCDSolidColor bitmap2Color = samplepixel(bitmap2_data, i-x2, k-y2, bitmap2_rowbytes);

        if(bitmap1Color == kColorBlack && bitmap2Color == kColorBlack) {
          return 1;
        }
      }
    }
  }

  return 0;
}

void ball_gravity(void) {
  float currentX, currentY, resultX, resultY;
  int colliders_len;
  pd->sprite->getPosition(ball_sprite, &currentX, &currentY);
  SpriteCollisionInfo* collision = pd->sprite->checkCollisions(ball_sprite, currentX, currentY+1, &resultX, &resultY, &colliders_len);

  if(collision != NULL && collision->other != NULL) {
    float flipper_x, flipper_y;
    pd->sprite->getPosition(collision->other, &flipper_x, &flipper_y);
    LCDBitmap* bmp = pd->sprite->getImage(collision->other);
    int alpha_collide = checkMaskCollision(ball_bmp, resultX, resultY, bmp, flipper_x, flipper_y);
    if(alpha_collide == 1) {
      return;
    }
  }

  pd->sprite->moveTo(ball_sprite, resultX, resultY);
}

void update_buttons(void) {
  pd->system->getButtonState(&buttons_pressed, &buttons_held, &buttons_released);
}

// checks if button is pressed (and not released)
// call this after update_buttons() to make sure you have
// latest button state in memory
bool is_button_pressed(PDButtons button) {
  if(button & buttons_pressed && !(button & buttons_released)) {
    return true;
  }
  return false;
}

bool is_button_released(PDButtons button) {
  if(button & buttons_released) {
    return true;
  }
  return false;
}

void increment_flipper_frame(int by) {
  int temp = current_flipper_frame+by;
  if(temp > 0 && temp < 6) {
    current_flipper_frame = temp;
    pd->sprite->setImage(flipper_r_sprite, pd->graphics->getTableBitmap(flipper_bmp_table, current_flipper_frame), kBitmapUnflipped);
  }
}

int frame = 0;
static int update(void* userdata) {
  update_buttons();
  if(is_button_pressed(kButtonA)) {
    reset_ball_pos(false);
  } else if(is_button_pressed(kButtonB)) {
    reset_ball_pos(true);
  }

  if(is_button_released(kButtonUp)) {
    increment_flipper_frame(1);
  } else if (is_button_released(kButtonDown)) {
    increment_flipper_frame(-1);
  }

  ball_gravity();
  pd->sprite->updateAndDrawSprites();
  pd->system->drawFPS(0, 0);
  frame ++;
  return 1;
}
