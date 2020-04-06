#ifndef _CETRIS_H
#define _CETRIS_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef CETRIS_ENABLE_DAS
#define CETRIS_ENABLE_DAS 1
#endif

typedef struct
{
  char x; 
  char y;
} ctrs_vec2;

typedef unsigned char ctrs_matrix[4];

enum 
{
  CTRS_SLOT_OCCUPIED = 1,
};

typedef enum 
{
  MINO_Z,
  MINO_L,
  MINO_O,
  MINO_S,
  MINO_I,
  MINO_J,
  MINO_T
} ctrs_mino_type;

typedef enum
{ 
  INIT, 
  ONCE_RIGHT, 
  TWICE, 
  ONCE_LEFT 
} ctrs_srs;

typedef enum 
{
  DOWN,
  RIGHT,
  LEFT,
  ROTATE_CCW,
  ROTATE_CW,
  HARD_DROP
} ctrs_dir;

typedef struct 
{
  ctrs_vec2 pos;
  ctrs_matrix m;
  ctrs_mino_type t;
  ctrs_srs r;

  bool locked; 
  bool held;

  unsigned long lock_tick;
  unsigned long force_lock_tick;

  char ghost_y;
} ctrs_mino;

typedef struct 
{
  ctrs_vec2 pos;
  ctrs_matrix m;
  ctrs_mino_type t;

  bool processed;
} ctrs_lock_event;

typedef struct ctrs_game ctrs_game;

typedef struct 
{
  unsigned long drop_period;
  unsigned long next_piece_delay;
  unsigned long line_delay_clear;
  unsigned long lock_delay;
  unsigned long force_lock;

#if CETRIS_ENABLE_DAS
  unsigned long das_arr; 
  unsigned long das_das;
#endif
  
  unsigned char board_x;
  unsigned char board_y;
  unsigned char board_visible;

  unsigned char mino_start_x;
  unsigned char mino_start_y;

  unsigned char starting_level;

  bool wait_on_clear;

  unsigned long *levels;

  // variable win condition
  bool (*win_condition)(ctrs_game *);
} ctrs_config;

typedef struct {
  unsigned char move_event;
  unsigned char hold_event;

  unsigned char start_event;

  unsigned char combo_event;
  unsigned char tetris_event;
  unsigned char line_event;

  ctrs_lock_event lock_event;

  bool start_game;
  bool stop_game;

} ctrs_events;

struct ctrs_game {
  // playfield represented by a 2d array
  unsigned char **board;

  // highest y pos of any piece
  int highest_line;

  // queue of all 7 possible tetrimino
  unsigned char piece_queue[7];
  unsigned char next_queue[7];
  unsigned char current_index;

  // current tetrimino
  ctrs_mino current;
  ctrs_mino held;

  bool piece_held;

  // waiting to start
  bool waiting;

  // internal game tick
  unsigned long tick;
  unsigned long next_drop_tick;
  unsigned long next_piece_tick;
  unsigned long down_move_tick;
  unsigned long *line_remove_tick;

#if CETRIS_HI_RES
  // microsecond accuracy timer
  long long timer;
#endif

#if CETRIS_ENABLE_DAS
  unsigned long das_wait;
  unsigned long next_das_move;
  unsigned char das_move;
  unsigned char held_moves[8];
#endif

  unsigned char level;
  unsigned char lines;
  unsigned char line_combo;

  bool game_over;

  ctrs_events events;

  bool tspin;
  bool mini_tspin;

  unsigned short score;

  // keep config at bottom
  ctrs_config config;
};

const ctrs_matrix ctrs_default_matrices[7] =
{
  { 0b0000, 0b1100, 0b0110, 0b0000}, 
  { 0b0000, 0b0010, 0b1110, 0b0000},
  { 0b0000, 0b0110, 0b0110, 0b0000}, 
  { 0b0000, 0b0110, 0b1100, 0b0000},
  { 0b0000, 0b1111, 0b0000, 0b0000}, 
  { 0b0000, 0b1000, 0b1110, 0b0000},
  { 0b0000, 0b0100, 0b1110, 0b0000}
};

