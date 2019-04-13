#include <stdint.h>

#include "input.h"

#include "types.h"
#include "matrix.h"
#include "cetris.h"

bool handle_inputs(struct cetris_game* g) {
  if ((g->held_moves[RIGHT] || g->held_moves[LEFT]) && !g->das_move_tick) {
    if (g->das_repeat == 0) {
      g->das_move_tick = g->tick + CETRIS_DAS_DELAY;
    } else if (g->das_repeat > 0) {
      g->das_move_tick = g->tick + CETRIS_DAS_PERIOD;
    }
  }

  if (g->held_moves[USER_DOWN] && !g->down_move_tick) {
    g->down_move_tick = g->tick + CETRIS_DROP_PERIOD;
  }

  bool did_move = false;
  if (g->das_move_tick && g->tick >= g->das_move_tick) {
    if (g->held_moves[RIGHT]) {
      if (g->prev_das_move == RIGHT || g->das_repeat == 0) g->das_repeat++;
      else g->das_repeat = 0;
      move_current(g, RIGHT);
      g->prev_das_move = RIGHT;
    }

    if (g->held_moves[LEFT]) {
      if (g->prev_das_move == LEFT || g->das_repeat == 0) g->das_repeat++;
      else g->das_repeat = 0;
      move_current(g, LEFT);
      g->prev_das_move = LEFT;
    }

    g->das_move_tick = 0;
    did_move = true;
  }

  if (g->down_move_tick && g->tick >= g->down_move_tick) {
    if (g->held_moves[USER_DOWN]) { 
      move_current(g, USER_DOWN);
      did_move = true;
    }

    g->down_move_tick = 0;
  }
  return did_move;
}
