#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "drawable.h"
#include <string.h>
#include <stdlib.h>

GLfloat vertices[] = {
  //  Position             Texcoords
  0.0f,  0.0f, 0.0f, 0.0f, // Top-left
  0.0f,  0.0f, 1.0f, 0.0f, // Top-right
  0.0f,  0.0f, 1.0f, 1.0f, // Bottom-right
  0.0f,  0.0f, 0.0f, 1.0f  // Bottom-left
};

GLuint elements[] = {
  0, 1, 2,
  2, 3, 0
};

bool load_image(char* file_name, drawable_t *drawable) {
  int w, h, channel;
  unsigned char *data = stbi_load(file_name, &w, &h, &channel, STBI_rgb_alpha);
  if (data == NULL) return false;

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  glGenerateMipmap(GL_TEXTURE_2D);

  //glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(data);
  
  drawable->w = w;
  drawable->h = h;

  return true;
}

void crop_texture(drawable_t *drawable, int h, int w) {
  drawable->vertices[2] = drawable->vertices[14] = w;
  drawable->vertices[3] = drawable->vertices[7] = h;
  drawable->vertices[6] = drawable->vertices[10] = 1.0f;
  drawable->vertices[15] = drawable->vertices[11] = h;
}

void set_block_texture(drawable_t *drawable, uint8_t mino) {
  GLfloat index = (mino + 2) * .0625f;
  drawable->vertices[2] = drawable->vertices[14] = index;
  drawable->vertices[6] = drawable->vertices[10] = index + .0625f;
}

void update_rect(drawable_t *drawable, GLfloat x, GLfloat y, 
    GLfloat w, GLfloat h, GLfloat window_width, GLfloat window_height) {
  GLfloat x_ratio = window_width / 2.0f;
  GLfloat y_ratio = window_height / 2.0f;
  drawable->vertices[0] = drawable->vertices[12] = -1.0f + (x / x_ratio);
  drawable->vertices[1] = drawable->vertices[5] = 1.0f - (y / y_ratio); 
  drawable->vertices[9] = drawable->vertices[13] = drawable->vertices[1] - (h) / y_ratio;
  drawable->vertices[4] = drawable->vertices[8] = drawable->vertices[0] + (w) / x_ratio;

  glBindBuffer(GL_ARRAY_BUFFER, drawable->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), drawable->vertices, GL_DYNAMIC_DRAW);
}

void new_rectangle(drawable_t* drawable) {
  drawable->vertices = malloc(sizeof(vertices));
  memcpy(drawable->vertices, &vertices[0], sizeof(vertices));

  glGenVertexArrays(1, &drawable->vao);
  glBindVertexArray(drawable->vao);

  glGenBuffers(1, &drawable->vbo);

  glBindBuffer(GL_ARRAY_BUFFER, drawable->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), drawable->vertices, GL_DYNAMIC_DRAW);

  glGenBuffers(1, &drawable->ebo);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
  glEnableVertexAttribArray(0);

  //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(2* sizeof(GLfloat)));
  //glEnableVertexAttribArray(1);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

}