// https://tetris.wiki/SRS
static const ctrs_vec2 srs_wall_kicks[8][5] = {
  {{0, 0}, {-1, 0}, {-1, 1},  {0, -2}, {-1, -2}}, // 0->R
  {{0, 0}, {1, 0},  {1, -1},  {0, 2},  {1, 2}},   // R->0
  {{0, 0}, {1, 0},  {1, -1},  {0, 2},  {1, 2}},   // R->2
  {{0, 0}, {-1, 0}, {-1, 1},  {0, -2}, {-1, -2}}, // 2->R
  {{0, 0}, {1, 0},  {1, 1},   {0, -2}, {1, -2}},  // 2->L
  {{0, 0}, {-1, 0}, {-1, -1}, {0, 2},  {-1, 2}},  // L->2
  {{0, 0}, {-1, 0}, {-1, -1}, {0, 2},  {-1, 2}},  // L->0
  {{0, 0}, {1, 0},  {1, 1},   {0, -2}, {1, -2}}   // 0->L
};

static const ctrs_vec2 srs_wall_kicks_i[8][5] = {
  {{0, 0}, {-2, 0}, {1, 0},  {-2, -1}, {1, 2}},   // 0->R
  {{0, 0}, {2, 0},  {-1, 0}, {2, 1},   {-1, -2}}, // R->0
  {{0, 0}, {-1, 0}, {2, 0},  {-1, 2},  {2, -1}},  // R->2
  {{0, 0}, {1, 0},  {-2, 0}, {1, -2},  {-2, 1}},  // 2->R
  {{0, 0}, {2, 0},  {-1, 0}, {2, 1},   {-1, -2}}, // 2->L
  {{0, 0}, {-2, 0}, {1, 0},  {-2, -1}, {1, 2}},   // L->2
  {{0, 0}, {1, 0},  {-2, 0}, {1, -2},  {-2, 1}},  // L->0
  {{0, 0}, {-1, 0}, {2, 0},  {-1, 2},  {2, -1}}   // 0->L
};

static const ctrs_vec2 basic_movements[3] = {
    {0, 1}, {1, 0}, {-1, 0} // DOWN, RIGHT, LEFT
};

static void update_board(ctrs_game *g);

static void 
set_piece(ctrs_game *g, uint8_t type, ctrs_mino* mino)
{
  memset(mino, 0, sizeof(ctrs_mino));

  mino->t = type;
  memcpy(mino->m, ctrs_default_matrices[type], sizeof(ctrs_matrix));

  /* Pieces should spawn so that on the first down
   * tick the bottom row will show. Values here are adjusted
   * for the default 4x4 matricies for each piece */
  mino->pos.x = g->config.mino_start_x;
  mino->pos.y = g->config.mino_start_y;

  if (type == MINO_I) mino->pos.y++;
}

static inline void 
shuffle_queue(ctrs_game *g)
{
  unsigned char i, r, t;
  for (i = 0; i < 7; i++) 
  {
    r = rand() % 7;
    t = g->next_queue[i];

    g->next_queue[i] = g->next_queue[r];
    g->next_queue[r] = t;
  }
}

static inline int 
check_matrix(ctrs_game *g, ctrs_matrix *m) 
{
  ctrs_vec2 r;

  for (int j = 0; j < 4; j++)
  for (int s = 0; s < 4; s++)
  {
    r = (ctrs_vec2){j + g->current.pos.x, s + g->current.pos.y};
    if (r.y < 0) continue;
    if (((*m)[s]>>(3 - j))&1)
    {
      if (r.x >= g->config.board_x || r.x < 0)
        return 0;
      if (r.y >= g->config.board_y)
        return -1;
      if (g->board[r.x][r.y] & CTRS_SLOT_OCCUPIED)
        return -1;
    }
  }

  return 1;
}

