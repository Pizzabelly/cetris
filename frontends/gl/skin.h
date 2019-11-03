#pragma once

#include "drawable.h"
#include "audio.h"

typedef struct {
  drawable_t block;
  drawable_t overlay;
  drawable_t playboard;
  drawable_t border;
  drawable_t background;

  GLfloat current_shine;
  GLfloat overlay_shine;

  Mix_Chunk **combo_sound;
  int combo_sound_count;

  Mix_Chunk **clear_sound;
  int clear_sound_count;

  Mix_Chunk **tetris_sound;
  int tetris_sound_count;

  Mix_Chunk *lock_sound;
  Mix_Chunk *move_sound;
} cetris_skin_t;

void load_skin(char* name, cetris_skin_t* skin);
