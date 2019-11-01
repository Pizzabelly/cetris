#pragma once

#include "drawable.h"

typedef struct {
  bool has_block_texture;
  drawable_t block;

  bool has_overlay_texture;
  drawable_t overlay;

} cetris_skin_t;

void load_skin(char* name, cetris_skin_t* skin);
