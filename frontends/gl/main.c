#include <stdio.h>
#include <string.h>

#include <glad/glad.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>

#include <cetris.h>
#include <timer.h>
#include <rules.h>
#include <ini.h>

#include "shader.h"
#include "skin.h"
#include "drawable.h"
#include "ui.h"
#include "events.h"

static const int FRAME_RATE = 144;
static const int SCREEN_FULLSCREEN = 0;
static const int SCREEN_WIDTH  = 720;
static const int SCREEN_HEIGHT = 900;
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

void load_config(cetris_ui *ui) {
  ui->board.game.config = tetris_ds_config;

  ini_parser p;
  if (load_ini_file(&p, "config.ini")) {
    char* das = get_ini_value(&p, "das", "das");
    if (das) {
      ui->board.game.config.das_das = atoi(das);
      free(das);
    } 

    char *arr = get_ini_value(&p, "das", "arr");
    if (arr) {
      ui->board.game.config.das_arr = atoi(arr);
      free(arr);
    }

    char* drop_delay = get_ini_value(&p, "game", "drop_delay");
    if (drop_delay) {
      ui->board.game.config.drop_period = atoi(drop_delay);
      free(drop_delay);
    }
    
    char* next_piece_delay = get_ini_value(&p, "game", "next_piece_delay");
    if (next_piece_delay) {
      ui->board.game.config.next_piece_delay = atoi(next_piece_delay);
      free(next_piece_delay);
    }

    char* lock_delay = get_ini_value(&p, "game", "lock_delay");
    if (lock_delay) {
      ui->board.game.config.lock_delay = atoi(lock_delay);
      free(lock_delay);
    }

    char* force_lock = get_ini_value(&p, "game", "force_lock");
    if (force_lock) {
      ui->board.game.config.force_lock = atoi(force_lock);
      free(force_lock);
    }

    char* wait_on_clear = get_ini_value(&p, "game", "wait_on_clear");
    if (wait_on_clear && !strcmp(wait_on_clear, "true")) {
      ui->board.game.config.wait_on_clear = true;
      free(wait_on_clear);
    } else ui->board.game.config.wait_on_clear = false;

    char* line_delay_clear = get_ini_value(&p, "game", "line_clear_delay");
    if (line_delay_clear) {
      ui->board.game.config.line_delay_clear = atoi(line_delay_clear);
      free(line_delay_clear);
    }

    char *skin = get_ini_value(&p, "ui", "skin");
    if (skin) {
      load_skin(skin, &ui->board.skin);
    } else load_skin("default", &ui->board.skin);
  }
}


void handle_game_events(cetris_ui *ui, tetris_board_t *board) {
  if (board->game.waiting) {
    SDL_Delay(100);
    cetris_start_game(&board->game);
  }

  if (board->game.lock_event > 0) {
    Mix_PlayChannel( 1, board->skin.lock_sound, 0 );
    board->game.lock_event--;
  }
 
  if (board->game.line_event > 0) {
    
    int index;
    //if (ui->skin.random_audio) {
    //  index = rand() % 4;//ui->skin.clear_sound_count;
    //} else {
      index = board->game.line_combo - 1;
    //}
    //if (index >= 4) 
      //index = ui->skin.clear_sound_count - 1;
   
    Mix_PlayChannel( 1, board->skin.clear_sound[index], 0 );

    board->game.line_event--;
  }

  if (board->game.tetris_event > 0) {
    //int index = rand() % ui->skin.tetris_sound_count;
    Mix_PlayChannel( 1, board->skin.tetris_sound[0], 0 );
    board->game.tetris_event--;
  }
  if (board->game.move_event > 0) {
    //int index = rand() % ui->skin.tetris_sound_count;
    Mix_PlayChannel( 0, board->skin.move_sound, 0);
    board->game.move_event--;
  }
}

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
      } else if (sym == keys->key_rotate_cw || sym == 'z') {
        move_piece(&board->game, ROTATE_CW);
      } else if (sym == keys->key_rotate_ccw) {
        move_piece(&board->game, ROTATE_CCW);
      } else if (sym == keys->key_restart) {
        cetris_stop_game(&board->game);
        board->game.waiting = true;
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
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0) {
    printf("error\n");
  }

  Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
  Mix_AllocateChannels(4);
  Mix_Volume(0, 20);

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
  ui.window_height = SCREEN_HEIGHT;
  ui.window_width = SCREEN_WIDTH;

  ui.keys = default_keys; 

  glViewport(0, 0, 720, 900);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glClearColor(0.21f, 0.12f, 0.11f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glClearDepth(1.0); 
  glDepthFunc(GL_LEQUAL);

  GLuint shaderProgram = glCreateProgram();
  new_shader_program(&shaderProgram);
  glUseProgram(shaderProgram);
  ui.shader_program = shaderProgram;

  load_config(&ui);
  init_game(&ui.board.game);
  cetris_start_game(&ui.board.game);
  
  load_tetris_board(&ui, &ui.board, 235.0f, 200.0f, 250.0f, 500.0f);
  load_held_piece(&ui, &ui.board, 152.0f, 210.0f, 88.0f, 88.0f);
  
  start_event_thread(&ui);

  SDL_Event e;

  int delay = 1000/FRAME_RATE;
  for (;;) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    while(SDL_PollEvent(&e)) {
      handle_key(e, &ui.keys, &ui.board);
    }
    
    draw_tetris_board(&ui);
    draw_held_piece(&ui);
    //handle_game_events(&ui, &ui.board);

    SDL_GL_SwapWindow(window);

    //SDL_Delay(delay);
  }
  
  SDL_GL_DeleteContext(maincontext);
  SDL_DestroyWindow(window);
 
  SDL_Quit();
  return 0;
}
