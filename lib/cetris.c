#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cetris.h"

#ifdef BUILD_TESTS
#include "test.h"
#endif

/* DEFAULT TETRIMINO */

CETRIS_EXPORT const piece_matrix default_matrices[7] = {
    { 0b0000, 0b0110, 0b0110, 0b0000},
    { 0b0000, 0b1111, 0b0000, 0b0000},
    { 0b0000, 0b0110, 0b1100, 0b0000},
    { 0b0000, 0b1100, 0b0110, 0b0000},
    { 0b0000, 0b0010, 0b1110, 0b0000},
    { 0b0000, 0b1000, 0b1110, 0b0000},
    { 0b0000, 0b0100, 0b1110, 0b0000}};

/* SRS WALL KICK VALUES */

// https://tetris.wiki/SRS
static const vec2 srs_wall_kicks[8][5] = {
    {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}}, // 0->R
    {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},     // R->0
    {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},     // R->2
    {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}}, // 2->R
    {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}},    // 2->L
    {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},  // L->2
    {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},  // L->0
    {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}}     // 0->L
};

static const vec2 srs_wall_kicks_i[8][5] = {
    {{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}}, // 0->R
    {{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}}, // R->0
    {{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}}, // R->2
    {{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}}, // 2->R
    {{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}}, // 2->L
    {{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}}, // L->2
    {{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}}, // L->0
    {{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}}  // 0->L
};

static const vec2 basic_movements[5] = {
    {0, 1}, // DOWN
    {1, 0}, // RIGHT
    {-1, 0} // LEFT
};

static void update_board(cetris_game *g);

static void set_piece(cetris_game *g, uint8_t type, tetrimino* mino) {
  memset(mino, 0, sizeof(tetrimino));

  mino->t = type;
  memcpy(mino->m, default_matrices[type], sizeof(piece_matrix));

  /* Pieces should spawn so that on the first down
   * tick the bottom row will show. Values here are adjusted
   * for the default 4x4 matricies for each piece */
  mino->pos.x = g->config.mino_start_x;
  mino->pos.y = g->config.mino_start_y;
  if (type == MINO_I) mino->pos.y++;
}

static void shuffle_queue(cetris_game *g) {
  for (int i = 0; i < 7; i++) {
    uint8_t rand_index = rand() % 7;
    uint8_t tmp = g->next_queue[i];
    g->next_queue[i] = g->next_queue[rand_index];
    g->next_queue[rand_index] = tmp;
  }
}

static int check_matrix(cetris_game *g, piece_matrix *m) {
  for (uint8_t y = 0; y < 4; y++) {
    for (uint8_t x = 0; x < 4; x++) {
      vec2 r = (vec2){x + g->current.pos.x, y + g->current.pos.y};
      if (r.y < 0)
        continue;
      if (((*m)[y]>>(3 - x))&1) {
        if (r.x >= g->config.board_x || r.x < 0)
          return 0;
        if (r.y >= g->config.board_y)
          return -1;
        if (g->board[r.x][r.y] & SLOT_OCCUPIED)
          return -1;
      }
    }
  }
  return 1;
}

// TODO: hard score
static void add_score(cetris_game *g, int lines) {
  if (!g->tspin && !g->mini_tspin) {
    switch (lines) {
    case 1:
      g->score += 100 * g->level;
      break;
    case 2:
      g->score += 300 * g->level;
      break;
    case 3:
      g->score += 500 * g->level;
      break;
    case 4:
      g->score += 800 * g->level;
      break;
    }
  } else if (g->tspin) {
    switch (lines) {
    case 0:
      g->score += 400 * g->level;
      break;
    case 1:
      g->score += 800 * g->level;
      break;
    case 2:
      g->score += 1200 * g->level;
      break;
    case 3:
      g->score += 1600 * g->level;
      break;
    }
    g->tspin = false;
  } else if (g->mini_tspin) {
    switch (lines) {
    case 0:
      g->score += 100 * g->level;
      break;
    case 1:
      g->score += 200 * g->level;
      break;
    case 2:
      g->score += 400 * g->level;
      break;
    }
    g->mini_tspin = false;
  }
}

