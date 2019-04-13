#pragma once

#include <stdint.h>

#include "types.h"
#include "cetris.h"

extern const piece_matrix default_matrices[7];

void move_current(struct cetris_game* g, input_t move);
void hard_drop(struct cetris_game* g);
void rotate_matrix(struct cetris_game* g, bool clockwise);
void set_matrix(struct cetris_game* g, piece_matrix *m);
i8 check_matrix(struct cetris_game* g, piece_matrix *m);
