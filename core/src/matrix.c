#include <string.h>

#include "matrix.h"
#include "types.h"
#include "cetris.h"

/* SRS WALL KICK VALUES */

/* https://tetris.wiki/SRS */
static const vec2 srs_wall_kicks[8][5] = {
  { {0, 0}, {-1, 0}, {-1,1},  {0,-2}, {-1,-2} }, /* 0->R */
  { {0, 0}, {1, 0},  {1,-1},  {0,2},  {1,2}   }, /* R->0 */
  { {0, 0}, {1, 0},  {1,-1},  {0,2},  {1,2}   }, /* R->2 */
  { {0, 0}, {-1, 0}, {-1,1},  {0,-2}, {-1,-2} }, /* 2->R */
  { {0, 0}, {1, 0},  {1,1},   {0,-2}, {1,-2}  }, /* 2->L */
  { {0, 0}, {-1, 0}, {-1,-1}, {0,2},  {-1,2}  }, /* L->2 */
  { {0, 0}, {-1, 0}, {-1,-1}, {0,2},  {-1,2}  }, /* L->0 */
  { {0, 0}, {1, 0},  {1,1},   {0,-2}, {1,-2}  }  /* 0->L */
};

static const vec2 srs_wall_kicks_i[8][5] = {
  { {0, 0}, {-2, 0}, {1, 0},  {-2,-1}, {1,2}   }, /* 0->R */
  { {0, 0}, {2, 0},  {-1, 0}, {2,1},   {-1,-2} }, /* R->0 */
  { {0, 0}, {-1, 0}, {2, 0},  {-1,2},  {2,-1}  }, /* R->2 */
  { {0, 0}, {1, 0},  {-2, 0}, {1,-2},  {-2,1}  }, /* 2->R */
  { {0, 0}, {2, 0},  {-1, 0}, {2,1},   {-1,-2} }, /* 2->L */
  { {0, 0}, {-2, 0}, {1, 0},  {-2,-1}, {1,2}   }, /* L->2 */
  { {0, 0}, {1, 0},  {-2, 0}, {1,-2},  {-2,1}  }, /* L->0 */
  { {0, 0}, {-1, 0}, {2, 0},  {-1,2},  {2,-1}  }  /* 0->L */
};

/* DEFAULT MATRIX FOR EACH POSSIBLE TETRIMINO */

const piece_matrix default_matrices[7] = {
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

/* MATRIX MODIFICATION */

const vec2 basic_movements[5] = {
  {0, 0}, {0, 1}, {0, 1}, {1, 0}, {-1, 0} /* NONE, DOWN, USER_DOWN, RIGHT, LEFT */
};

void move_current(cetris_game* g, input_t move) {
  if (g->game_over || g->next_piece_tick) return;

  g->current.pos.y += basic_movements[move].y; 
  g->current.pos.x += basic_movements[move].x;

  i8 check = check_matrix(g, &g->current.m); 
  if (check <= 0) {
    g->current.pos.y -= basic_movements[move].y; 
    g->current.pos.x -= basic_movements[move].x;

    if (move == USER_DOWN) g->score++;
    if (move == DOWN && check == -1 && !g->current.lock_tick) {
      g->current.lock_tick = g->tick + CETRIS_LOCK_DELAY;
    }
  }

  update_board(g);
}

i8 check_matrix(cetris_game* g, piece_matrix* m) {
  i8 x, y;
  for (y = 0; y < 4; y++) {
    for (x = 0; x < 4; x++) {
      vec2 r = new_vec2(x + g->current.pos.x, y + g->current.pos.y);
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
  i8 x, y;
  for (y = 0; y < 4; y++) {
    for (x = 0; x < 4; x++) {
      vec2 r = new_vec2(x + g->current.pos.x, y + g->current.pos.y);
      if ((*m)[y][x]) {
        if (r.y >= 0) {
          g->board[r.x][r.y].occupied = true;
          g->board[r.x][r.y].c = g->current.c;
        }
        if (g->current.ghost_y + y >= 0) {
          if (r.y != (g->current.ghost_y + y)) { 
            g->board[r.x][g->current.ghost_y + y].ghost = true;
          }
        }
      }
    }
  }
}

void hard_drop(cetris_game* g) {
  if (g->game_over || g->next_piece_tick) return;

  bool drop = false;
  u8 drop_count = 0;
  while (!drop) {
    g->current.pos.y++;
    drop_count++;
    if (check_matrix(g, &g->current.m) <= 0) {
      g->current.pos.y--;
      drop_count--;
      drop = true;
    }
  }
  
  /* 2 score for each hard-drop'd cell */
  g->score += 2 * drop_count; 

  update_board(g);
  lock_current(g);
}

void rotate_matrix(cetris_game* g, bool clockwise) {
  if (g->game_over || g->next_piece_tick) return;
  if (g->current.t == O) return;
 
  u8 x, y, i;

  rstate next = 0; 
  u8 wall_kick = 0;
  switch (g->current.r) {
    case INIT:
     if (clockwise) {
       next = ONCE_RIGHT;
       wall_kick = 0;
     } else {
       next = ONCE_LEFT;
       wall_kick = 7;
     } break;
    case ONCE_RIGHT: 
     if (clockwise) {
       next = TWICE;
       wall_kick = 2;
     } else {
       next = INIT;
       wall_kick = 1;
     } break;
    case ONCE_LEFT:
     if (clockwise) {
       next = INIT;
       wall_kick = 6;
     } else {
       next = TWICE;
       wall_kick = 5;
     } break;
    case TWICE:
     if (clockwise) {
       next = ONCE_LEFT;
       wall_kick = 4;
     } else {
       next = ONCE_RIGHT;
       wall_kick = 3;
     } break;
  }

  piece_matrix m;
  memset(m, 0, sizeof(piece_matrix));

  for (x = 0; x < 4; x++) {
    for (y = 0; y < 4; y++) {
      if (g->current.m[y][x]) {
        u8 new_x = (clockwise) ? 1 - (y - 2) : 1 + (y - 2);
        u8 new_y = (clockwise) ? 2 + (x - 1) : 2 - (x - 1);

        if (g->current.t == I) {
          if (clockwise) {
            new_y--;
          } else {
            new_x++;
          }
        }

        m[new_y][new_x] = 1;
      }
    }
  }

  vec2 kick;
  bool set_current = false;
  bool did_kick = false;
  for (i = 0; i < 5; i++) {
    if (g->current.t == I) {
      kick = srs_wall_kicks_i[wall_kick][i];
    } else {
      kick = srs_wall_kicks[wall_kick][i];
    }

    g->current.pos.x += kick.x;
    g->current.pos.y += kick.y;

    if (check_matrix(g, &m) > 0) {
      set_current = true;
      if (i > 0) did_kick = true;
      break;
    } else {
      g->current.pos.x -= kick.x;
      g->current.pos.y -= kick.y;
    }
  }

  if (set_current) { 

    /* check for tspin */
    if (g->current.t == T) {
      bool did_tspin = true;
      for (i = 1; i < 5; i++) {
        g->current.pos.x += basic_movements[i].x;
        g->current.pos.y += basic_movements[i].y;

        if (check_matrix(g, &m) == 1) did_tspin = false;

        g->current.pos.x -= basic_movements[i].x;
        g->current.pos.y -= basic_movements[i].y;
      }

      if (did_tspin) {
        if (did_kick) g->mini_tspin = true;
        else g->tspin = true;
      }
    }

    g->current.r = next;
    memcpy(g->current.m, m, sizeof(piece_matrix));
    update_board(g);
  }
}
