#pragma once

#include "drawable.h"
#include "audio.h"

typedef struct {
  drawable_t block;
  drawable_t overlay;
  drawable_t playboard;
  drawable_t border;
  drawable_t background;

  Mix_Chunk **clear_sound;
  Mix_Chunk **tetris_sound;
  Mix_Chunk *lock_sound;
  Mix_Chunk *move_sound;
} cetris_skin_t;

void load_skin(char* name, cetris_skin_t* skin);
