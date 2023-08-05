#include "ball.h"
#include "flipper.h"
#include "constants.h"
#include "types.h"
#include "math.h"

#define BALL_RADIUS 10
#define BALL_MIN_X BALL_RADIUS
#define BALL_MIN_Y BALL_RADIUS
#define BALL_MAX_X LCD_COLUMNS-BALL_RADIUS
#define BALL_MAX_Y LCD_ROWS-BALL_RADIUS

#define BALL_T_VELOCITY 3
#define BALL_ACCEL 0.1f

#define ANIMATE_ON 2
#define VECTOR_MULTIPLIER 3

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

  current_vector = (Vector){ .x=0, .y=0 };
}

void reset_ball_pos(void) {
  pd->sprite->moveTo(ball_sprite, SCREEN_MID_X + 30, SCREEN_MID_Y-100);
}

void ball_move(float xBy, float yBy) {
  PDRect ball_bounds = pd->sprite->getBounds(ball_sprite);

  float moveTargetX, moveTargetY;
  int colliders_len;
  int x_modifier = xBy < 0 ? -BALL_RADIUS : BALL_RADIUS;
  int y_modifier = yBy < 0 ? -BALL_RADIUS : BALL_RADIUS;
  int new_x = ball_bounds.x + x_modifier + xBy;
  int new_y = ball_bounds.y + y_modifier + yBy;


  SpriteCollisionInfo* collision = pd->sprite->checkCollisions(ball_sprite,
                                                               new_x,
                                                               new_y,
                                                               &moveTargetX,
                                                               &moveTargetY,
                                                               &colliders_len);
  if(collision != NULL && collision->other != NULL) {
    PDRect collider_bounds = pd->sprite->getBounds(collision->other);
    LCDBitmap* colliderBmp = pd->sprite->getImage(collision->other);

    int alpha_collide = pd->graphics->checkMaskCollision(ball_bmp,
                                                         ball_bounds.x + xBy,
                                                         ball_bounds.y + yBy,
                                                         kBitmapUnflipped,
                                                         colliderBmp,
                                                         collider_bounds.x,
                                                         collider_bounds.y,
                                                         kBitmapUnflipped,
                                                         (LCDRect){.left=0, .top=0, .bottom=0, .right=0});
    if(alpha_collide == 1) {
      Vector flipper_normal = new_x < LCD_COLUMNS/2 ? current_flipper_normal_l() : current_flipper_normal_r();
      Vector reflection = multiply_vector(flipper_normal, VECTOR_MULTIPLIER);
      reflection.y = -reflection.y;
      /* Vector reflection = mirror_vector(normalize_vector(current_vector), flipper_normal); */
      pd->system->logToConsole("(%f, %f)", reflection.x, reflection.y);
      current_vector = reflection;
      return;
    }
  }

  if(moveTargetY < BALL_MAX_Y   &&
     moveTargetY > BALL_MIN_Y   &&
     moveTargetX < BALL_MAX_X   &&
     moveTargetX > BALL_MIN_X) {
    pd->sprite->moveTo(ball_sprite, moveTargetX, moveTargetY);
  }

  if (current_vector.y > -BALL_T_VELOCITY) {
    current_vector.y -= BALL_ACCEL;
  }
}


void update_ball(int frame) {
  if(frame % ANIMATE_ON == 0) {
    ball_move(current_vector.x, -current_vector.y);
  }
}

