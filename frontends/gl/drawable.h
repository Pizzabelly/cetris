#pragma once

#include <cetris.h>

typedef struct {
  GLuint vao;
  GLuint vbo;
  GLuint ebo;
  GLuint texture;

  GLfloat w;
  GLfloat h;

  GLfloat *vertices;
} drawable_t;

void new_rectangle(drawable_t* drawable);
void update_rect(drawable_t *drawable, GLfloat x, GLfloat y, GLfloat w, GLfloat h, GLfloat window_width, GLfloat window_height);
void set_block_texture(drawable_t *drawable, uint8_t mino);
bool load_image(char* file_name, drawable_t *drawable);
void crop_texture(drawable_t *drawable, int h, int w);
