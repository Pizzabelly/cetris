#ifndef RULES_H
#define RULES_H

#include "cetris.h"

bool twenty_line_sprint(ctrs_game *g) 
{
  return g->lines >= 20;
}

bool forty_line_sprint(ctrs_game *g) 
{
  return g->lines >= 40;
}

bool marathon(ctrs_game *g) 
{
  return false;
}

// https://tetris.fandom.com/wiki/Tetris_Worlds
unsigned long tetris_worlds_levels[20] = 
{
  1000, 793, 618, 473, 355, 262, 189, 134, 94, 64,
  43,   28,  18,  11,  7,   4,   2,   1,   1,  1
};

// https://tetris.fandom.com/wiki/Tetris_DS
ctrs_config tetris_ds_config = {
  .drop_period = 83,
  .next_piece_delay = 666,
  .line_delay_clear = 666,
  .lock_delay = 500,
  .force_lock = 0,
  .das_arr = 83,
  .das_das = 183,
  .board_x = 10,
  .board_y = 43,
  .board_visible = 20,
  .mino_start_x = 3,
  .mino_start_y = 19,
  .starting_level = 1,
  .wait_on_clear = true,
  .levels = &tetris_worlds_levels[0],
  .win_condition = marathon
};

#endif /* RULES_H */
