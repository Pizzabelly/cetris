#pragma once

#include <stdint.h>
#include <stdbool.h>

struct cetris_game;

bool handle_inputs(struct cetris_game* g);
void clear_held_key(struct cetris_game* g);
