#pragma once

#include <stdint.h>

#include "cetris.h"

typedef enum {
  DOWN       = 1,
  RIGHT      = 2,
  LEFT       = 3,
  ROTATE_CCW = 4,
  ROTATE_CW  = 5,
  HARD_DROP  = 6
} input_t;

struct input_manager {
  input_t held_move;
  input_t prev_move;
  int next_move_tick;
  uint8_t can_rotate;
  uint8_t can_hard_drop;
};

uint8_t handle_inputs(struct cetris_game* g);
void clear_held_key(struct input_manager* input);
