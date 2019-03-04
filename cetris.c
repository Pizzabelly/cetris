#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "cetris.h"

#ifdef BUILD_TESTS
#include "test.h"
#endif

/* FUNCTION PROTOTYPES */

static void init_piece_queue(struct cetris_game* g);
static void shuffle_queue(struct cetris_game* g);
static void next_piece(struct cetris_game* g);
static void move_current(struct cetris_game* g, vec2 offset);
static int check_new_matrix(struct cetris_game* g, piece_matrix m);
static void wipe_board(struct cetris_game* g);
static void set_constants(struct cetris_game* g);
static void rotate_matrix(struct cetris_game* g, int clockwise);
static void clear_move_queue(struct cetris_game* g);

/* DEFAULT MATRIX FOR EACH POSSIBLE TETRIMINO */

static const piece_matrix default_matrices[7] = {
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
  
/* MATRIX MODIFICATION */

static const vec2 cardinal_movements[4] = {
  {0, 1}, {0, -1}, {1, 0}, {-1, 0} // DOWN, UP, RIGHT, LEFT
};

void move_current(struct cetris_game* g, vec2 offset) {
  g->current.pos.y += offset.y;
  g->current.pos.x += offset.x;

  int check = check_new_matrix(g, g->current.mat); 
  if (check <= 0) {
    g->current.pos.y -= offset.y;
    g->current.pos.x -= offset.x;

    if (check == -1) {
      next_piece(g);
    }
    
    clear_move_queue(g);
  }
}

void move_hard_drop(struct cetris_game* g) {
  int drop = 0;
  while (!drop) {
    g->current.pos.y++;
    int check = check_new_matrix(g, g->current.mat);
    if (check <= 0) {
      g->current.pos.y--;
      drop = 1;
    }
  }

  wipe_board(g);
  next_piece(g);
}

void move_down(struct cetris_game* g) {
  if (g->move_queue_pos >= g->move_queue_count - 1) {
    g->move_queue[g->move_queue_count] = DOWN;
    g->move_queue_count++;
  }
}

void move_right(struct cetris_game* g) {
  if (g->move_queue_pos >= g->move_queue_count - 1) {
    g->move_queue[g->move_queue_count] = RIGHT;
    g->move_queue_count++;
  }
}

void move_left(struct cetris_game* g) {
  if (g->move_queue_pos >= g->move_queue_count - 1) {
    g->move_queue[g->move_queue_count] = LEFT;
    g->move_queue_count++;
  }
}

void rotate_matrix(struct cetris_game* g, int clockwise) {
  if (g->current.t == O) return;

  piece_matrix m;
  memset(m, 0, sizeof(piece_matrix));

  rstate next;
  switch (g->current.r) {
    case INIT:
     next = (clockwise) ? RRIGHT : RLEFT;
     break;
    case RRIGHT: 
     next = (clockwise) ? TWO : INIT;
     break;
    case RLEFT:
     next = (clockwise) ? INIT : TWO;
     break;
    case TWO:
     next = (clockwise) ? RLEFT : RRIGHT;
     break;
  }

  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      if (g->current.mat[y][x]) {
        int new_x = (clockwise) ? 1 - (y - 2) : 1 + (y - 2);
        int new_y = (clockwise) ? 2 + (x - 1) : 2 - (x - 1);
        if (g->current.t == I) {
          if (clockwise) new_y--;
          else new_x++;
        }
        m[new_y][new_x] = 1;
      }
    }
  }

  int wall_kick;
  switch (g->current.r) {
    case INIT:
      wall_kick = (next == RRIGHT) ? 0 : 7;
      break;
    case RRIGHT:
      wall_kick = (next == INIT) ? 1 : 2;
      break;
    case RLEFT:
      wall_kick = (next == INIT) ? 6 : 5;
      break;
    case TWO:
      wall_kick = (next == RRIGHT) ? 3 : 4;
      break;
    default:  // check for UB causing invalid rotations
      assert(0);
  }

  vec2 kick;
  int set_current = 0;
  int did_kick = 0;
  for (int i = 0; i < 5; i++) {
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
      int did_tspin = 1;
      for (int i = 0; i < 4; i++) {
        g->current.pos.x += cardinal_movements[i].x;
        g->current.pos.y += cardinal_movements[i].y;
        if (check_new_matrix(g, m) == 1) {
          did_tspin = 0;
          break;
        }
        g->current.pos.x -= cardinal_movements[i].x;
        g->current.pos.y -= cardinal_movements[i].y;
      }
      if (did_tspin) {
        if (did_kick) g->mini_tspin = 1;
        else g->tspin = 1;
      }
    }

    g->current.r = next;
    memcpy(g->current.mat, m, sizeof(piece_matrix));
  }
}

void rotate_clockwise(struct cetris_game* g) {
  rotate_matrix(g, 1);
}

void rotate_counterclockwise(struct cetris_game* g) {
  rotate_matrix(g, 0);
}

int check_new_matrix(struct cetris_game* g, piece_matrix m) {
  vec2 r;
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      r = (vec2){g->current.pos.x + x, g->current.pos.y + y};
      if (m[y][x]) { 
        if (r.x > BOARD_X - 1 || r.x < 0) return 0;

        if (r.y > BOARD_Y - 1 || r.y < 0) return -1;

        if (g->board[r.x][r.y].occupied && 
            g->board[r.x][r.y].constant) return -1;
      }
    }
  }
  return 1;
}

/* GAME FUNCTIONS */

