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

static void init_piece_queue(cetris_game* g);
static void shuffle_queue(cetris_game* g);
static void add_score(cetris_game* g, u8 lines);

/* LEVEL DROP SPEED VALUES */

// https://tetris.fandom.com/wiki/Tetris_Worlds
// TODO: Make this more accurate
static const u32 level_drop_delay[20] = {
  60, 48, 37, 28, 21, 16, 11, 8, 6, 4, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1 
};
  
/* GAME FUNCTIONS */

void init_game(cetris_game* g) {
  
  /* check for config errors */
  assert(CETRIS_NEXT_PIECE_DELAY >= CETRIS_LINE_CLEAR_DELAY);

  srand(time(NULL));

#ifdef BUILD_TESTS
  apply_test_board(g, TSPIN_NO_LINES);
#endif

  memset(g, 0, sizeof(cetris_game));

  g->level = CETRIS_STARTING_LEVEL;

  init_piece_queue(g);
  shuffle_queue(g);

  next_piece(g);
}

void init_piece_queue(cetris_game* g) {
  for (u8 i = 0; i < 7; i++) {
    g->piece_queue[i].t = i;
    g->piece_queue[i].c = i + 1;
    memcpy(g->piece_queue[i].m, default_matrices[i], sizeof(piece_matrix));
    g->piece_queue[i].r = INIT;
    g->piece_queue[i].lock_tick = 0;
    g->piece_queue[i].locked = false;
    g->piece_queue[i].ghost_y = 0;

    /* Pieces should spawn so that on the first down 
     * tick the bottom row will show. Values here are adjusted 
     * for the default 4x4 matricies for each piece */
    g->piece_queue[i].pos.x = 3;
    g->piece_queue[i].pos.y = (i == I) ? 17 : 16;
  }
}

void shuffle_queue(cetris_game* g) {
  for (u8 i = 0; i < 7; i++) {
    tetrimino t = g->piece_queue[i];
    u8 rand_index = rand() % 7;
    g->piece_queue[i] = g->piece_queue[rand_index];
    g->piece_queue[rand_index] = t;
  }
}

void update_game_tick(cetris_game* g) {
  if (g->game_over) return;

  g->tick++;

  if (g->next_piece_tick && g->tick >= g->next_piece_tick) {
    next_piece(g);
  }
  
  if (g->next_piece_tick) return;

  bool did_move = false;
  if (g->tick >= g->next_drop_tick || !g->next_drop_tick) {
    if (g->next_drop_tick) {
      move_current(g, DOWN);
      did_move = true;
    }

    if (g->level <= 20) {
      g->next_drop_tick = g->tick + level_drop_delay[g->level - 1];
    } else {
      g->next_drop_tick = g->tick + level_drop_delay[19];
    }
  }

  /* lock piece if it was hovering for CETRIS_LOCK_DELAY */
  if (!g->next_piece_tick && g->current.lock_tick && g->current.lock_tick <= g->tick) {
    g->current.pos.y++;
    if (check_matrix(g, &g->current.m) <= 0) {
      lock_current(g);
      did_move = true;
    }
    g->current.pos.y--;
    g->current.lock_tick = 0;
  }

  if (handle_inputs(g)) {
    did_move = true;
  }
  
  if (did_move) update_board(g); 
}

void next_piece(cetris_game* g) {
  g->next_drop_tick = 0;
  g->next_piece_tick = 0;

  g->current = g->piece_queue[g->current_index];
  if (check_matrix(g, &g->current.m) <= 0) {
    g->game_over = true;
  }
  g->current_index++;

  if (!g->game_over) {
    move_current(g, DOWN);
  }

  if (g->current_index >= 7) {
    g->current_index = 0;
    shuffle_queue(g);
  }

  update_board(g);
}

void lock_current(cetris_game* g) {
  g->current.locked = true;
  for (u8 x = 0; x < CETRIS_BOARD_X; x++) { 
    for (u8 y = 0; y < CETRIS_BOARD_Y; y++) {
      if (g->board[x][y].occupied) g->board[x][y].constant = 1;
    }
  }
  update_board(g);
}

void make_ghosts(cetris_game* g) {
  u8 orig_y = g->current.pos.y;
  while (true) {
    g->current.pos.y++;
    if (check_matrix(g, &g->current.m) <= 0) {
      g->current.ghost_y = g->current.pos.y - 1;
      g->current.pos.y = orig_y;
      break;
    }
  }
}

void update_board(cetris_game* g) {
  if (g->game_over) return;

  u8 lines_cleared = 0;
  for (u8 y = 0; y < CETRIS_BOARD_Y; y++) {
    bool clear_line = true;
    for (u8 x = 0; x < CETRIS_BOARD_X; x++) {
      if (!g->board[x][y].constant) { 
        memset(&g->board[x][y], 0, sizeof(slot));
      }

      if (!g->board[x][y].occupied || g->board[0][y].remove_tick > 0) {
        clear_line = false;
      }
    }
    // remove tick only tracked on first block of line
    if (g->board[0][y].remove_tick && g->board[0][y].remove_tick <= g->tick) {
      for (i8 s = y - 1; s >= 0; s--) {
        for (u8 x = 0; x < CETRIS_BOARD_X; x++) {
          g->board[x][s + 1] = g->board[x][s];
        }
      }
    }
    if (clear_line) {
      g->board[0][y].remove_tick = g->tick + CETRIS_LINE_CLEAR_DELAY;
      lines_cleared++;
    }
  }

  make_ghosts(g);
  set_matrix(g, &g->current.m);

  assert(lines_cleared <= 4);

  if (g->current.locked && !g->next_piece_tick) {
    if (lines_cleared > 0) {
      g->next_piece_tick = g->tick + CETRIS_NEXT_PIECE_DELAY;
    } else {
      next_piece(g);
    }
  }

  if (lines_cleared > 0 || g->tspin || g->mini_tspin) {
    add_score(g, lines_cleared);
    if (lines_cleared > 0) {
      g->lines += lines_cleared;
      if (g->lines >= (g->level * 10)) g->level++;
    }
  }
}

/* SCORE FUNCTIONS */

void add_score(cetris_game* g, u8 lines) {
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

/* MOVEMENT FUNCTIONS */

void move_piece(cetris_game* g, input_t move, bool hold) {
  if (!g->held_moves[move]) {
    switch (move) {
      case LEFT:
      case RIGHT:
      case DOWN:
      case USER_DOWN:
        move_current(g, move);
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
  if (hold) g->held_moves[move] = true;
}

void stop_holding(cetris_game* g, input_t move) {
  g->held_moves[move] = false;
  if (move == RIGHT || move == LEFT) {
    g->das_move_tick = 0;
    g->das_repeat = 0;
  } else if (move == USER_DOWN) g->down_move_tick = 0;
}

void reset_tetrimino(tetrimino* t) {
  t->r = INIT;
  t->pos.x = 3;
  t->pos.y = (t->t == I) ? 17 : 16;
  t->ghost_y = 0;
}

void hold_piece(cetris_game* g) {
  if (g->piece_held) {
    tetrimino tmp = g->current;
    g->current = g->held;
    g->held = tmp;
  } else {
    g->held = g->current;
    reset_tetrimino(&g->held);
    g->piece_held = true;
    next_piece(g);
  }
  update_board(g);
}
    

