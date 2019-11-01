#pragma once

#include "drawable.h"
#include "audio.h"

typedef struct {
  drawable_t block;
  drawable_t overlay;
  drawable_t playboard;
  drawable_t border;
  drawable_t background;

  audio_clip_t *clear_sound;
  audio_clip_t *tetris_sound;
  audio_clip_t lock_sound;
} cetris_skin_t;

void load_skin(char* name, cetris_skin_t* skin);
