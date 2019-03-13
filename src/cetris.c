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
static void wipe_board(struct cetris_game* g);
static void set_constants(struct cetris_game* g);

/* LEVEL DROP SPEED VALUES */

static const uint8_t level_drop_delay[20] = {
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
    memcpy(g->piece_queue[i].m, default_matrices[i], sizeof(piece_matrix));
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
    if (check_new_matrix(g, g->current.m) <= 0) {
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
  if (check_new_matrix(g, g->current.m) <= 0) {
    g->game_over = 1;
  }
  g->current_index++;

  if (g->current_index >= 7) {
    g->current_index = 0;
    shuffle_queue(g);
  }
}

void set_constants(struct cetris_game* g) {
  for (uint8_t x = 0; x < CETRIS_BOARD_X; x++) { 
    for (uint8_t y = 0; y < CETRIS_BOARD_Y; y++) {
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

void wipe_board(struct cetris_game* g) {
  uint8_t lines_cleared = 0;
  for (uint8_t y = 0; y < CETRIS_BOARD_Y; y++) {
    uint8_t clear_line = 1;
    for (uint8_t x = 0; x < CETRIS_BOARD_X; x++) {
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
      for (uint8_t x = 0; x < CETRIS_BOARD_X; x++) {
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

void move_hard_drop(struct cetris_game* g) {
  if (g->input.held_move != HARD_DROP) {
    hard_drop(g);
  }
  g->input.held_move = HARD_DROP;
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