static void make_ghosts(cetris_game *g) {
  int8_t orig_y = g->current.pos.y;
  while (check_matrix(g, &g->current.m) > 0) {
    g->current.pos.y++;
  }
  if (g->current.pos.y == orig_y) {
    g->current.ghost_y = orig_y;
  } else {
    g->current.ghost_y = g->current.pos.y - 1;
  }
  g->current.pos.y = orig_y;
}

static void move_current(cetris_game *g, uint8_t move) {
  if (g->game_over || g->next_piece_tick)
    return;

  g->current.pos.y += basic_movements[move].y;
  g->current.pos.x += basic_movements[move].x;

  int check = check_matrix(g, &g->current.m);
  if (check <= 0) {
    g->current.pos.y -= basic_movements[move].y;
    g->current.pos.x -= basic_movements[move].x;

    if (move == DOWN && check == -1) {
      if (!g->current.force_lock_tick && g->config.force_lock)
        g->current.force_lock_tick = g->tick + g->config.force_lock;
      if (!g->current.lock_tick)
        g->current.lock_tick = g->tick + g->config.lock_delay;
    }
  } else {
    if (g->current.lock_tick) 
      g->current.lock_tick = g->tick + g->config.lock_delay;
    if (move == DOWN && g->held_moves[DOWN])
      g->score++;
  }

  update_board(g);
}

static void next_piece(cetris_game *g) {
  g->next_drop_tick = 0;
  g->next_piece_tick = 0;

  set_piece(g, g->piece_queue[g->current_index], &g->current);

  if (check_matrix(g, &g->current.m) <= 0) {
    g->game_over = true;
  }

  if (!g->game_over) {
    move_current(g, DOWN);
  }
  
  g->current_index++;
  if (g->current_index == 7) {
    memcpy(&g->piece_queue, &g->next_queue, sizeof(g->piece_queue));
    g->current_index = 0;
    shuffle_queue(g);
  }

  update_board(g);
}

static void lock_current(cetris_game *g) {
  g->current.locked = true;
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if ((g->current.m[y]>>(3 - x))&1) {
        g->board[g->current.pos.x + x][g->current.pos.y + y] |= SLOT_OCCUPIED;
        g->board[g->current.pos.x + x][g->current.pos.y + y] |= g->current.t << 5;
      }
    }
  }

  if (g->current.pos.y < g->highest_line) {
    g->highest_line = g->current.pos.y;
  }

  update_board(g);
}

static void hard_drop(cetris_game *g) {
  if (g->game_over || g->next_piece_tick)
    return;

  int drop_count = 0;
  while (check_matrix(g, &g->current.m) > 0) {
    g->current.pos.y++;
    drop_count++;
  }
  g->current.pos.y--;
  drop_count--;

  g->score += 2 * drop_count; // 2 score for each hard-drop'd cell

  lock_current(g);
  update_board(g);
}

static void rotate_matrix(cetris_game *g, piece_matrix *m, bool clockwise) {
  for (uint8_t x = 0; x < 4; x++) {
    for (uint8_t y = 0; y < 4; y++) {
      if ((g->current.m[y]>>(3 - x))&1) {
        uint8_t new_x = (clockwise) ? 1 - (y - 2) : 1 + (y - 2);
        uint8_t new_y = (clockwise) ? 2 + (x - 1) : 2 - (x - 1);

        if (g->current.t == MINO_I) {
          clockwise ? new_y-- : new_x++;
        }
        
        (*m)[new_y] |= (uint8_t)0b1000 >> (new_x);
      }
    }
  }
}

