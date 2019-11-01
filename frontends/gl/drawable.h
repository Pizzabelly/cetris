#pragma once

#include <cetris.h>

typedef struct {
  GLuint vao;
  GLuint vbo;
  GLuint ebo;
  GLuint texture;
} drawable_t;

void new_rectangle(drawable_t* drawable);
void update_rect(drawable_t *block, GLfloat x, GLfloat y, GLfloat w, GLfloat h, uint8_t mino);
bool load_image(char* file_name, GLuint texture);
