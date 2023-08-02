#include "ball.h"
#include "constants.h"
#include "types.h"

#define HALF_BALL_SIZE 10
#define BALL_MIN_X HALF_BALL_SIZE
#define BALL_MAX_X LCD_COLUMNS-HALF_BALL_SIZE
#define BALL_MIN_Y HALF_BALL_SIZE
#define BALL_MAX_Y LCD_ROWS-HALF_BALL_SIZE

Vector current_vector;

PlaydateAPI* pd;

SpriteCollisionResponseType ball_collider_handler(LCDSprite* sprite, LCDSprite* other) {
  return kCollisionTypeOverlap;
}

void init_ball(PlaydateAPI* playdate) {
  pd = playdate;

  const char* outerr = NULL;

  ball_bmp = pd->graphics->loadBitmap("image/ball4", &outerr);
  ball_sprite = pd->sprite->newSprite();
  pd->sprite->setImage(ball_sprite, ball_bmp, kBitmapUnflipped);
  reset_ball_pos();
  PDRect b_cr = { .x=0, .y=0, .width=20, .height=20 };
  pd->sprite->setCollideRect(ball_sprite, b_cr);
  pd->sprite->setCollisionResponseFunction(ball_sprite, ball_collider_handler);
  pd->sprite->addSprite(ball_sprite);

  current_vector = (Vector){ .x=0, .y=-1 };
}

void reset_ball_pos(void) {
  pd->sprite->moveTo(ball_sprite, SCREEN_MID_X + 30, SCREEN_MID_Y-100);
}

void ball_move(int xBy, int yBy) {
  PDRect ball_bounds = pd->sprite->getBounds(ball_sprite);

  float moveTargetX, moveTargetY;
  int colliders_len;
  SpriteCollisionInfo* collision = pd->sprite->checkCollisions(ball_sprite,
                                                               ball_bounds.x + ball_bounds.width/2 + xBy,
                                                               ball_bounds.y + ball_bounds.height/2 + yBy,
                                                               &moveTargetX,
                                                               &moveTargetY,
                                                               &colliders_len);

  if(collision != NULL && collision->other != NULL) {
    PDRect collider_bounds = pd->sprite->getBounds(collision->other);
    LCDBitmap* colliderBmp = pd->sprite->getImage(collision->other);

    int alpha_collide = pd->graphics->checkMaskCollision(ball_bmp,
                                                         ball_bounds.x,
                                                         ball_bounds.y,
                                                         kBitmapUnflipped,
                                                         colliderBmp,
                                                         collider_bounds.x,
                                                         collider_bounds.y,
                                                         kBitmapUnflipped,
                                                         (LCDRect){.left=0, .top=0, .bottom=0, .right=0});
    if(alpha_collide == 1) {
      return;
    }
  }

  if(moveTargetY < BALL_MAX_Y   &&
     moveTargetY > BALL_MIN_Y   &&
     moveTargetX < BALL_MAX_X   &&
     moveTargetX > BALL_MIN_X) {
    pd->sprite->moveTo(ball_sprite, moveTargetX, moveTargetY);
  }
}

void update_ball(void) {
  ball_move(current_vector.x, -current_vector.y);
}