static void rotate_piece(cetris_game *g, bool clockwise) {
  if (g->game_over || g->next_piece_tick)
    return;
  if (g->current.t == MINO_O)
    return;
  
  uint8_t next = 0;
  uint8_t wall_kick = 0;
  if (clockwise) {
    next = (g->current.r + 1)%4;
    wall_kick = g->current.r * 2;
  } else {
    next = ((g->current.r - 1) + 4)%4;
    wall_kick = (next * 2) + 1;
  }

  piece_matrix m;
  memset(&m, 0, sizeof(piece_matrix));

  rotate_matrix(g, &m, clockwise);

  vec2 kick;
  bool set_current = false;
  bool did_kick = false;
  for (int i = 0; i < 4; i++) {
    if (g->current.t == MINO_I) {
      kick = srs_wall_kicks_i[wall_kick][i];
    } else {
      kick = srs_wall_kicks[wall_kick][i];
    }

    g->current.pos.x += kick.x;
    g->current.pos.y -= kick.y;

    if (check_matrix(g, &m) > 0) {
      set_current = true;
      if (i > 0) did_kick = true;
      break;
    }

    g->current.pos.x -= kick.x;
    g->current.pos.y += kick.y;
  }

  if (set_current) {
    /* check for tspin */
    if (g->current.t == MINO_T) {
      bool did_tspin = true;
      for (int i = 1; i < 5; i++) {
        g->current.pos.x += basic_movements[i].x;
        g->current.pos.y += basic_movements[i].y;

        if (check_matrix(g, &m) == 1)
          did_tspin = false;

        g->current.pos.x -= basic_movements[i].x;
        g->current.pos.y -= basic_movements[i].y;
      }

      if (did_tspin) {
        if (did_kick) g->mini_tspin = true;
        else g->tspin = true;
      }
    }

    g->current.r = next;
    memcpy(g->current.m, &m, sizeof(piece_matrix));
    update_board(g);
  }
}

void update_board(cetris_game *g) {
  if (g->game_over)
    return;

  int lines_cleared = 0;
  for (int y = g->highest_line; y < g->config.board_y; y++) {
    bool clear_line = true;
    for (int x = 0; x < g->config.board_x; x++) {
      if (!(g->board[x][y] & SLOT_OCCUPIED) 
          || g->line_remove_tick[y] > 0) {
        clear_line = false;
      }
    }

    if (g->config.wait_on_clear) {
      // remove tick only tracked on first block of line
      if (g->line_remove_tick[y] && g->line_remove_tick[y] <= g->tick) {
        g->line_remove_tick[y] = 0;
        for (int s = y - 1; s >= 0; s--) {
          for (int x = 0; x < g->config.board_x; x++) {
            g->board[x][s + 1] = g->board[x][s];
          }
        }
      }
      if (clear_line) {
        g->line_remove_tick[y] = g->tick + g->config.line_delay_clear;
        lines_cleared++;
      }
    } else if (clear_line) {
      for (int s = y - 1; s >= 0; s--) {
        for (int x = 0; x < g->config.board_x; x++) {
          g->board[x][s + 1] = g->board[x][s];
        }
      }
      lines_cleared++;
    }
  }

  make_ghosts(g);

  if (g->current.locked && !g->next_piece_tick) {
    if (lines_cleared > 0) {
      g->next_piece_tick = g->tick + g->config.next_piece_delay;
    } else {
      next_piece(g);
      g->line_combo = 0;
    }
  }
  
  if (g->tspin || g->mini_tspin) {
    add_score(g, lines_cleared);
  } else if (lines_cleared > 0) {
    add_score(g, lines_cleared);
  }

  g->lines += lines_cleared;
  if (lines_cleared > 0) {
    g->line_combo++;
    g->line_event = true;
  }
  if (g->lines >= (g->level * 10)) {
    g->level++;
  }
}

CETRIS_EXPORT void hold_piece(cetris_game *g) {
  if (g->current.held) return;
  if (g->piece_held) {
    uint8_t tmp = g->current.t;
    g->current = g->held;
    set_piece(g, tmp, &g->held);
  } else {
    set_piece(g, g->current.t, &g->held);
    g->piece_held = true;
    next_piece(g);
  }
  g->current.held = true;
  update_board(g);
}

#if CETRIS_ENABLE_DAS
CETRIS_EXPORT void unhold_move(cetris_game* g, uint8_t move) {
  if (g->das_move == move) {
    if (move == LEFT && g->held_moves[RIGHT]) {
      g->das_move = RIGHT;
      g->das_wait = g->tick + g->config.das_das;
    } else if (move == RIGHT && g->held_moves[LEFT]) {
      g->das_move = LEFT;
      g->das_wait = g->tick + g->config.das_das;
    } else {
      g->das_wait = 0;
    }
    g->next_das_move = 0;
  }
  if (move == DOWN) g->next_drop_tick = 0;
  g->held_moves[move] = 0;
}
#endif

