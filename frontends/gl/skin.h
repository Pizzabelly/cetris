#pragma once

typedef struct {
  bool has_block_texture;
  GLuint block_texture;
} cetris_skin_t;

void load_skin(char* name, cetris_skin_t* skin);
