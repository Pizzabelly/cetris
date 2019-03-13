#include "input.h"

uint8_t handle_inputs(struct cetris_game* g) {
  uint8_t did_move = 0;
  if (g->input.held_move && !g->input.next_move_tick) {
    if (g->input.held_move == RIGHT || g->input.held_move == LEFT) {
      if (g->input.prev_move == g->input.held_move) {
	      g->input.next_move_tick = g->tick + CETRIS_DAS_PERIOD;
      } else {
	      g->input.next_move_tick = g->tick + CETRIS_DAS_DELAY;
      }
    } else {
      g->input.next_move_tick = g->tick + CETRIS_DROP_PERIOD;
    }
  }

  if (g->input.next_move_tick && g->tick >= g->input.next_move_tick) {
    switch (g->input.held_move) {
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
    g->input.next_move_tick = 0;
    g->input.prev_move = g->input.held_move;
  }
  return did_move;
}

void clear_held_key(struct input_manager* input) {
  input->prev_move = 0;
  input->held_move = 0;
  input->next_move_tick = 0;
}