CETRIS_EXPORT void move_piece(cetris_game *g, uint8_t move) {
#if CETRIS_ENABLE_DAS
  if (g->held_moves[move]) return;
  if (move == LEFT || move == RIGHT) {
    if ((move != g->das_move) || !g->das_wait) {
      g->das_move = move;
      if (!g->waiting) {
        g->das_wait = g->tick + g->config.das_das;
        g->next_das_move = 0;
      }
      else g->next_das_move = 1;
    }
  }
  if (move == DOWN) g->next_drop_tick = g->tick + g->config.drop_period;
  g->held_moves[move] = 1;
#endif
  
  if (g->waiting) return;

  switch (move) {
  case LEFT:
  case RIGHT:
  case DOWN:
    move_current(g, move);
    break;
  case HARD_DROP:
    hard_drop(g);
    break;
  case ROTATE_CW:
    rotate_piece(g, 1);
    break;
  case ROTATE_CCW:
    rotate_piece(g, 0);
    break;
  }

}

CETRIS_EXPORT void init_game(cetris_game *g, cetris_config* c) {
    srand(time(NULL));

#ifdef BUILD_TESTS
  //apply_test_board(g, TSPIN_NO_LINES);
#endif

  cetris_config config;
  if (!c) {
    config = g->config;
  } else {
    config = *c;
  }

  // check for config errorsa
  if (config.next_piece_delay < config.line_delay_clear) {
    config.next_piece_delay = config.line_delay_clear;
  }

  if (!config.wait_on_clear) {
    config.next_piece_delay = 0;
  }

  memset(g, 0, sizeof(cetris_game));

  memcpy(&g->config, &config, sizeof(cetris_config));

  g->board = (uint8_t **)malloc(sizeof(uint8_t *) * config.board_x);
  for (int i = 0; i < config.board_x; i++) {
    g->board[i] = (uint8_t *)malloc(sizeof(uint8_t) * config.board_y);
    memset(g->board[i], 0, sizeof(uint8_t) * config.board_y);
  }

  g->line_remove_tick = (ctick *)malloc(sizeof(ctick) * config.board_y);
  memset(g->line_remove_tick, 0, sizeof(ctick) * config.board_y);

  g->level = config.starting_level;
  g->waiting = true;

  g->highest_line = config.board_y;

  for (int i = 0; i < 7; i++) {
    g->next_queue[i] = i;
  }

  shuffle_queue(g);
  memcpy(&g->piece_queue, &g->next_queue, sizeof(g->piece_queue));
  shuffle_queue(g);

  next_piece(g);
}

CETRIS_EXPORT bool update_game_tick(cetris_game *g) {
  if (g->game_over)
    return false;

  if (g->next_piece_tick && g->tick >= g->next_piece_tick) {
    next_piece(g);
  }

  if (g->next_piece_tick)
    return true;

  bool did_move = false;
  if (g->next_drop_tick && g->tick >= g->next_drop_tick) {
    move_current(g, DOWN);
    g->next_drop_tick = 0;
    did_move = true;
  }

  if (!g->next_drop_tick) {
    if (g->held_moves[DOWN]) {
      g->next_drop_tick = g->tick + g->config.drop_period;
    } else {
      if (g->level <= 20) {
        g->next_drop_tick = g->tick + g->config.levels[g->level - 1];
      } else {
        g->next_drop_tick = g->tick + g->config.levels[19];
      }
    }
  }

  /* lock piece if it was hovering for CETRIS_LOCK_DELAY */
  if (!g->next_piece_tick && ((g->current.lock_tick && g->current.lock_tick <= g->tick) 
	|| (g->current.force_lock_tick && g->current.force_lock_tick <= g->tick))) {
    g->current.pos.y++;
    int8_t res = check_matrix(g, &g->current.m);
    g->current.pos.y--;
    if (res <= 0) {
      lock_current(g);
      did_move = true;
    }
    g->current.lock_tick = 0;
  }

#if CETRIS_ENABLE_DAS
  if ((g->next_das_move && g->tick >= g->next_das_move) || g->next_das_move == 1) {
    if (!g->waiting) move_current(g, g->das_move);
    g->next_das_move = g->tick + g->config.das_arr;
  } else if (!g->next_das_move && g->das_wait && g->tick >= g->das_wait) {
    g->next_das_move = g->tick + g->config.das_arr;
  }
#endif

  if (did_move) update_board(g);

  if (g->config.win_condition(g)) g->game_over = true;

  return true;
}
