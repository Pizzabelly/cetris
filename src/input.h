#pragma once

#include <stdint.h>

#include "types.h"
#include "cetris.h"

uint8_t handle_inputs(struct cetris_game* g);
void clear_held_key(struct cetris_game* g);
