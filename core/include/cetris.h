#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "types.h"

#define CETRIS_BOARD_X 10
#define CETRIS_BOARD_Y 20
#define CETRIS_INITIAL_X 3
#define CETRIS_INITIAL_Y 0
#define CETRIS_INITIAL_Y_OFFSET 2 

#define CETRIS_HZ 60
#define CETRIS_DAS_DELAY 11 
#define CETRIS_DAS_PERIOD 3
#define CETRIS_DROP_PERIOD 2
#define CETRIS_NEXT_PIECE_DELAY 40
#define CETRIS_LINE_CLEAR_DELAY 40
#define CETRIS_LOCK_DELAY 40
#define CETRIS_WAIT_ON_CLEAR 0

#define CETRIS_STARTING_LEVEL 1

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
  u32 next_piece_tick;

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

void next_piece(cetris_game* g);
void update_board(cetris_game* g);
void lock_current(cetris_game* g);

/* API PROTOTYPES FUNCTIONS */

void init_game(cetris_game* g);
void update_game_tick(cetris_game* g);
void move_piece(cetris_game* g, input_t move);
void stop_holding(cetris_game* g, input_t move);