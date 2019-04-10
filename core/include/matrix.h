#pragma once

#include <stdint.h>

#include "types.h"
#include "cetris.h"

extern const piece_matrix default_matrices[7];
extern const vec2 basic_movements[4];

void move_current(struct cetris_game* g, vec2 offset);
void overlay_current_matrix(struct cetris_game* g);
void hard_drop(struct cetris_game* g);
void rotate_matrix(struct cetris_game* g, bool clockwise);
i8 check_new_matrix(struct cetris_game* g, piece_matrix m);
