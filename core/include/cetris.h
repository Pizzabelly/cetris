#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "types.h"

#define CETRIS_BOARD_X 10
#define CETRIS_BOARD_Y 43 
#define CETRIS_BOARD_VISABLE 23 

#define CETRIS_HZ 60
#define CETRIS_DAS_DELAY 11 
#define CETRIS_DAS_PERIOD 3
#define CETRIS_DROP_PERIOD 2
#define CETRIS_LINE_CLEAR_DELAY 40
#define CETRIS_WAIT_ON_CLEAR 0

typedef enum {
  O, I, S, Z, L, J, T
} type;

typedef enum {
  COLOR_NONE,
  COLOR_O,
  COLOR_I,
  COLOR_S,
  COLOR_Z,
  COLOR_L,
  COLOR_J,
  COLOR_T
} color;

typedef enum {
  INIT,
  ONCE_RIGHT,
  ONCE_LEFT,
  TWICE 
} rstate;

struct tetrimino {
  type t;
  rstate r;
  color c;
  piece_matrix m;
  vec2 pos;
  u32 lock_tick;
};

typedef struct {
  bool occupied;
  bool constant;
  u32 remove_tick;
  color c;
} slot;

struct cetris_game {
  /* playfield represented by a 2d array */
  slot board[CETRIS_BOARD_X][CETRIS_BOARD_Y];

  /* constant queue of all 7 possible tetrimino */
  struct tetrimino piece_queue[7];

  /* current tetrimino */
  struct tetrimino current;
  u8 current_index;

  /* input_manager */
  bool held_moves[7];
  input_t prev_das_move;
  u8 das_repeat;
  u32 das_move_tick;
  u32 down_move_tick;

  /* internal game tick */
  u32 tick;
  u32 next_drop_tick;

  /* progress trackers */
  u32 lines;
  u32 level;
  bool game_over;

  /* scoring flags */
  bool tspin;
  bool mini_tspin;

  /* score counter  */
  u64 score;
};

void next_piece(struct cetris_game* g);
void wipe_board(struct cetris_game* g);

/* API PROTOTYPES FUNCTIONS */

void init_game(struct cetris_game* g);
void update_game_tick(struct cetris_game* g);
void move_peice(struct cetris_game* g, input_t move);
void stop_holding(struct cetris_game* g, input_t move);
