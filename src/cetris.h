#pragma once

#include <stdint.h>

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
  int lock_tick;
};

typedef struct {
  uint8_t occupied;
  uint8_t constant;
  int remove_tick;
  color c;
} slot;

struct cetris_game {
  /* playfield represented by a 2d array */
  slot board[CETRIS_BOARD_X][CETRIS_BOARD_Y];

  /* constant queue of all 7 possible tetrimino */
  struct tetrimino piece_queue[7];

  /* current tetrimino */
  struct tetrimino current;
  uint8_t current_index;

  /* input_manager */
  uint8_t held_moves[7];
  int das_repeat;
  input_t prev_das_move;
  int das_move_tick;
  int down_move_tick;

  /* internal game tick */
  int tick;
  int next_drop_tick;

  /* progress trackers */
  int lines;
  uint8_t level;
  uint8_t game_over;

  /* scoring flags */
  uint8_t tspin;
  uint8_t mini_tspin;

  /* long int just incase */
  long int score;
};

void next_piece(struct cetris_game* g);
void wipe_board(struct cetris_game* g);

/* API PROTOTYPES FUNCTIONS */

void init_game(struct cetris_game* g);
void update_game_tick(struct cetris_game* g);
void move_peice(struct cetris_game* g, input_t move);
void stop_holding(struct cetris_game* g, input_t move);
