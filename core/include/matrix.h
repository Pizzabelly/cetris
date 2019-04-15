#pragma once

#include <stdint.h>

#include "types.h"
#include "cetris.h"

extern const piece_matrix default_matrices[7];

void move_current(cetris_game* g, input_t move);
void hard_drop(cetris_game* g);
void rotate_matrix(cetris_game* g, bool clockwise);
void set_matrix(cetris_game* g, piece_matrix* m);
i8 check_matrix(cetris_game* g, piece_matrix* m);