void init_game(struct cetris_game* g) {
  srand(time(NULL));

  memset(g->board, 0, sizeof(slot) * BOARD_X * BOARD_Y);

#ifdef BUILD_TESTS
  apply_test_board(g, TSPIN);
#endif

  g->tick = 0;
  g->current_index = 0;

  clear_move_queue(g);

  g->lines = 0;
  g->level = 0;

  g->tspin = 0;
  g->mini_tspin = 0;

  init_piece_queue(g);
  shuffle_queue(g);

  next_piece(g);
}

void clear_move_queue(struct cetris_game* g) {
  memset(g->move_queue, 0, sizeof(enum movements) * 20);
  g->move_queue_count = 0;
  g->move_queue_pos = 0;
}

void init_piece_queue(struct cetris_game* g) {
  for (int i = 0; i < 7; i++) {
    switch (i) {
      case 0: 
        g->piece_queue[i].t = O;
        g->piece_queue[i].c = COLOR_O;
        break;
      case 1: 
        g->piece_queue[i].t = I; 
        g->piece_queue[i].c = COLOR_I;
        break;
      case 2: 
        g->piece_queue[i].t = S; 
        g->piece_queue[i].c = COLOR_S;
        break;
      case 3: 
        g->piece_queue[i].t = Z; 
        g->piece_queue[i].c = COLOR_Z;
        break;
      case 4: 
        g->piece_queue[i].t = L; 
        g->piece_queue[i].c = COLOR_L;
        break;
      case 5: 
        g->piece_queue[i].t = J; 
        g->piece_queue[i].c = COLOR_J;
        break;
      case 6: 
        g->piece_queue[i].t = T; 
        g->piece_queue[i].c = COLOR_T;
        break;
    }
    memcpy(g->piece_queue[i].mat, default_matrices[i], sizeof(piece_matrix));
    g->piece_queue[i].r = INIT;
    g->piece_queue[i].pos = (vec2){5, 0};
  }
}

void shuffle_queue(struct cetris_game* g) {
  for (int i = 0; i < 7; i++) {
    struct tetrimino t = g->piece_queue[i];
    int rand_index = rand() % 7;
    g->piece_queue[i] = g->piece_queue[rand_index];
    g->piece_queue[rand_index] = t;
  }
}

void update_game_tick(struct cetris_game* g) {
  if ((g->tick % CETRIS_HZ) == 0) move_down(g);

  enum movements current_move = g->move_queue[g->move_queue_pos];

  /* input independedent das movement */
  int delay = 0;
  if (g->move_queue_pos > 0) {
    if (current_move == g->move_queue[g->move_queue_pos - 1]) {
      if (g->move_queue_pos == 1) {
        delay = CETRIS_DAS_DELAY;
      } else {
        delay = CETRIS_DAS_PERIOD;
      }
    } else {
      clear_move_queue(g);
    }
  }
  
  if (!delay || g->tick % delay == 0) {
    wipe_board(g); 
    if (current_move > 0) {
      switch (current_move) {
        case DOWN:
          move_current(g, cardinal_movements[0]);
          break;
        case LEFT: 
          move_current(g, cardinal_movements[3]);
          break;
        case RIGHT:
          move_current(g, cardinal_movements[2]);
          break;
      }
      if (g->move_queue_count != 0) g->move_queue_pos++;
      if (g->move_queue_pos >= 20) clear_move_queue(g);
    }
  }

  g->tick++;
}

void next_piece(struct cetris_game* g) {
  clear_move_queue(g);

  set_constants(g);

  if (g->current_index >= 7) {
    g->current_index = 0;
    shuffle_queue(g);
  }

  g->current = g->piece_queue[g->current_index];
  g->current_index++;
}

void set_constants(struct cetris_game* g) {
  for (int x = 0; x < BOARD_X; x++) { 
    for (int y = 0; y < BOARD_Y; y++) {
      if (g->board[x][y].occupied) g->board[x][y].constant = 1;
    }
  }
}

void wipe_board(struct cetris_game* g) {
  int lines_cleared = 0;
  for (int y = 0; y < BOARD_Y; y++) {
    int clear_line = 1;

    for (int x = 0; x < BOARD_X; x++) {
      if (!g->board[x][y].constant) { 
        memset(&g->board[x][y], 0, sizeof(slot));
      }
      if (g->board[x][y].remove_tick && g->board[x][y].remove_tick <= g->tick) {
        memset(&g->board[x][y], 0, sizeof(slot));
        for (int s = y - 1; s >= 0; s--) {
          g->board[x][s + 1] = g->board[x][s];
        }
      }
      if (!g->board[x][y].occupied || g->board[x][y].remove_tick > 0) clear_line = 0;
    }

    if (clear_line) {
      lines_cleared++;

      for (int x = 0; x < BOARD_X; x++) {
        g->board[x][y].remove_tick = g->tick + CETRIS_LINE_CLEAR_DELAY;
      }
    }
  }

  vec2 r;
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      r = (vec2){x + g->current.pos.x, y + g->current.pos.y};
      if (g->current.mat[y][x]) {
        g->board[r.x][r.y].occupied = 1;
        g->board[r.x][r.y].c = g->current.c;
      }
    }
  }

  assert(lines_cleared <= 4);
  if (lines_cleared > 0) {
    if (g->tspin || g->mini_tspin) {
      g->tspin = 0;
      g->mini_tspin = 0;
    }
    g->lines += lines_cleared;
    if (g->lines >= (g->level * 10) && g->level <= 20) g->level++;
  }
}