// TODO: hard score
static void 
add_score(ctrs_game *g, int lines) 
{
  if (!g->tspin && !g->mini_tspin) 
  {
    switch (lines) 
    {
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
  }
  else if (g->tspin) 
  {
    switch (lines)
    {
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
  } 
  else if (g->mini_tspin) 
  {
    switch (lines)
    {
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

static void 
make_ghosts(ctrs_game *g) 
{
  int orig_y = g->current.pos.y;

  while (check_matrix(g, &g->current.m) > 0) 
    g->current.pos.y++;

  if (g->current.pos.y == orig_y)
    g->current.ghost_y = orig_y;
  else 
    g->current.ghost_y = g->current.pos.y - 1;
 
  g->current.pos.y = orig_y;
}

static void 
move_current(ctrs_game *g, unsigned char move) 
{
  if (g->game_over || g->next_piece_tick)
    return;

  g->current.pos.y += basic_movements[move].y;
  g->current.pos.x += basic_movements[move].x;

  int check = check_matrix(g, &g->current.m);
  if (check <= 0)
  {
    g->current.pos.y -= basic_movements[move].y;
    g->current.pos.x -= basic_movements[move].x;

    if (move == DOWN && check == -1) 
    {
      if (!g->current.force_lock_tick && g->config.force_lock)
        g->current.force_lock_tick = g->tick + g->config.force_lock;
      if (!g->current.lock_tick)
        g->current.lock_tick = g->tick + g->config.lock_delay;
    }
  } 
  else 
  {
    g->events.move_event++;
    if (g->current.lock_tick) 
      g->current.lock_tick = g->tick + g->config.lock_delay;
    if (move == DOWN && g->held_moves[DOWN])
      g->score++;
  }

  update_board(g);
}

static void 
next_piece(ctrs_game *g)
{
  g->next_drop_tick = 0;
  g->next_piece_tick = 0;

  set_piece(g, g->piece_queue[g->current_index], &g->current);

  if (check_matrix(g, &g->current.m) <= 0) 
  {
    g->game_over = true;
    g->events.stop_game = 1;
  }

  if (!g->game_over) 
    move_current(g, DOWN);
  
  g->current_index++;
  if (g->current_index == 7) 
  {
    g->current_index = 0;

    memcpy(&g->piece_queue, &g->next_queue, sizeof(g->piece_queue));
    shuffle_queue(g);
  }

  update_board(g);
}

static void 
lock_current(ctrs_game *g) 
{
  for (int j = 0; j < 4; j++)
  for (int s = 0; s < 4; s++)
  {
    if ((g->current.m[s]>>(3 - j))&1) 
    {
      g->board[g->current.pos.x + j][g->current.pos.y + s] |= CTRS_SLOT_OCCUPIED;
      g->board[g->current.pos.x + j][g->current.pos.y + s] |= g->current.t << 5;
    }
  }

  if (g->current.pos.y < g->highest_line)
    g->highest_line = g->current.pos.y;

  g->events.lock_event.processed = false;
  g->events.lock_event.pos = g->current.pos;
  g->events.lock_event.t = g->current.t;

  memcpy(&g->events.lock_event.m, &g->current.m, sizeof(ctrs_matrix));

  g->current.locked = true;
  update_board(g);
}

static void 
hard_drop(ctrs_game *g) 
{
  if (g->game_over || g->next_piece_tick)
    return;

  int drop_count = 0;
  while (check_matrix(g, &g->current.m) > 0) 
  {
    g->current.pos.y++;
    drop_count++;
  }

  g->current.pos.y--;
  drop_count--;

  g->score += 2 * drop_count; // 2 score for each hard-drop'd cell

  lock_current(g);
}

static inline void 
rotate_matrix(ctrs_game *g, ctrs_matrix *m, bool clockwise) 
{
  for (int j = 0; j < 4; j++)
  for (int s = 0; s < 4; s++)
  {
    if ((g->current.m[s]>>(3 - j))&1) 
    {
      unsigned char new_x = (clockwise) ? 1 - (s - 2) : 1 + (s - 2);
      unsigned char new_y = (clockwise) ? 2 + (j - 1) : 2 - (j - 1);

      if (g->current.t == MINO_I) 
        clockwise ? new_y-- : new_x++;
      
      (*m)[new_y] |= (unsigned char)0b1000 >> (new_x);
    }
  }
}

static void 
rotate_piece(ctrs_game *g, bool clockwise) 
{
  if (g->game_over || g->next_piece_tick)
    return;
  
  if (g->current.t == MINO_O)
    return;
  
  unsigned char next = 0, wall_kick = 0;
  if (clockwise) 
  {
    next = (g->current.r + 1) % 4;
    wall_kick = g->current.r * 2;
  } 
  else 
  {
    next = ((g->current.r - 1) + 4) % 4;
    wall_kick = (next * 2) + 1;
  }

  ctrs_matrix m;
  memset(&m, 0, sizeof(ctrs_matrix));

  rotate_matrix(g, &m, clockwise);

  ctrs_vec2 kick;
  bool set_current = false, did_kick = false;
  for (int i = 0; i < 4; i++)
  {
    if (g->current.t == MINO_I) 
      kick = srs_wall_kicks_i[wall_kick][i];
    else 
      kick = srs_wall_kicks[wall_kick][i];

    g->current.pos.x += kick.x;
    g->current.pos.y -= kick.y;

    if (check_matrix(g, &m) > 0) 
    {
      set_current = true;
      if (i > 0) did_kick = true;
      break;
    }

    g->current.pos.x -= kick.x;
    g->current.pos.y += kick.y;
  }

  if (set_current) 
  {
    /* check for tspin */
    if (g->current.t == MINO_T) 
    {
      bool did_tspin = true;
      for (int i = 1; i < 3; i++) 
      {
        g->current.pos.x += basic_movements[i].x;
        g->current.pos.y += basic_movements[i].y;

        if (check_matrix(g, &m) == 1)
          did_tspin = false;

        g->current.pos.x -= basic_movements[i].x;
        g->current.pos.y -= basic_movements[i].y;
      }

      if (did_tspin) 
      {
        if (did_kick) g->mini_tspin = true;
        else g->tspin = true;
      }
    }

    g->events.move_event++;

    g->current.r = next;
    memcpy(g->current.m, &m, sizeof(ctrs_matrix));

    update_board(g);
  }
}

static inline void 
clear_line(ctrs_game *g, int y) 
{
  for (int s = y - 1; s >= 0; s--)
  for (int x = 0; x < g->config.board_x; x++) 
    g->board[x][s + 1] = g->board[x][s];
}

static void 
update_board(ctrs_game *g)
{
  if (g->game_over)
    return;

  int lines_cleared = 0;
  for (int y = g->highest_line; y < g->config.board_y; y++) 
  {
    bool should_clear_line = true;
    for (int x = 0; x < g->config.board_x; x++) 
    {
      if (!(g->board[x][y] & CTRS_SLOT_OCCUPIED) 
          || g->line_remove_tick[y] > 0) 
        should_clear_line = false;
    }

    if (g->config.wait_on_clear) 
    {
      // remove tick only tracked on first block of line
      if (g->line_remove_tick[y] && g->line_remove_tick[y] <= g->tick) 
      {
        g->line_remove_tick[y] = 0;
        clear_line(g, y);
      }
      if (should_clear_line) 
      {
        g->line_remove_tick[y] = g->tick + g->config.line_delay_clear;
        lines_cleared++;
      }
    } 
    else if (should_clear_line)
    {
      clear_line(g, y); 
      lines_cleared++;
    }
  }

  make_ghosts(g);

  if (g->current.locked && !g->next_piece_tick) 
  {
    if (lines_cleared > 0) 
      g->next_piece_tick = g->tick + g->config.next_piece_delay;
    else 
    {
      next_piece(g);
      g->line_combo = 0;
    }
  }
  
  if (g->tspin || g->mini_tspin) 
    add_score(g, lines_cleared);
  else if (lines_cleared > 0) 
    add_score(g, lines_cleared);

  g->lines += lines_cleared;
  if (lines_cleared > 0) 
  {
    g->events.line_event = lines_cleared;
    if (lines_cleared == 4)
      g->events.tetris_event++;
    else
      g->events.combo_event++;

    g->line_combo++;
  }
  if (g->lines >= (g->level * 10)) g->level++;
}

void 
ctrs_hold_piece(ctrs_game *g) 
{
  if (g->current.held) return;
  if (g->piece_held) 
  {
    unsigned char tmp = g->current.t;
    g->current = g->held;
    set_piece(g, tmp, &g->held);
  }
  else
  {
    set_piece(g, g->current.t, &g->held);
    g->piece_held = true;
    next_piece(g);
  }

  g->current.held = true;
  g->events.hold_event++;

  update_board(g);
}

#if CETRIS_ENABLE_DAS
void 
ctrs_unhold_move(ctrs_game* g, unsigned char move)
{
  if (!g->held_moves[move]) return;

  if (g->das_move == move)
  {
    if (move == LEFT && g->held_moves[RIGHT]) 
    {
      g->das_move = RIGHT;
      g->das_wait = g->tick + g->config.das_das;
    } 
    else if (move == RIGHT && g->held_moves[LEFT])
    {
      g->das_move = LEFT;
      g->das_wait = g->tick + g->config.das_das;
    } 
    else 
      g->das_wait = 0;

    g->next_das_move = 0;
  }

  if (move == DOWN)
    g->next_drop_tick = 0;

  g->held_moves[move] = 0;
}
#endif

void 
ctrs_move_piece(ctrs_game *g, unsigned char move) 
{
#if CETRIS_ENABLE_DAS
  if (g->held_moves[move]) return;
  if (move == LEFT || move == RIGHT) 
  {
    if ((move != g->das_move) || !g->das_wait) 
    {
      g->das_move = move;
      if (!g->waiting) 
      {
        g->das_wait = g->tick + g->config.das_das;
        g->next_das_move = 0;
      }
      else g->next_das_move = 1;
    }
  }

  if (move == DOWN)
    g->next_drop_tick = g->tick + g->config.drop_period;

  g->held_moves[move] = 1;
#endif
  
  if (g->waiting) return;

  switch (move) 
  {
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

void 
ctrs_init_game(ctrs_game *g)
{
  srand(time(NULL));

  // check for config errors
  if (g->config.next_piece_delay < g->config.line_delay_clear)
    g->config.next_piece_delay = g->config.line_delay_clear;

  if (!g->config.wait_on_clear)
    g->config.next_piece_delay = 0;

  memset(g, 0, sizeof(ctrs_game) - sizeof(ctrs_config));

  g->board = calloc(g->config.board_x, sizeof(unsigned char *));
  for (int i = 0; i < g->config.board_x; i++) 
    g->board[i] = calloc(g->config.board_y, sizeof(unsigned char));

  g->line_remove_tick = (unsigned long *)calloc(g->config.board_y, sizeof(unsigned long));

  g->waiting = false;
  g->level = g->config.starting_level;

  g->highest_line = g->config.board_y;

  for (int i = 0; i < 7; i++) 
    g->next_queue[i] = i;

  shuffle_queue(g);
  memcpy(&g->piece_queue, &g->next_queue, sizeof(g->piece_queue));
  shuffle_queue(g);

  next_piece(g);
}

bool 
ctrs_update_game_tick(ctrs_game *g)
{
  if (g->game_over)
    return false;

  if (g->next_piece_tick && g->tick >= g->next_piece_tick)
    next_piece(g);

  if (g->next_piece_tick)
    return true;

  bool did_move = false;
  if (g->next_drop_tick && g->tick >= g->next_drop_tick) 
  {
    move_current(g, DOWN);
    g->next_drop_tick = 0;
    did_move = true;
  }

  if (!g->next_drop_tick) 
  {
    if (g->held_moves[DOWN]) 
    {
      g->next_drop_tick = g->tick + g->config.drop_period;
    }
    else 
    {
      if (g->level <= 20) 
        g->next_drop_tick = g->tick + g->config.levels[g->level - 1];
      else 
        g->next_drop_tick = g->tick + g->config.levels[19];
    }
  }

  /* lock piece if it was hovering for CETRIS_LOCK_DELAY */
  if (((g->current.lock_tick && g->current.lock_tick <= g->tick) 
	|| (g->current.force_lock_tick && g->current.force_lock_tick <= g->tick))) 
  {
    g->current.pos.y++;
    char res = check_matrix(g, &g->current.m);
    g->current.pos.y--;

    if (res <= 0)
    {
      lock_current(g);
      did_move = true;
    }

    g->current.lock_tick = 0;
  }

#if CETRIS_ENABLE_DAS
  if ((g->next_das_move && g->tick >= g->next_das_move) || g->next_das_move == 1) 
  {
    if (!g->waiting) 
      move_current(g, g->das_move);

    g->next_das_move = g->tick + g->config.das_arr;
  }
  else if (!g->next_das_move && g->das_wait && g->tick >= g->das_wait) 
    g->next_das_move = g->tick + g->config.das_arr;
#endif

  if (did_move) update_board(g);

  if (g->config.win_condition(g))
    g->game_over = true;

  return true;
}

#endif // _CETRIS_H
