#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cetris.h>

const char *vertex_shader_source = "#version 450 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    "   TexCoord = aTexCoord;\n"
    "}\0";

const char *fragment_shader_source = "#version 450 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D ourTexture;\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);\n"
    "}\n\0";


GLuint shader_program;

GLfloat default_rect[32] = {
   // positions       // colors          // texture coords
   0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,   // top right
   0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,   // bottom right
   0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,   // bottom left
   0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f    // top left 
};

typedef struct {
  GLfloat r;
  GLfloat g;
  GLfloat b;
} rbg_color;

GLuint indices[] = {  
  0, 1, 3, // first triangle
  1, 2, 3  // second triangle
};

rbg_color colors[8] = {
  {0.0f, 0.0f, 0.0f},     // Black
  {0.127f,0.219f,0.255f}, // Aqua
  {0.61f,0.153f,0.112f},  // Olive
  {0.177f,0.13f,0.201f},  // Purple
  {0.240f,0.18f,0.190f},  // Fuchsia
  {0.255f,0.133f,0.27f},  // Orange
  {0.0f,0.31f,0.63f},     // Navy
  {0.255f,0.220f,0.0f}    // Yellow 
};

struct block_drawable {
  GLuint vao;
  GLuint vbo;
  GLuint ebo;
  GLuint texture;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}  

void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);
}

void load_texture(char *file_name, GLuint texture) {
  int w, h, channel;
  unsigned char *data = stbi_load(file_name, &w, &h, &channel, 0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
}

void load_vertex_shader(GLuint program) {
  GLint vertex_shader;
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  glAttachShader(program, vertex_shader);
  glDeleteShader(vertex_shader);
}

void load_fragment_shader(GLuint program) {
  GLint fragment_shader;
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  glAttachShader(program, fragment_shader);
  glDeleteShader(fragment_shader);
}

void create_block(struct block_drawable *b) {
  glGenVertexArrays(1, &b->vao);
  glGenBuffers(1, &b->vbo);
  glGenBuffers(1, &b->ebo);
  glGenTextures(1, &b->texture);

  glBindVertexArray(b->vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

  glBindBuffer(GL_ARRAY_BUFFER, b->vbo);
  glBufferData(GL_ARRAY_BUFFER, 32 * sizeof(GLfloat), default_rect, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3* sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

  load_texture("block.jpg", b->texture);
}

void update_block(struct block_drawable *b, int x, int y, int color) {
  GLfloat block[32];
  memcpy(block, default_rect, sizeof(GLfloat) * 32);

  block[0] = block[8] = (x + 1.0f) / 10.0f;
  block[1] = block[25] = (((y - CETRIS_BOARD_VISABLE) * -1.0f) + 1.0f) / 20.0f; 
  block[9] = block[17] = block[1] - .05f;
  block[16] = block[24] = block[0] - .1f;

  block[3] = block[11] = block[19] = block[27] = colors[color].r;
  block[4] = block[12] = block[20] = block[28] = colors[color].g;
  block[5] = block[13] = block[21] = block[29] = colors[color].b;

  glBindVertexArray(b->vao);
  glBindBuffer(GL_ARRAY_BUFFER, b->vbo);
  glBufferData(GL_ARRAY_BUFFER, 32 * sizeof(GLfloat), block, GL_DYNAMIC_DRAW);
}

int main(void) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(400, 800, "Cetris", NULL, NULL);
  if (window == NULL) {
    printf("[Error] failed to create GLFW window\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("[Error] failed to load GLAD\n");
    return -1;
  }

  glViewport(0, 0, 400, 800);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  shader_program = glCreateProgram();
  load_fragment_shader(shader_program);
  load_vertex_shader(shader_program);
  glLinkProgram(shader_program);

  struct block_drawable block;
  create_block(&block);

  struct cetris_game cetris;
  init_game(&cetris);

  double prev_time = glfwGetTime();

  while(!glfwWindowShouldClose(window)) {
    process_input(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, block.texture);
    glUseProgram(shader_program);
    glBindVertexArray(block.vao);
    for (int x = 0; x < CETRIS_BOARD_X; x++) {
      for (int y = CETRIS_BOARD_VISABLE; y < CETRIS_BOARD_Y; y++) {
        if (cetris.board[x][y].occupied) {
          update_block(&block, x, y, cetris.board[x][y].c);  
          glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
      }
    }

    double current_time = glfwGetTime();
    if (current_time - prev_time >= 0.0166) {
      update_game_tick(&cetris);
      prev_time = current_time;
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  
  glfwTerminate();
  return 0;
}
