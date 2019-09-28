#ifndef CETRIS_H
#define CETRIS_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef BUILD_TESTS
#include "test.h"
#endif

#define CETRIS_EXPORT

#define CETRIS_BOARD_X 10
#define CETRIS_BOARD_Y 40
#define CETRIS_BOARD_VISABLE 20

#define CETRIS_HZ 60
#define CETRIS_DROP_PERIOD 2
#define CETRIS_NEXT_PIECE_DELAY 40
#define CETRIS_LINE_CLEAR_DELAY 40
#define CETRIS_LOCK_DELAY 30
#define CETRIS_WAIT_ON_CLEAR 0

#define CETRIS_STARTING_LEVEL 1

typedef struct {
  int x;
  int y;
} vec2;

typedef int piece_matrix[4][4];

typedef enum { O, I, S, Z, L, J, T } type;

typedef enum {
  COLOR_NONE,
  COLOR_O, // yellow
  COLOR_I, // cyan
  COLOR_S, // green
  COLOR_Z, // red
  COLOR_L, // orange
  COLOR_J, // blue
  COLOR_T  // purple
} color;

typedef enum { INIT, ONCE_RIGHT, ONCE_LEFT, TWICE } rstate;

typedef struct {
  type t;
  rstate r;
  color c;
  piece_matrix m;
  int ghost_y;
  vec2 pos;
  int lock_tick;
  bool locked;
} tetrimino;

typedef struct {
  bool occupied;
  bool ghost;
  bool constant;
  int remove_tick;
  color c;
} slot;

typedef enum {
  DOWN = 1,
  USER_DOWN = 2,
  RIGHT = 3,
  LEFT = 4,
  ROTATE_CCW = 5,
  ROTATE_CW = 6,
  HARD_DROP = 7
} input_t;

typedef struct {
  /* playfield represented by a 2d array */
  slot board[CETRIS_BOARD_X][CETRIS_BOARD_Y];

  /* constant queue of all 7 possible tetrimino */
  tetrimino piece_queue[7];

  /* current tetrimino */
  tetrimino current;
  tetrimino held;
  bool piece_held;
  int current_index;

  /* internal game tick */
  int tick;
  int next_drop_tick;
  int next_piece_tick;
  int down_move_tick;

  /* progress trackers */
  int lines;
  int level;
  bool game_over;

  /* scoring flags */
  bool tspin;
  bool mini_tspin;

  /* score counter  */
  int score;
} cetris_game;

/* PROTOTYPES */

static void next_piece(cetris_game *g);
static void update_board(cetris_game *g);
static void lock_current(cetris_game *g);
static void move_current(cetris_game *g, input_t move);
static void hard_drop(cetris_game *g);
static void rotate_piece(cetris_game *g, bool clockwise);
static void init_piece_queue(cetris_game *g);
static void shuffle_queue(cetris_game *g);
static void make_ghosts(cetris_game *g);
static void add_score(cetris_game *g, int lines);
static void reset_tetrimino(tetrimino *t);
static int check_matrix(cetris_game *g, piece_matrix *m);
static void set_matrix(cetris_game *g, piece_matrix *m);
static void rotate_matrix(cetris_game *g, piece_matrix *m, bool clockwise);

