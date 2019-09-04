#pragma once

#include "types.h"
#include "cetris.h"

extern piece_matrix default_matrices[7];
s8 check_matrix(cetris_game* g, piece_matrix* m);
void set_matrix(cetris_game* g, piece_matrix* m);
void rotate_matrix(cetris_game* g, piece_matrix* m, bool clockwise);
