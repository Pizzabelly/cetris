#pragma once

#include <stdint.h>

struct cetris_game;

uint8_t handle_inputs(struct cetris_game* g);
void clear_held_key(struct cetris_game* g);
