#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "types.h"

#define CETRIS_BOARD_X 10
#define CETRIS_BOARD_Y 40 
#define CETRIS_BOARD_VISABLE 20 

#define CETRIS_HZ 60
#define CETRIS_DAS_DELAY 11 
#define CETRIS_DAS_PERIOD 3
#define CETRIS_DROP_PERIOD 2
#define CETRIS_NEXT_PIECE_DELAY 40
#define CETRIS_LINE_CLEAR_DELAY 40
#define CETRIS_LOCK_DELAY 30 
#define CETRIS_WAIT_ON_CLEAR 0

#define CETRIS_STARTING_LEVEL 1

typedef u8 piece_matrix[4][4];

typedef enum {
  O, I, S, Z, L, J, T
} type;

typedef enum {
  COLOR_NONE,
  COLOR_O, // yellow
  COLOR_I, // cyan
  COLOR_S, // green
  COLOR_Z, // red
  COLOR_L, // orange
  COLOR_J, // blue
  COLOR_T // purple
} color;

typedef enum {
  INIT,
  ONCE_RIGHT,
  ONCE_LEFT,
  TWICE 
} rstate;

typedef struct {
  type t;
  rstate r;
  color c;
  piece_matrix m;
  i8 ghost_y;
  vec2 pos;
  u32 lock_tick;
  bool locked;
} tetrimino;

typedef struct {
  bool occupied;
  bool ghost;
  bool constant;
  u32 remove_tick;
  color c;
} slot;

typedef struct {
  /* playfield represented by a 2d array */
  slot board[CETRIS_BOARD_X][CETRIS_BOARD_Y];

  /* constant queue of all 7 possible tetrimino */
  tetrimino piece_queue[7];

  /* current tetrimino */
  tetrimino current;
  tetrimino held;
  bool piece_held;
  u8 current_index;

  /* internal game tick */
  u32 tick;
  u32 next_drop_tick;
  u32 next_piece_tick;
  u32 down_move_tick;

  /* progress trackers */
  u32 lines;
  u32 level;
  bool game_over;

  /* scoring flags */
  bool tspin;
  bool mini_tspin;

  /* score counter  */
  u32 score;
} cetris_game;

typedef enum {
  DOWN       = 1,
  USER_DOWN  = 2,
  RIGHT      = 3,
  LEFT       = 4,
  ROTATE_CCW = 5,
  ROTATE_CW  = 6,
  HARD_DROP  = 7 
} input_t;

/* API FUNCTIONS */
void init_game(cetris_game* g);
void update_game_tick(cetris_game* g);
void move_piece(cetris_game* g, input_t move);
void hold_piece(cetris_game* g);
