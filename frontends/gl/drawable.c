#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "drawable.h"
#include <string.h>
#include <stdlib.h>

GLfloat vertices[] = {
  //  Position      Color             Texcoords
  0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
  0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Top-right
  0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, // Bottom-right
  0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f  // Bottom-left
};

GLuint elements[] = {
  0, 1, 2,
  2, 3, 0
};

void calc_pos(GLfloat *rect, GLfloat x, GLfloat y, GLfloat w, GLfloat h) {
  rect[0] = rect[21] = -1.0f + (x / 200.0f);
  rect[1] = rect[8] = 1.0f - (y / 400.0f); 
  rect[15] = rect[22] = rect[1] - (h) / 400.0f;
  rect[7] = rect[14] = rect[0] + (w) / 200.0f;
}

bool load_image(char* file_name, GLuint texture) {
  glBindTexture(GL_TEXTURE_2D, texture);

  int w, h, channel;
  unsigned char *data = stbi_load(file_name, &w, &h, &channel, 0);
  if (data == NULL) return false;

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  return true;
}

void set_block_texture(GLfloat *rect, uint8_t mino) {
  GLfloat index = (mino + 2) * .1f;
  rect[5] = rect[26] = index;
  rect[12] = rect[19] = index + .1f;
}

void update_rect(drawable_t *block, GLfloat x, GLfloat y, GLfloat w, GLfloat h, uint8_t mino) {
  calc_pos(&vertices[0], x, y, w, h);
  set_block_texture(&vertices[0], mino);

  glBindBuffer(GL_ARRAY_BUFFER, block->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
}

void new_rectangle(drawable_t* drawable) {
  glGenVertexArrays(1, &drawable->vao);
  glBindVertexArray(drawable->vao);

  glGenBuffers(1, &drawable->vbo);

  glBindBuffer(GL_ARRAY_BUFFER, drawable->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &drawable->ebo);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

  
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(2* sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

}
