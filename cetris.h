#pragma once

#define BOARD_X 10
#define BOARD_Y 43 
#define BOARD_VISABLE 23 

#define CETRIS_HZ 60
#define CETRIS_DAS_DELAY 11
#define CETRIS_DAS_PERIOD 5
#define CETRIS_LINE_CLEAR_DELAY 40

typedef struct {
  int x;
  int y;
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

typedef int piece_matrix[4][4];

struct tetrimino {
  type t;
  rstate r;
  color c;
  piece_matrix mat;
  vec2 pos;
  int lock_tick;
};

typedef struct {
  int occupied;
  int constant;
  int remove_tick;
  color c;
} slot;

enum movements {
  DOWN = 1,
  LEFT = 2,
  RIGHT = 3
};

struct cetris_game {
  /* playfield represented by a 2d array */
  slot board[BOARD_X][BOARD_Y];

  /* constant queue of all 7 possible tetrimino */
  struct tetrimino piece_queue[7];

  /* current tetrimino */
  struct tetrimino current;
  int current_index;

  /* 20 action movment queue so das 
   * can be input independedent */
  enum movements move_queue[20];
  int move_queue_count;
  int move_queue_pos;

  /* internal game tick */
  int tick;
  int next_drop_tick;

  /* progress trackers */
  int lines;
  int level;

  /* scoring flags */
  int tspin;
  int mini_tspin;

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
