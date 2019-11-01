#pragma once
#include <glad/glad.h>

#include "drawable.h"
#include "skin.h"
#include "audio.h"

typedef struct {
  GLfloat block_width;
  GLfloat block_height;

  GLfloat x_offset;
  GLfloat y_offset;
} held_piece_t;

typedef struct {
  GLfloat block_width;
  GLfloat block_height;
  GLfloat block_offset;

  GLfloat x_offset;
  GLfloat y_offset;

  cetris_game game;

  held_piece_t held;

  cetris_skin_t skin;
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
  GLfloat window_height;
  GLfloat window_width;

  SDL_AudioDeviceID audio_device;

  key_bindings_t keys;

  char* skin_name;

  tetris_board_t board;

  GLuint shader_program;

} cetris_ui;

void draw_tetris_board(cetris_ui *ui);
void draw_held_piece(cetris_ui *ui);
void draw_current(cetris_ui *ui);
void load_tetris_board(cetris_ui *ui, tetris_board_t *board, GLfloat x, GLfloat y, GLfloat w, GLfloat h);
void load_held_piece(cetris_ui *ui, tetris_board_t *board, GLfloat x, GLfloat y, GLfloat w, GLfloat h);
