#include <string.h>

#include "matrix.h"
#include "types.h"
#include "cetris.h"

/* SRS WALL KICK VALUES */

// https://tetris.wiki/SRS
static const vec2 srs_wall_kicks[8][5] = {
  { {0, 0}, {-1, 0}, {-1,1},  {0,-2}, {-1,-2} }, // 0->R
  { {0, 0}, {1, 0},  {1,-1},  {0,2},  {1,2}   }, // R->0
  { {0, 0}, {1, 0},  {1,-1},  {0,2},  {1,2}   }, // R->2
  { {0, 0}, {-1, 0}, {-1,1},  {0,-2}, {-1,-2} }, // 2->R
  { {0, 0}, {1, 0},  {1,1},   {0,-2}, {1,-2}  }, // 2->L
  { {0, 0}, {-1, 0}, {-1,-1}, {0,2},  {-1,2}  }, // L->2
  { {0, 0}, {-1, 0}, {-1,-1}, {0,2},  {-1,2}  }, // L->0
  { {0, 0}, {1, 0},  {1,1},   {0,-2}, {1,-2}  }  // 0->L
};

static const vec2 srs_wall_kicks_i[8][5] = {
  { {0, 0}, {-2, 0}, {1, 0},  {-2,-1}, {1,2}   }, // 0->R
  { {0, 0}, {2, 0},  {-1, 0}, {2,1},   {-1,-2} }, // R->0
  { {0, 0}, {-1, 0}, {2, 0},  {-1,2},  {2,-1}  }, // R->2
  { {0, 0}, {1, 0},  {-2, 0}, {1,-2},  {-2,1}  }, // 2->R
  { {0, 0}, {2, 0},  {-1, 0}, {2,1},   {-1,-2} }, // 2->L
  { {0, 0}, {-2, 0}, {1, 0},  {-2,-1}, {1,2}   }, // L->2
  { {0, 0}, {1, 0},  {-2, 0}, {1,-2},  {-2,1}  }, // L->0
  { {0, 0}, {-1, 0}, {2, 0},  {-1,2},  {2,-1}  }  // 0->L
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

const vec2 basic_movements[4] = {
  {0, 0}, {0, 1}, {1, 0}, {-1, 0} // NONE, DOWN, RIGHT, LEFT
};

void move_current(struct cetris_game* g, vec2 offset) {
  if (g->game_over) return;

  g->current.pos.y += offset.y;
  g->current.pos.x += offset.x;

  int8_t check = check_new_matrix(g, g->current.m); 
  if (check <= 0) {
    g->current.pos.y -= offset.y;
    g->current.pos.x -= offset.x;

    if (check == -1 && g->current.lock_tick == 0) {
      g->current.lock_tick = g->tick + 30;
    }
  }

  wipe_board(g);
}

void overlay_current_matrix(struct cetris_game* g) {
  for (uint8_t y = 0; y < 4; y++) {
    for (uint8_t x = 0; x < 4; x++) {
      vec2 r = (vec2){x + g->current.pos.x, y + g->current.pos.y};
      if (g->current.m[y][x]) {
        g->board[r.x][r.y].occupied = 1;
        g->board[r.x][r.y].c = g->current.c;
      }
    }
  }
}

void hard_drop(struct cetris_game* g) {
  if (g->game_over) return;

  uint8_t drop = 0;
  uint8_t drop_count = 0;
  while (!drop) {
    g->current.pos.y++;
    drop_count++;
    int8_t check = check_new_matrix(g, g->current.m);
    if (check <= 0) {
      g->current.pos.y--;
      drop_count--;
      drop = 1;
    }
  }
  
  g->score += 2 * drop_count; // 2 score for each harddrop cell

  wipe_board(g);
  next_piece(g);
}

void rotate_matrix(struct cetris_game* g, int clockwise) {
  if (g->game_over) return;
  if (g->current.t == O) return;
  
  rstate next; 
  int8_t wall_kick;
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

  for (uint8_t x = 0; x < 4; x++) {
    for (uint8_t y = 0; y < 4; y++) {
      if (g->current.m[y][x]) {
        uint8_t new_x = (clockwise) ? 1 - (y - 2) : 1 + (y - 2);
        uint8_t new_y = (clockwise) ? 2 + (x - 1) : 2 - (x - 1);

        if (g->current.t == I) {
          if (clockwise) new_y--;
          else new_x++;
        }

        m[new_y][new_x] = 1;
      }
    }
  }

  vec2 kick;
  uint8_t set_current = 0;
  uint8_t did_kick = 0;
  for (uint8_t i = 0; i < 5; i++) {
    if (g->current.t == I) {
      kick = srs_wall_kicks_i[wall_kick][i];
    } else {
      kick = srs_wall_kicks[wall_kick][i];
    }
    g->current.pos.x += kick.x;
    g->current.pos.y += kick.y;
    if (check_new_matrix(g, m) > 0) {
      set_current = 1;
      if (i > 0) did_kick = 1;
      break;
    } else {
      g->current.pos.x -= kick.x;
      g->current.pos.y -= kick.y;
    }
  }

  if (set_current) { 
    /* check for tspin */
    if (g->current.t == T) {
      uint8_t did_tspin = 1;
      for (uint8_t i = 0; i < 4; i++) {
        g->current.pos.x += basic_movements[i].x;
        g->current.pos.y += basic_movements[i].y;

        if (check_new_matrix(g, m) == 1)
          did_tspin = 0;

        g->current.pos.x -= basic_movements[i].x;
        g->current.pos.y -= basic_movements[i].y;
      }

      if (did_tspin) {
        if (did_kick) g->mini_tspin = 1;
        else g->tspin = 1;
      }
    }

    g->current.r = next;
    memcpy(g->current.m, m, sizeof(piece_matrix));
    wipe_board(g);
  }
}

int8_t check_new_matrix(struct cetris_game* g, piece_matrix m) {
  for (uint8_t x = 0; x < 4; x++) {
    for (uint8_t y = 0; y < 4; y++) {
      vec2 r = (vec2){g->current.pos.x + x, g->current.pos.y + y};
      if (m[y][x]) { 
        if (r.x > CETRIS_BOARD_X - 1 || r.x < 0) 
          return 0;

        if (r.y > CETRIS_BOARD_Y - 1 || r.y < 0) 
          return -1;

        if (g->board[r.x][r.y].occupied && g->board[r.x][r.y].constant) 
          return -1;
      }
    }
  }
  return 1;
}
