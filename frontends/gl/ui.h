#pragma once
#include <glad/glad.h>

#include "drawable.h"
#include "skin.h"

typedef struct {
  GLfloat block_width;
  GLfloat block_height;
  GLfloat x_offset;
  GLfloat y_offset;

  drawable_t block;
  cetris_game game;
  cetris_config config;
} tetris_board_t;

typedef struct {
  int key_left;
  int key_right;
  int key_down;
  int key_drop;
  int key_hold;
  int key_rotate_cw;
  int key_rotate_ccw;
  int key_restart;
} key_bindings_t;

typedef struct {
  key_bindings_t keys;

  char* skin_name;
  cetris_skin_t skin;

  tetris_board_t board;
} cetris_ui;

void draw_tetris_board(cetris_ui *ui);
void draw_current(cetris_ui *ui);
void load_tetris_board(tetris_board_t *board, GLfloat x, GLfloat y, GLfloat w, GLfloat h);
