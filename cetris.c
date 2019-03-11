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
static int8_t check_new_matrix(struct cetris_game* g, piece_matrix m);
static void wipe_board(struct cetris_game* g);
static void set_constants(struct cetris_game* g);
static void rotate_matrix(struct cetris_game* g, int clockwise);
static void overlay_current_matrix(struct cetris_game* g);
static uint8_t handle_inputs(struct cetris_game* g);

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

/* LEVEL DROP SPEED VALUES */
static const uint8_t level_drop_delay[20] = {
  48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 3 
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

static const vec2 basic_movements[4] = {
  {0, 0}, {0, 1}, {1, 0}, {-1, 0} // NONE, DOWN, RIGHT, LEFT
};

void move_current(struct cetris_game* g, vec2 offset) {
  g->current.pos.y += offset.y;
  g->current.pos.x += offset.x;

  int8_t check = check_new_matrix(g, g->current.mat); 
  if (check <= 0) {
    g->current.pos.y -= offset.y;
    g->current.pos.x -= offset.x;

    if (check == -1 && g->current.lock_tick == 0) {
      g->current.lock_tick = g->tick + 30;
    }
  }

  wipe_board(g);
}

void hard_drop(struct cetris_game* g) {
  if (g->game_over) return;

  uint8_t drop = 0;
  uint8_t drop_count = 0;
  while (!drop) {
    g->current.pos.y++;
    drop_count++;
    int8_t check = check_new_matrix(g, g->current.mat);
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

void move_hard_drop(struct cetris_game* g) {
  if (g->input.held_move != HARD_DROP) {
    hard_drop(g);
  }
  g->input.held_move = HARD_DROP;
}

void move_down(struct cetris_game* g) {
  if (g->input.held_move != DOWN) {
    move_current(g, basic_movements[DOWN]);
  }
  g->input.held_move = DOWN;
}

void move_right(struct cetris_game* g) {
  if (g->input.held_move != RIGHT) {
    move_current(g, basic_movements[RIGHT]);
  }
  g->input.held_move = RIGHT;
}

void move_left(struct cetris_game* g) {
  if (g->input.held_move != LEFT) {
    move_current(g, basic_movements[LEFT]);
  }
  g->input.held_move = LEFT;
}

void rotate_matrix(struct cetris_game* g, int clockwise) {
  if (g->current.t == O) return;
  
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

  piece_matrix m;
  memset(m, 0, sizeof(piece_matrix));

  for (uint8_t x = 0; x < 4; x++) {
    for (uint8_t y = 0; y < 4; y++) {
      if (g->current.mat[y][x]) {
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

  uint8_t wall_kick;
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
    default:  // check for invalid rotations
      assert(0);
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
        if (check_new_matrix(g, m) == 1) {
          did_tspin = 0;
        }
        g->current.pos.x -= basic_movements[i].x;
        g->current.pos.y -= basic_movements[i].y;
      }
      if (did_tspin) {
        if (did_kick) g->mini_tspin = 1;
        else g->tspin = 1;
      }
    }

    g->current.r = next;
    memcpy(g->current.mat, m, sizeof(piece_matrix));
    wipe_board(g);
  }
}

void rotate_clockwise(struct cetris_game* g) {
  if (g->input.held_move != ROTATE_CW) {
    rotate_matrix(g, 1);
  }
  g->input.held_move = ROTATE_CW;
}

void rotate_counterclockwise(struct cetris_game* g) {
  if (g->input.held_move != ROTATE_CCW) {
    rotate_matrix(g, 0);
  }
  g->input.held_move = ROTATE_CCW;
}

int8_t check_new_matrix(struct cetris_game* g, piece_matrix m) {
  vec2 r;
  for (uint8_t x = 0; x < 4; x++) {
    for (uint8_t y = 0; y < 4; y++) {
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
  apply_test_board(g, TSPIN_NO_LINES);
#endif

  g->tick = 0;
  g->next_drop_tick = level_drop_delay[0];

  g->current_index = 0;

  g->input.held_move = 0;
  g->input.prev_move = 0;
  g->input.next_move_tick = 0;
  g->input.can_rotate = 0;
  g->input.can_hard_drop = 0;

  g->lines = 0;
  g->level = 1;
  g->game_over = 0;

  g->tspin = 0;
  g->mini_tspin = 0;

  g->score = 0;

  init_piece_queue(g);
  shuffle_queue(g);

  next_piece(g);
}

void init_piece_queue(struct cetris_game* g) {
  for (uint8_t i = 0; i < 7; i++) {
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
    g->piece_queue[i].lock_tick = 0;
    g->piece_queue[i].pos = (vec2){3, 20}; // y = 22 - 1 for matrix
  }
}

void shuffle_queue(struct cetris_game* g) {
  for (uint8_t i = 0; i < 7; i++) {
    struct tetrimino t = g->piece_queue[i];
    uint8_t rand_index = rand() % 7;
    g->piece_queue[i] = g->piece_queue[rand_index];
    g->piece_queue[rand_index] = t;
  }
}

uint8_t handle_inputs(struct cetris_game* g) {
  uint8_t did_move = 0;
  if (g->input.held_move && !g->input.next_move_tick) {
    if (g->input.held_move == RIGHT || g->input.held_move == LEFT) {
      if (g->input.prev_move == g->input.held_move) {
	g->input.next_move_tick = g->tick + CETRIS_DAS_PERIOD;
      } else {
	g->input.next_move_tick = g->tick + CETRIS_DAS_DELAY;
      }
    } else {
      g->input.next_move_tick = g->tick + CETRIS_DROP_PERIOD;
    }
  }

  if (g->input.next_move_tick && g->tick >= g->input.next_move_tick) {
    switch (g->input.held_move) {
      case DOWN:
        g->score++;
        move_current(g, basic_movements[DOWN]);
        break;
      case LEFT: 
        move_current(g, basic_movements[LEFT]);
        break;
      case RIGHT:
        move_current(g, basic_movements[RIGHT]);
        break;
    }
    did_move = 1;
  }

  if (did_move) {
    g->input.next_move_tick = 0;
    g->input.prev_move = g->input.held_move;
  }
  return did_move;
}

void clear_held_key(struct input_manager* input) {
  input->prev_move = 0;
  input->held_move = 0;
  input->next_move_tick = 0;
}

void update_game_tick(struct cetris_game* g) {
  if (g->game_over) return;

  uint8_t did_move = 0;
  if (g->tick == g->next_drop_tick) {
    move_current(g, basic_movements[DOWN]);
    did_move = 1;
    g->next_drop_tick = g->tick + level_drop_delay[g->level - 1];
  }

  /* lock piece if it was hovering for CETRIS_LOCK_DELAY */
  if (g->current.lock_tick && g->current.lock_tick <= g->tick) {
    g->current.pos.y++;
    if (check_new_matrix(g, g->current.mat) <= 0) {
      next_piece(g);
    }
    g->current.pos.y--;
    g->current.lock_tick = 0;
  }

  if (handle_inputs(g)) {
    did_move = 1;
  }
  
  if (did_move) {
    wipe_board(g); 
  }

  g->tick++;
}

void next_piece(struct cetris_game* g) {
  set_constants(g);

  g->current = g->piece_queue[g->current_index];
  if (check_new_matrix(g, g->current.mat) <= 0) {
    g->game_over = 1;
  }
  g->current_index++;

  if (g->current_index >= 7) {
    g->current_index = 0;
    shuffle_queue(g);
  }
}

void set_constants(struct cetris_game* g) {
  for (uint8_t x = 0; x < BOARD_X; x++) { 
    for (uint8_t y = 0; y < BOARD_Y; y++) {
      if (g->board[x][y].occupied) g->board[x][y].constant = 1;
    }
  }
}

void add_score(struct cetris_game* g, int lines) {
  if (!g->tspin && !g->mini_tspin) {
    switch (lines) {
      case 1: g->score += 100 * g->level; break;
      case 2: g->score += 300 * g->level; break;
      case 3: g->score += 500 * g->level; break;
      case 4: g->score += 800 * g->level; break;
    }
  } else if (g->tspin) {
    switch (lines) {
      case 0: g->score += 400 * g->level; break;
      case 1: g->score += 800 * g->level; break;
      case 2: g->score += 1200 * g->level; break;
      case 3: g->score += 1600 * g->level; break;
    }
    g->tspin = 0;
  } else if (g->mini_tspin) {
    switch (lines) {
      case 0: g->score += 100 * g->level; break;
      case 1: g->score += 200 * g->level; break;
      case 2: g->score += 400 * g->level; break;
    }
    g->mini_tspin = 0;
  }
}

void overlay_current_matrix(struct cetris_game* g) {
  vec2 r;
  for (uint8_t y = 0; y < 4; y++) {
    for (uint8_t x = 0; x < 4; x++) {
      r = (vec2){x + g->current.pos.x, y + g->current.pos.y};
      if (g->current.mat[y][x]) {
        g->board[r.x][r.y].occupied = 1;
        g->board[r.x][r.y].c = g->current.c;
      }
    }
  }
}

void wipe_board(struct cetris_game* g) {
  uint8_t lines_cleared = 0;
  for (uint8_t y = 0; y < BOARD_Y; y++) {
    uint8_t clear_line = 1;
    for (uint8_t x = 0; x < BOARD_X; x++) {
      if (!g->board[x][y].constant) { 
        memset(&g->board[x][y], 0, sizeof(slot));
      }

      if (g->board[x][y].remove_tick && g->board[x][y].remove_tick <= g->tick) {
        memset(&g->board[x][y], 0, sizeof(slot));
        for (int8_t s = y - 1; s >= 0; s--) {
          g->board[x][s + 1] = g->board[x][s];
        }
      }

      if (!g->board[x][y].occupied || g->board[x][y].remove_tick > 0) {
        clear_line = 0;
      }
    }
    if (clear_line) {
      lines_cleared++;
      for (uint8_t x = 0; x < BOARD_X; x++) {
        g->board[x][y].remove_tick = g->tick + CETRIS_LINE_CLEAR_DELAY;
      }
    }
  }

  overlay_current_matrix(g);

  assert(lines_cleared <= 4);
  if (lines_cleared > 0) {
    add_score(g, lines_cleared);
    if (lines_cleared > 0) {
      g->lines += lines_cleared;
      if (g->lines >= (g->level * 10) && g->level <= 20) g->level++;
    }
  }
}
