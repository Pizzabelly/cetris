#pragma once

#include <stdint.h>

#define BOARD_X 10
#define BOARD_Y 43 
#define BOARD_VISABLE 23 

#define CETRIS_HZ 60
#define CETRIS_DAS_DELAY 6  
#define CETRIS_DAS_PERIOD 3 
#define CETRIS_LINE_CLEAR_DELAY 40
#define CETRIS_WAIT_ON_CLEAR 0

typedef struct {
  int8_t x;
  int8_t y;
} vec2;

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
  RRIGHT,
  RLEFT,
  TWO
} rstate;

typedef uint8_t piece_matrix[4][4];

struct tetrimino {
  type t;
  rstate r;
  color c;
  piece_matrix mat;
  vec2 pos;
  int lock_tick;
};

typedef struct {
  uint8_t occupied;
  uint8_t constant;
  int remove_tick;
  color c;
} slot;

enum movement {
  DOWN = 1,
  LEFT = 2,
  RIGHT = 3,
  USER_DOWN = 4 
};

struct cetris_game {
  /* playfield represented by a 2d array */
  slot board[BOARD_X][BOARD_Y];

  /* constant queue of all 7 possible tetrimino */
  struct tetrimino piece_queue[7];

  /* current tetrimino */
  struct tetrimino current;
  uint8_t current_index;

  enum movement queued_move;
  enum movement prev_move;
  uint16_t move_repeat;

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

/* API PROTOTYPES FUNCTIONS */

void init_game(struct cetris_game* g);
void update_game_tick(struct cetris_game* g);
void move_down(struct cetris_game* g);
void move_left(struct cetris_game* g);
void move_right(struct cetris_game* g);
void move_hard_drop(struct cetris_game* g);
void rotate_clockwise(struct cetris_game* g);
void rotate_counterclockwise(struct cetris_game* g);
