#include <stdio.h>
#include <string.h>

#include <glad/glad.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_opengl.h>

#include <cetris.h>
#include <timer.h>
#include <rules.h>

#include "shader.h"
#include "skin.h"
#include "drawable.h"
#include "ui.h"

static const int FRAME_RATE = 144;
static const int SCREEN_FULLSCREEN = 0;
static const int SCREEN_WIDTH  = 400;
static const int SCREEN_HEIGHT = 800;
static SDL_Window *window = NULL;
static SDL_GLContext maincontext;

key_bindings_t default_keys = {
    .key_down = SDLK_DOWN,
    .key_right = SDLK_RIGHT,
    .key_left = SDLK_LEFT,
    .key_rotate_cw = SDLK_UP,
    .key_rotate_ccw = 'x',
    .key_drop = SDLK_SPACE,
    .key_hold = 'c',
    .key_restart = 'r'
};

void handle_key(SDL_Event e, key_bindings_t *keys, tetris_board_t* board) {
  int sym; 
  switch (e.type) {
    case SDL_QUIT: exit(0);
    case SDL_KEYDOWN:
      sym = e.key.keysym.sym; 
      if (sym == keys->key_left) {
        move_piece(&board->game, LEFT);
      } else if (sym == keys->key_right) {
        move_piece(&board->game, RIGHT);
      } else if (sym == keys->key_down) {
        move_piece(&board->game, DOWN);
      } else if (sym == keys->key_drop) {
        move_piece(&board->game, HARD_DROP);
      } else if (sym == keys->key_hold) {
        hold_piece(&board->game);
      } else if (sym == keys->key_rotate_cw) {
        move_piece(&board->game, ROTATE_CW);
      } else if (sym == keys->key_rotate_ccw) {
        move_piece(&board->game, ROTATE_CCW);
      } else if (sym == keys->key_restart) {
        cetris_stop_game(&board->game);
        //board->count_down = 3;
      }
      break;
    case SDL_KEYUP:
      sym = e.key.keysym.sym; 
      if (sym == keys->key_left) {
        unhold_move(&board->game, LEFT);
      } else if (sym == keys->key_right) {
        unhold_move(&board->game, RIGHT);
      } else if (sym == keys->key_down) {
        unhold_move(&board->game, DOWN);
      } else if (sym == keys->key_drop) {
        unhold_move(&board->game, HARD_DROP);
      } else if (sym == keys->key_rotate_cw) {
        unhold_move(&board->game, ROTATE_CW);
      } else if (sym == keys->key_rotate_ccw) {
        unhold_move(&board->game, ROTATE_CCW);
      }
      break;
  }
}

int main(void) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("error\n");
  }

  SDL_GL_LoadLibrary(NULL);
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

  if (SCREEN_FULLSCREEN) {
    window = SDL_CreateWindow(
      "cetris", 
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
      0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL
    );
  } else {
    window = SDL_CreateWindow(
      "cetris", 
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
      SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL
    );
  }

  maincontext = SDL_GL_CreateContext(window);

  printf("OpenGL loaded\n");
  gladLoadGLLoader(SDL_GL_GetProcAddress);
  printf("Vendor:   %s\n", glGetString(GL_VENDOR));
  printf("Renderer: %s\n", glGetString(GL_RENDERER));
  printf("Version:  %s\n", glGetString(GL_VERSION));

  SDL_GL_SetSwapInterval(1);

  cetris_ui ui;
  ui.keys = default_keys; 

  glViewport(0, 0, 400, 800);

  GLuint shaderProgram = glCreateProgram();
  new_shader_program(&shaderProgram);
  glUseProgram(shaderProgram);

  printf("%i\n", tetris_ds_config.board_x); 
  ui.board.config = tetris_ds_config;

  init_game(&ui.board.game, &ui.board.config);
  cetris_start_game(&ui.board.game);
  
  load_tetris_board(&ui.board, 50.0f, 155.0f, 150.0f, 645.0f);
  new_rectangle(&ui.board.block);

  load_skin("test", &ui.skin);

  glBindTexture(GL_TEXTURE_2D, ui.skin.block_texture);

  SDL_Event e;

  int delay = 1000/FRAME_RATE;
  for (;;) {
    while(SDL_PollEvent(&e)) {
      handle_key(e, &ui.keys, &ui.board);
    }

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    draw_tetris_board(&ui);
    draw_current(&ui);

    SDL_GL_SwapWindow(window);

    SDL_Delay(delay);
  }
  
  SDL_GL_DeleteContext(maincontext);
  SDL_DestroyWindow(window);
 
  SDL_Quit();
  return 0;
}