static piece_matrix default_matrices[7] = {
    {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},

    {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},

    {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},

    {{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},

    {{0, 0, 0, 0}, {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},

    {{0, 0, 0, 0}, {1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},

    {{0, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}}};

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
    {0, 0},
    {0, 1},
    {0, 1},
    {1, 0},
    {-1, 0} // NONE, DOWN, USER_DOWN, RIGHT, LEFT
};

// https://tetris.fandom.com/wiki/Tetris_Worlds
// TODO: Make this more accurate
static const int level_drop_delay[20] = {60, 48, 37, 28, 21, 16, 11, 8, 6, 4,
                                         3,  2,  1,  1,  1,  1,  1,  1, 1, 1};

/* GAME FUNCTIONS */

CETRIS_EXPORT void init_game(cetris_game *g) {

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

void init_piece_queue(cetris_game *g) {
  for (int i = 0; i < 7; i++) {
    g->piece_queue[i].t = i;
    g->piece_queue[i].c = i + 1;
    g->piece_queue[i].r = INIT;
    g->piece_queue[i].lock_tick = 0;
    g->piece_queue[i].locked = false;
    g->piece_queue[i].ghost_y = 0;
    memcpy(g->piece_queue[i].m, default_matrices[i], sizeof(piece_matrix));

    /* Pieces should spawn so that on the first down
     * tick the bottom row will show. Values here are adjusted
     * for the default 4x4 matricies for each piece */
    g->piece_queue[i].pos.x = 3;
    g->piece_queue[i].pos.y = (i == I) ? 17 : 16;
  }
}

void shuffle_queue(cetris_game *g) {
  for (int i = 0; i < 7; i++) {
    tetrimino t = g->piece_queue[i];
    int rand_index = rand() % 7;
    g->piece_queue[i] = g->piece_queue[rand_index];
    g->piece_queue[rand_index] = t;
  }
}

CETRIS_EXPORT void update_game_tick(cetris_game *g) {
  if (g->game_over)
    return;

  g->tick++;

  if (g->next_piece_tick && g->tick >= g->next_piece_tick) {
    next_piece(g);
  }

  if (g->next_piece_tick)
    return;

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
  if (!g->next_piece_tick && g->current.lock_tick &&
      g->current.lock_tick <= g->tick) {
    g->current.pos.y++;
    if (check_matrix(g, &g->current.m) <= 0) {
      lock_current(g);
      did_move = true;
    }
    g->current.pos.y--;
    g->current.lock_tick = 0;
  }

  if (did_move)
    update_board(g);
}

void next_piece(cetris_game *g) {
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

void lock_current(cetris_game *g) {
  g->current.locked = true;
  for (int x = 0; x < CETRIS_BOARD_X; x++) {
    for (int y = 0; y < CETRIS_BOARD_Y; y++) {
      if (g->board[x][y].occupied)
        g->board[x][y].constant = 1;
    }
  }
  update_board(g);
}

void make_ghosts(cetris_game *g) {
  int orig_y = g->current.pos.y;
  while (true) {
    g->current.pos.y++;
    if (check_matrix(g, &g->current.m) <= 0) {
      g->current.ghost_y = g->current.pos.y - 1;
      g->current.pos.y = orig_y;
      break;
    }
  }
}

void update_board(cetris_game *g) {
  if (g->game_over)
    return;

  int lines_cleared = 0;
  for (int y = 0; y < CETRIS_BOARD_Y; y++) {
    bool clear_line = true;
    for (int x = 0; x < CETRIS_BOARD_X; x++) {
      if (!g->board[x][y].constant) {
        memset(&g->board[x][y], 0, sizeof(slot));
      }

      if (!g->board[x][y].occupied || g->board[0][y].remove_tick > 0) {
        clear_line = false;
      }
    }
    // remove tick only tracked on first block of line
    if (g->board[0][y].remove_tick && g->board[0][y].remove_tick <= g->tick) {
      for (int s = y - 1; s >= 0; s--) {
        for (int x = 0; x < CETRIS_BOARD_X; x++) {
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
      if (g->lines >= (g->level * 10))
        g->level++;
    }
  }
}

/* SCORE FUNCTIONS */

// TODO: hard score
void add_score(cetris_game *g, int lines) {
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

/* MOVEMENT FUNCTIONS */

CETRIS_EXPORT void move_piece(cetris_game *g, input_t move) {
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
    rotate_piece(g, 1);
    break;
  case ROTATE_CCW:
    rotate_piece(g, 0);
    break;
  }
}

void reset_tetrimino(tetrimino *t) {
  t->r = INIT;
  t->pos.x = 3;
  t->pos.y = (t->t == I) ? 17 : 16;
  t->ghost_y = 0;
}

CETRIS_EXPORT void hold_piece(cetris_game *g) {
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

void move_current(cetris_game *g, input_t move) {
  if (g->game_over || g->next_piece_tick)
    return;

  g->current.pos.y += basic_movements[move].y;
  g->current.pos.x += basic_movements[move].x;

  int check = check_matrix(g, &g->current.m);
  if (check <= 0) {
    g->current.pos.y -= basic_movements[move].y;
    g->current.pos.x -= basic_movements[move].x;

    if (move == DOWN && check == -1 && !g->current.lock_tick) {
      g->current.lock_tick = g->tick + CETRIS_LOCK_DELAY;
    }
  } else {
    if (move == USER_DOWN)
      g->score++;
    if ((move == DOWN) | (move == USER_DOWN))
      g->current.lock_tick = 0;
  }

  update_board(g);
}

void hard_drop(cetris_game *g) {
  if (g->game_over || g->next_piece_tick)
    return;

  int drop_count = 0;
  while (true) {
    g->current.pos.y++;
    drop_count++;
    if (check_matrix(g, &g->current.m) <= 0) {
      g->current.pos.y--;
      drop_count--;
      break;
    }
  }

  g->score += 2 * drop_count; // 2 score for each hard-drop'd cell

  update_board(g);
  lock_current(g);
}

void rotate_piece(cetris_game *g, bool clockwise) {
  if (g->game_over || g->next_piece_tick)
    return;
  if (g->current.t == O)
    return;

  rstate next = 0;
  int wall_kick = 0;
  switch (g->current.r) {
  case INIT:
    if (clockwise) {
      next = ONCE_RIGHT;
      wall_kick = 0;
    } else {
      next = ONCE_LEFT;
      wall_kick = 7;
    }
    break;
  case ONCE_RIGHT:
    if (clockwise) {
      next = TWICE;
      wall_kick = 2;
    } else {
      next = INIT;
      wall_kick = 1;
    }
    break;
  case ONCE_LEFT:
    if (clockwise) {
      next = INIT;
      wall_kick = 6;
    } else {
      next = TWICE;
      wall_kick = 5;
    }
    break;
  case TWICE:
    if (clockwise) {
      next = ONCE_LEFT;
      wall_kick = 4;
    } else {
      next = ONCE_RIGHT;
      wall_kick = 3;
    }
    break;
  }

  piece_matrix m;
  memset(m, 0, sizeof(piece_matrix));

  rotate_matrix(g, &m, clockwise);

  vec2 kick;
  bool set_current = false;
  bool did_kick = false;
  for (int i = 0; i < 5; i++) {
    if (g->current.t == I) {
      kick = srs_wall_kicks_i[wall_kick][i];
    } else {
      kick = srs_wall_kicks[wall_kick][i];
    }

    g->current.pos.x += kick.x;
    g->current.pos.y += kick.y;

    if (check_matrix(g, &m) > 0) {
      set_current = true;
      if (i > 0)
        did_kick = true;
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
      for (int i = 1; i < 5; i++) {
        g->current.pos.x += basic_movements[i].x;
        g->current.pos.y += basic_movements[i].y;

        if (check_matrix(g, &m) == 1)
          did_tspin = false;

        g->current.pos.x -= basic_movements[i].x;
        g->current.pos.y -= basic_movements[i].y;
      }

      if (did_tspin) {
        if (did_kick)
          g->mini_tspin = true;
        else
          g->tspin = true;
      }
    }

    g->current.r = next;
    memcpy(g->current.m, m, sizeof(piece_matrix));
    update_board(g);
  }
}

/* FUNCTIONS FOR MATRIX INTERATIONS */

int check_matrix(cetris_game *g, piece_matrix *m) {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      vec2 r = (vec2){x + g->current.pos.x, y + g->current.pos.y};
      if (r.y < 0)
        continue;
      if ((*m)[y][x]) {
        if (r.x >= CETRIS_BOARD_X || r.x < 0)
          return 0;
        if (r.y >= CETRIS_BOARD_Y)
          return -1;
        if (g->board[r.x][r.y].occupied && g->board[r.x][r.y].constant)
          return -1;
      }
    }
  }
  return 1;
}

void set_matrix(cetris_game *g, piece_matrix *m) {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if ((*m)[y][x]) {
        vec2 r = (vec2){x + g->current.pos.x, y + g->current.pos.y};
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

void rotate_matrix(cetris_game *g, piece_matrix *m, bool clockwise) {
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      if (g->current.m[y][x]) {
        int new_x = (clockwise) ? 1 - (y - 2) : 1 + (y - 2);
        int new_y = (clockwise) ? 2 + (x - 1) : 2 - (x - 1);

        if (g->current.t == I) {
          if (clockwise)
            new_y--;
          else
            new_x++;
        }

        (*m)[new_y][new_x] = 1;
      }
    }
  }
}

#endif /* CETRIS_H */