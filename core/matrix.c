#include <stdbool.h>

#include "types.h"
#include "cetris.h"
#include "matrix.h"

piece_matrix default_matrices[7] = {
  { 
    { 0, 0, 0, 0 },
    { 0, 1, 1, 0 }, 
    { 0, 1, 1, 0 },
    { 0, 0, 0, 0 }
  }, 

  { 
    { 0, 0, 0, 0 },
    { 1, 1, 1, 1 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 }
  },

  { 
    { 0, 0, 0, 0 },
    { 0, 1, 1, 0 },
    { 1, 1, 0, 0 },
    { 0, 0, 0, 0 }
  },

  { 
    { 0, 0, 0, 0 },
    { 1, 1, 0, 0 },
    { 0, 1, 1, 0 },
    { 0, 0, 0, 0 }
  },
  
  { 
    { 0, 0, 0, 0 },
    { 0, 0, 1, 0 },
    { 1, 1, 1, 0 },
    { 0, 0, 0, 0 }
  },

  { 
    { 0, 0, 0, 0 },
    { 1, 0, 0, 0 },
    { 1, 1, 1, 0 },
    { 0, 0, 0, 0 }
  },

  { 
    { 0, 0, 0, 0 },
    { 0, 1, 0, 0 },
    { 1, 1, 1, 0 },
    { 0, 0, 0, 0 }
  }
};

s8 check_matrix(cetris_game* g, piece_matrix* m) {
  for (s8 y = 0; y < 4; y++) {
    for (s8 x = 0; x < 4; x++) {
      vec2 r = (vec2){x + g->current.pos.x, y + g->current.pos.y};
      if (r.y < 0) continue;
      if ((*m)[y][x]) {
        if (r.x >= CETRIS_BOARD_X || r.x < 0) return 0;
        if (r.y >= CETRIS_BOARD_Y) return -1;
        if (g->board[r.x][r.y].occupied && 
            g->board[r.x][r.y].constant) return -1;
      }
    }
  }
  return 1;
}

void set_matrix(cetris_game* g, piece_matrix* m) {
  for (s8 y = 0; y < 4; y++) {
    for (s8 x = 0; x < 4; x++) {
      if ((*m)[y][x]) {
        vec2 r = (vec2){x + g->current.pos.x,
                        y + g->current.pos.y};
        if (r.y >= 0) {
          if (!g->board[r.x][r.y].occupied) {
            g->board[r.x][r.y].occupied = true;
            g->board[r.x][r.y].c = g->current.c;
          }
        }
        if (g->current.ghost_y + y >= 0)
          if (r.y != (g->current.ghost_y + y))
            g->board[r.x][g->current.ghost_y + y].ghost = true;
      }
    }
  }
}

void rotate_matrix(cetris_game* g, piece_matrix* m, bool clockwise) {
  for (u8 x = 0; x < 4; x++) {
    for (u8 y = 0; y < 4; y++) {
      if (g->current.m[y][x]) {
        u8 new_x = (clockwise) ? 1 - (y - 2) : 1 + (y - 2);
        u8 new_y = (clockwise) ? 2 + (x - 1) : 2 - (x - 1);

        if (g->current.t == I) {
          if (clockwise) new_y--;
          else new_x++;
        }

        (*m)[new_y][new_x] = 1;
      }
    }
  }
}
