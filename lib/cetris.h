#ifndef CETRIS_H
#define CETRIS_H

#ifdef __linux__
#define _GNU_SOURCE
#endif

#include <stdint.h>
#include <stdbool.h>

#define CETRIS_EXPORT

#define CETRIS_HI_RES 1

#if CETRIS_HI_RES
#define CETRIS_HZ 1000
typedef uint64_t ctick;
#else
#define CETRIS_HZ 60
typedef uint16_t ctick;
#endif

#define CETRIS_ENABLE_DAS 1

typedef struct {
  int8_t x;
  int8_t y;
} vec2;

typedef uint8_t piece_matrix[4];

enum {
  MINO_O,
  MINO_I,
  MINO_S,
  MINO_Z,
  MINO_L,
  MINO_J,
  MINO_T
};

enum {
  SLOT_OCCUPIED = 1,
};

enum { 
  INIT, 
  ONCE_RIGHT, 
  TWICE, 
  ONCE_LEFT 
};

enum {
  DOWN,
  RIGHT,
  LEFT,
  ROTATE_CCW,
  ROTATE_CW,
  HARD_DROP
};

typedef struct {
  vec2 pos;
  uint8_t t;
  uint8_t r;
  uint8_t ghost_y;
  bool locked;
  bool held;
  ctick lock_tick;
  ctick force_lock_tick;
  piece_matrix m;
} tetrimino;

typedef struct cetris_game cetris_game;

typedef struct {
  ctick drop_period;
  ctick next_piece_delay;
  ctick line_delay_clear;
  ctick lock_delay;
  ctick force_lock;

#if CETRIS_ENABLE_DAS
  ctick das_arr; 
  ctick das_das;
#endif
  
  uint8_t board_x;
  uint8_t board_y;
  uint8_t board_visible;

  uint8_t mino_start_x;
  uint8_t mino_start_y;

  uint8_t starting_level;
  bool wait_on_clear;

  ctick *levels;
  
  bool (*win_condition)(cetris_game *);
} cetris_config;

struct cetris_game {
  // playfield represented by a 2d array
  uint8_t **board;
  int8_t highest_line;

  // queue of all 7 possible tetrimino
  uint8_t piece_queue[7];
  uint8_t next_queue[7];
  uint8_t current_index;

  // current tetrimino
  tetrimino current;
  tetrimino held;
  bool piece_held;

  // waiting to start
  bool waiting;

  // internal game tick
  ctick tick;
  ctick next_drop_tick;
  ctick next_piece_tick;
  ctick down_move_tick;
  ctick *line_remove_tick;

#if CETRIS_HI_RES
  // microsecond accuracy timer
  long long timer;
#endif

#if CETRIS_ENABLE_DAS
  ctick das_wait;
  ctick next_das_move;
  uint8_t das_move;
  uint8_t held_moves[8];
#endif

  // progress trackers 
  uint8_t level;
  uint8_t lines;
  uint8_t line_combo;
  bool line_event;
  bool game_over;

  // scoring flags
  bool tspin;
  bool mini_tspin;

  // score counter
  uint16_t score;
  
  // config
  cetris_config config;
};

CETRIS_EXPORT bool update_game_tick(cetris_game *g);
CETRIS_EXPORT void move_piece(cetris_game *g, uint8_t move);
CETRIS_EXPORT void unhold_move(cetris_game* g, uint8_t move);
CETRIS_EXPORT void init_game(cetris_game *g, cetris_config *c);
CETRIS_EXPORT void hold_piece(cetris_game *g);
CETRIS_EXPORT const piece_matrix default_matrices[7];

#endif /* CETRIS_H */
