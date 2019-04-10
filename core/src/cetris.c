#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "cetris.h"
#include "types.h"
#include "matrix.h"
#include "input.h"

#ifdef BUILD_TESTS
#include "test.h"
#endif

/* FUNCTION PROTOTYPES */

static void init_piece_queue(struct cetris_game* g);
static void shuffle_queue(struct cetris_game* g);
static void set_constants(struct cetris_game* g);

/* LEVEL DROP SPEED VALUES */

static const u32 level_drop_delay[20] = {
  48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 3 
};
  
/* GAME FUNCTIONS */

void init_game(struct cetris_game* g) {
  srand(time(NULL));

  memset(g->board, 0, sizeof(slot) * CETRIS_BOARD_X * CETRIS_BOARD_Y);

#ifdef BUILD_TESTS
  apply_test_board(g, TSPIN_NO_LINES);
#endif

  g->tick = 0;
  g->next_drop_tick = level_drop_delay[0];

  g->current_index = 0;

  memset(g->held_moves, 0, sizeof(bool) * 7);
  g->das_repeat = 0;
  g->prev_das_move = 0;
  g->das_move_tick = 0;
  g->down_move_tick = 0;

  g->lines = 0;
  g->level = 1;
  g->game_over = false;

  g->tspin = false;
  g->mini_tspin = false;

  g->score = 0;

  init_piece_queue(g);
  shuffle_queue(g);

  next_piece(g);
}

void init_piece_queue(struct cetris_game* g) {
  for (u8 i = 0; i < 7; i++) {
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
    memcpy(g->piece_queue[i].m, default_matrices[i], sizeof(piece_matrix));
    g->piece_queue[i].r = INIT;
    g->piece_queue[i].lock_tick = 0;
    g->piece_queue[i].pos = (vec2){3, 20}; // y = 22 - 1 for matrix
  }
}

void shuffle_queue(struct cetris_game* g) {
  for (u8 i = 0; i < 7; i++) {
    struct tetrimino t = g->piece_queue[i];
    u8 rand_index = rand() % 7;
    g->piece_queue[i] = g->piece_queue[rand_index];
    g->piece_queue[rand_index] = t;
  }
}

void update_game_tick(struct cetris_game* g) {
  if (g->game_over) return;

  bool did_move = false;
  if (g->tick == g->next_drop_tick) {
    move_current(g, basic_movements[DOWN]);
    did_move = true;
    g->next_drop_tick = g->tick + level_drop_delay[g->level - 1];
  }

  /* lock piece if it was hovering for CETRIS_LOCK_DELAY */
  if (g->current.lock_tick && g->current.lock_tick <= g->tick) {
    g->current.pos.y++;
    if (check_new_matrix(g, g->current.m) <= 0) {
      next_piece(g);
    }
    g->current.pos.y--;
    g->current.lock_tick = 0;
  }

  if (handle_inputs(g)) {
    did_move = true;
  }
  
  if (did_move) wipe_board(g); 

  g->tick++;
}

void next_piece(struct cetris_game* g) {
  set_constants(g);

  g->current = g->piece_queue[g->current_index];
  if (check_new_matrix(g, g->current.m) <= 0) {
    g->game_over = true;
  }
  g->current_index++;

  if (g->current_index >= 7) {
    g->current_index = 0;
    shuffle_queue(g);
  }
}

void set_constants(struct cetris_game* g) {
  for (u8 x = 0; x < CETRIS_BOARD_X; x++) { 
    for (u8 y = 0; y < CETRIS_BOARD_Y; y++) {
      if (g->board[x][y].occupied) g->board[x][y].constant = 1;
    }
  }
}

void add_score(struct cetris_game* g, u8 lines) {
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
    g->tspin = false;
  } else if (g->mini_tspin) {
    switch (lines) {
      case 0: g->score += 100 * g->level; break;
      case 1: g->score += 200 * g->level; break;
      case 2: g->score += 400 * g->level; break;
    }
    g->mini_tspin = false;
  }
}

void wipe_board(struct cetris_game* g) {
  uint32_t lines_cleared = 0;
  for (u8 y = 0; y < CETRIS_BOARD_Y; y++) {
    bool clear_line = true;
    for (u8 x = 0; x < CETRIS_BOARD_X; x++) {
      if (!g->board[x][y].constant) { 
        memset(&g->board[x][y], 0, sizeof(slot));
      }

      if (g->board[x][y].remove_tick && g->board[x][y].remove_tick <= g->tick) {
        memset(&g->board[x][y], 0, sizeof(slot));
        for (i8 s = y - 1; s >= 0; s--) {
          g->board[x][s + 1] = g->board[x][s];
        }
      }

      if (!g->board[x][y].occupied || g->board[x][y].remove_tick > 0) {
        clear_line = false;
      }
    }
    if (clear_line) {
      lines_cleared++;
      for (u8 x = 0; x < CETRIS_BOARD_X; x++) {
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

/* MOVEMENT FUNCTIONS */

void move_peice(struct cetris_game* g, input_t move) {
  if (!g->held_moves[move]) {
    switch (move) {
      case LEFT:
      case RIGHT:
      case DOWN:
        move_current(g, basic_movements[move]);
        break;
      case HARD_DROP:
        hard_drop(g);
        break;
      case ROTATE_CW:
        rotate_matrix(g, 1);
        break;
      case ROTATE_CCW:
        rotate_matrix(g, 0);
        break;
    }
  }
  g->held_moves[move] = true;
}

void stop_holding(struct cetris_game* g, input_t move) {
  g->held_moves[move] = false;
  if (move == RIGHT || move == LEFT) {
    g->das_move_tick = 0;
    g->das_repeat = 0;
  } else if (move == DOWN) g->down_move_tick = 0;
}
