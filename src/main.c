#include "pd_api.h"
#include "constants.h"
#include "input.h"
#include "flipper.h"
#include "ball.h"
#include <stdbool.h>
#include <math.h>

static int update(void *userdata);

PlaydateAPI* pd;

int frame = 0;
int last_time;
float delta_time = 0.0f;

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg) {
  (void)arg; // silence warning

  if (event == kEventInit) {
    pd = playdate;
    init_buttons(pd);
    init_flippers(pd);
    init_ball(pd);
    pd->display->setRefreshRate(0);
    pd->system->setUpdateCallback(update, playdate);
  }

  return 0;
}

void update_delta_time(void) {
  int current_time = pd->system->getCurrentTimeMilliseconds();
  delta_time = current_time - last_time;
  last_time = current_time;
}

static int update(void* userdata) {
  update_delta_time();

  update_buttons();

  // do this after update_buttons,
  // this will check button state in memory
  update_flippers();

  // seems right to do this after flippers?
  update_ball();

  pd->sprite->updateAndDrawSprites();
  frame ++;
  return 1;
}
