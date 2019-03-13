#pragma once

#include <stdint.h>

#include "cetris.h"

typedef uint8_t piece_matrix[4][4];

const piece_matrix default_matrices[7];
const vec2 basic_movements[4];

void move_current(struct cetris_game* g, vec2 offset);
void overlay_current_matrix(struct cetris_game* g);
void hard_drop(struct cetris_game* g);
void rotate_matrix(struct cetris_game* g, int clockwise);
int8_t check_new_matrix(struct cetris_game* g, piece_matrix m);
