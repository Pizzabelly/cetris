#include <stdint.h>

#include "input.h"

#include "types.h"
#include "matrix.h"
#include "cetris.h"

uint8_t handle_inputs(struct cetris_game* g) {
  uint8_t did_move = 0;
  if (g->held_move && !g->next_move_tick) {
    if (g->held_move == RIGHT || g->held_move == LEFT) {
      if (g->prev_move == g->held_move) {
	      g->next_move_tick = g->tick + CETRIS_DAS_PERIOD;
      } else {
	      g->next_move_tick = g->tick + CETRIS_DAS_DELAY;
      }
    } else {
      g->next_move_tick = g->tick + CETRIS_DROP_PERIOD;
    }
  }

  if (g->next_move_tick && g->tick >= g->next_move_tick) {
    switch (g->held_move) {
      case DOWN:
        g->score++;
        move_current(g, basic_movements[DOWN]);
        break;
      case LEFT: 
        move_current(g, basic_movements[LEFT]);
        break;
      case RIGHT:
        move_current(g, basic_movements[RIGHT]);
        break;
    }
    did_move = 1;
  }

  if (did_move) {
    g->next_move_tick = 0;
    g->prev_move = g->held_move;
  }
  return did_move;
}

void clear_held_key(struct cetris_game* g) {
  g->prev_move = 0;
  g->held_move = 0;
  g->next_move_tick = 0;
}
