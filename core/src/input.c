#include <stdint.h>

#include "input.h"

#include "types.h"
#include "matrix.h"
#include "cetris.h"

uint8_t handle_inputs(struct cetris_game* g) {
  if ((g->held_moves[RIGHT] || g->held_moves[LEFT]) && !g->das_move_tick) {
    if (g->das_repeat == 0) {
      g->das_move_tick = g->tick + CETRIS_DAS_DELAY;
    } else if (g->das_repeat > 0) {
      g->das_move_tick = g->tick + CETRIS_DAS_PERIOD;
    }
  }

  if (g->held_moves[DOWN] && !g->down_move_tick) {
    g->down_move_tick = g->tick + CETRIS_DROP_PERIOD;
  }

  uint8_t did_move = 0;
  if (g->das_move_tick && g->tick >= g->das_move_tick) {
    if (g->held_moves[RIGHT]) {
      if (g->prev_das_move == RIGHT || g->das_repeat == 0) g->das_repeat++;
      else g->das_repeat = 0;
      move_current(g, basic_movements[RIGHT]);
      g->prev_das_move = RIGHT;
    }

    if (g->held_moves[LEFT]) {
      if (g->prev_das_move == LEFT || g->das_repeat == 0) g->das_repeat++;
      else g->das_repeat = 0;
      move_current(g, basic_movements[LEFT]);
      g->prev_das_move = LEFT;
    }

    g->das_move_tick = 0;
    did_move = 1;
  }

  if (g->down_move_tick && g->tick >= g->down_move_tick) {
    if (g->held_moves[DOWN]) { 
      move_current(g, basic_movements[DOWN]);
    }

    g->down_move_tick = 0;
    did_move = 1;
  }
  return did_move;
}
