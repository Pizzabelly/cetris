#define SDL_MAIN_HANDLED
#ifdef _WIN32
#include <SDL.h>
#include <SDL_ttf.h>
#define format_str sprintf_s
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#define format_str snprintf
#endif
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <time.h>

#include <cetris.h>
#include <timer.h>
#include <rules.h>
#include <ini.h>

#include "cetris_sdl.h"

#define W 900
#define H 720
#define FRAME_RATE 144

SDL_Renderer* render;
SDL_Window *window;
SDL_Surface *screen;

cetris_game g;

game_board_t main_board;

int font_count;
font_t fonts[10];

audio_clip_t clear_sound[4];
audio_clip_t lock_sound;
SDL_AudioDeviceID audio_device;

void setup_sdl() {
  SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
  window = SDL_CreateWindow("cetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H, SDL_WINDOW_SHOWN);
  screen = SDL_GetWindowSurface(window);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  SDL_SetHint(SDL_HINT_RENDER_DRIVER,"opengl");
  render = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_ACCELERATED);
  SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);
  SDL_RenderSetLogicalSize(render, W, H);
  if (!render) exit(fprintf(stderr, "err: could not create SDL renderer\n")); 

  TTF_Init();

  for (int i = 0; i < 4; i++) {
    char name[25];
    format_str(name, 25, "data/clear_%i.wav", i);
    SDL_LoadWAV(name, &(clear_sound[i].wav_spec), &(clear_sound[i].wav_buffer), &(clear_sound[i].wav_length));
  }

  SDL_LoadWAV("data/lock.wav", &(lock_sound.wav_spec), &(lock_sound.wav_buffer), &(lock_sound.wav_length));
  audio_device = SDL_OpenAudioDevice(NULL, 0, &(clear_sound[0].wav_spec), NULL, 0);
  if (audio_device == 0) printf("failed to open audio device\n");
}

TTF_Font* get_font(int size) {
  for (int i = 0; i < font_count; i++) {
    if (fonts[i].size == size) {
      return fonts[i].font;
    }
  }
  fonts[font_count].font = TTF_OpenFont("data/OpenSans-Regular.ttf", size);
  TTF_SetFontHinting(fonts[font_count].font, TTF_HINTING_MONO);
  fonts[font_count].size = size;
  return fonts[font_count++].font;
}

void draw_text(char* string, int x, int y, TTF_Font* font, SDL_Color color) {
  SDL_Surface *surface;
  surface = TTF_RenderText_Solid(font, string, color);

  SDL_Rect message;
  message.x = x;
  message.y = y;
  message.w = surface->w;
  message.h = surface->h;
  SDL_Texture* tex = SDL_CreateTextureFromSurface(render, surface); 
  
  SDL_RenderCopy(render, tex, NULL, &message);
  SDL_DestroyTexture(tex);
  SDL_FreeSurface(surface);
}

void draw_block(int x, int y, int width, int height, SDL_Color c, SDL_Color off) {
  SDL_Rect b = {x + 1, y + 1, width - 1, height - 1};
  SDL_SetRenderDrawColor(render, c.r, c.g, c.b, c.a);
  SDL_RenderFillRect(render, &b);
  SDL_RenderDrawRect(render, &b);
  SDL_SetRenderDrawColor(render, off.r, off.g, off.b, off.a);
  b.y--; b.x--; b.w+=2; b.h+=2;
  SDL_RenderDrawRect(render, &b);
}

void draw_board(SDL_Texture *m, game_board_t* board, int x, int y, int w, int h) {
  SDL_SetRenderTarget(render, m);
  SDL_RenderClear(render);

  SDL_Rect background = {0, 0, w, h + 5};

  SDL_SetRenderDrawColor(render, 
      board->scheme.off.r - (int)(fmod((double)board->count_down, (double)1.0) * 50), 
      board->scheme.off.g, board->scheme.off.b, board->scheme.off.a);

  SDL_RenderFillRect(render, &background);
  SDL_RenderDrawRect(render, &background);

  SDL_SetRenderDrawColor(render, board->scheme.main.r, 
      board->scheme.main.g, board->scheme.main.b, board->scheme.main.a);

  int y_offset = 5;

  int board_x = board->game->config.board_x;
  int board_y = board->game->config.board_y;
  int board_visible = board_y - board->game->config.board_visible;

  int block_width = w / board->game->config.board_x;
  int block_height = h / board->game->config.board_visible;

  for (int s = 0; s < board_x + 1; s++) {
    int rx = (s * block_width);
    SDL_RenderDrawLine(render, rx, 1, rx, h + 5);
  }

  for (int j = 0; j < board->game->config.board_visible + 1; j++) {
    int ry = y_offset + (j * block_height);
    SDL_RenderDrawLine(render, 0, ry, w, ry);
  }

  for (int s = board->game->highest_line; s < board_y; s++) {
    for (int j = 0; j < board_x; j++) {
      if (board->game->board[j][s] & SLOT_OCCUPIED) {
        if (g.line_remove_tick[s]) {
          if (board->game->tick % 2 == 0) {
            continue;
          }
        }

        int block_x = (j * block_width);
        int block_y = (y_offset + ((s - board_visible) * block_height));
        SDL_Color mino_color = mino_colors[(board->game->board[j][s] >> 5)];

        draw_block(block_x, block_y, block_width, block_height,
            mino_color, board->scheme.off);
      }
    }
  }

  if (!board->game->game_over) {
    for (int s = 0; s < 4; s++) {
      for (int j = 0; j < 4; j++) {
        if ((board->game->current.m[s]>>(3 - j))&1) {
          int block_x = ((j + board->game->current.pos.x) * block_width);
          int block_y = y_offset + ((s + board->game->current.pos.y) - board_visible) * block_height;

          SDL_Color mino_color = mino_colors[board->game->current.t];
          draw_block(block_x, block_y, block_width, block_height,
              mino_color, board->scheme.off);
          
          int ghost_y = y_offset + ((s + board->game->current.ghost_y) - board_visible) * block_height;
          if (ghost_y != block_y) {
            mino_color.a -= 150;

            draw_block(block_x, ghost_y, block_width, block_height,
                mino_color, board->scheme.off);
          }
        }
      }
    }
  }

  if (board->count_down > 0) board->count_down-=(1.0f/FRAME_RATE);

  SDL_SetRenderTarget(render, NULL);

  SDL_Rect dest = {x, y, w, h};
  //SDL_RenderCopyEx(render, m, NULL, &dest, 0, NULL, SDL_FLIP_NONE); 
  SDL_RenderCopy(render, m, NULL, &dest);  
}

void draw_held_piece(SDL_Texture *m, game_board_t* board, int x, int y, int w, int h) {
  if (w < 8) return; 
  if (h < 8) return;

  SDL_SetRenderTarget(render, m);
  SDL_RenderClear(render);

  SDL_RenderClear(render);

  int block_width = (w - 4) / 4;
  int block_height = (h - 4) / 4;

  // make sure blocks are square
  if (block_width > block_height) {
    block_width = block_height;
  } else {
    block_height = block_width;
  }

  SDL_Rect hold = {0, 0, w, h};

  SDL_SetRenderDrawColor(render, board->scheme.off.r, 
      board->scheme.off.g, board->scheme.off.b, board->scheme.off.a);

  SDL_RenderFillRect(render, &hold);
  SDL_RenderDrawRect(render, &hold);

  if (board->game->piece_held) {
    for (int s = 0; s < 4; s++) {
      for (int j = 0; j < 4; j++) {
        if ((board->game->held.m[s]>>(3 - j))&1) {
          int block_x = 2 + ((j) * block_width);
          int block_y = (s * block_height);
          if (board->game->held.t == MINO_I) {
            block_y += block_height / 2;
          } else if (board->game->held.t != MINO_O) {
            block_x += block_width / 2;
          }

          SDL_Color mino_color = mino_colors[board->game->held.t];
          draw_block(block_x, block_y, block_width, block_height,
              mino_color, board->scheme.off);

        }
      }
    }
  }

  SDL_SetRenderTarget(render, NULL);

  SDL_Rect dest = {x, y, w, h};
  //SDL_RenderCopyEx(render, m, NULL, &dest, 0, NULL, SDL_FLIP_NONE);
  SDL_RenderCopy(render, m, NULL, &dest); 
}

void draw_piece_queue(SDL_Texture *m, game_board_t* board, int x, int y, int w, int h) {
  if (w < 8) return;
  if (h < 32) return;

  SDL_SetRenderTarget(render, m);

  SDL_RenderClear(render);

  int block_width = ((w - 4) / 4);
  int block_height = ((h - 5) / 5) / 3;

  // make sure blocks are square
  if (block_width > block_height) {
    block_width = block_height;
  } else {
    block_height = block_width;
  }

  SDL_SetRenderDrawColor(render, board->scheme.off.r, 
      board->scheme.off.g, board->scheme.off.b, board->scheme.off.a);

  SDL_Rect queue = {0, 0, w, h};
  SDL_RenderFillRect(render, &queue);
  SDL_RenderDrawRect(render, &queue);

  for (int i = 0; i < 5; i++) {
    int index = (board->game->current_index + i);

    uint8_t mino;
    if (index <= 6) {
      mino = board->game->piece_queue[index];
    } else {
      index = index % 7;
      mino = board->game->next_queue[index];
    }

    for (int s = 0; s < 4; s++) {
      for (int j = 0; j < 4; j++) {
        if ((default_matrices[mino][s]>>(3 - j))&1) {
          int block_x =  2 + ((j) * block_width);
          int block_y = (int)(h * (i/5.0)) + (s * block_height);
          if (mino == MINO_I) {
            block_y += block_height / 2;
          } else if (mino != MINO_O) {
            block_x += block_width / 2;
          }

          SDL_Color mino_color = mino_colors[mino];
          draw_block(block_x, block_y, block_width, block_height,
              mino_color, board->scheme.off);

        }
      }
    }
  }

  SDL_SetRenderTarget(render, NULL);

  SDL_Rect dest = {x, y, w, h};
  //SDL_RenderCopyEx(render, m, NULL, &dest, 0, NULL, SDL_FLIP_NONE);
  SDL_RenderCopy(render, m, NULL, &dest);  

}

void draw_timer(game_board_t *board, int x, int y) {
  char *buf = malloc(50);
  long double second = g.timer / 1000000.0f;
  if (second > 60.0f) {
    int minute = (int)(second / 60.0f);
    second -= (minute * 60.0f);
    format_str(buf, 50, "%02d:%09.6Lf", minute, second);
  } else {
    format_str(buf, 50, "%.6Lf", second);
  }
  draw_text(buf, x, y, get_font(25), board->scheme.text);

  //format_str(buf, 50, "lines remaining: %i", 20 - g.lines);
  //draw_text(buf, 20, H - 60, false);

  free(buf);
}

void draw(solo_game *s) {
  SDL_SetRenderDrawColor(render, main_board.scheme.main.r, 
      main_board.scheme.main.g, main_board.scheme.main.b, main_board.scheme.main.a);

  SDL_RenderClear(render);
  
  draw_board(s->main, &main_board, (W / 2) - 125, (H / 2) - 250, 250, 500);
  draw_held_piece(s->hold, &main_board, (W / 2) - 230, (H / 2) - 250, 100, 100);
  draw_piece_queue(s->queue, &main_board, (W / 2) + 130, (H / 2) - 250, 100, 450);
  draw_timer(&main_board, 20, 20);

  SDL_RenderPresent(render);
}

void load_solo(solo_game* s) {
  s->main = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 250, 505);
  s->queue = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 100, 450);
  s->hold = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 100, 100);
}

int main(void) {
  setup_sdl();

  cetris_config config = tetris_ds_config;
  config.win_condition = twenty_line_sprint;
  config.levels = &tetris_worlds_levels[0];
  config.wait_on_clear = 0;

  bool dark_theme = false;

  ini_parser p;
  if (load_ini_file(&p, "config.ini")) {
    int das = atoi(get_ini_value(&p, "das", "das"));
    config.das_das = das;

    int arr = atoi(get_ini_value(&p, "das", "arr"));
    config.das_arr = arr;

    char* drop_delay = get_ini_value(&p, "game", "drop_delay");
    if (drop_delay) config.drop_period = atoi(drop_delay);
    
    char* next_piece_delay = get_ini_value(&p, "game", "next_piece_delay");
    if (next_piece_delay) config.next_piece_delay = atoi(next_piece_delay);

    char* lock_delay = get_ini_value(&p, "game", "lock_delay");
    if (lock_delay) config.lock_delay = atoi(lock_delay);

    char* force_lock = get_ini_value(&p, "game", "force_lock");
    if (force_lock) config.force_lock = atoi(force_lock);

    char* wait_on_clear = get_ini_value(&p, "game", "wait_on_clear");
    if (wait_on_clear) config.wait_on_clear = atoi(wait_on_clear);

    char* line_delay_clear = get_ini_value(&p, "game", "line_clear_delay");
    if (line_delay_clear) config.line_delay_clear = atoi(line_delay_clear);

    char *dark = get_ini_value(&p, "ui", "dark_mode");
    if (dark && atoi(dark)) {
      dark_theme = true;
    }
  }
  
  if (dark_theme) {
    main_board.scheme = dark_mode;
  } else {
    main_board.scheme = light_mode;
  }

  solo_game screen;
  load_solo(&screen);

  main_board.count_down = 3;  
  init_game(&g, &config);
  main_board.game = &g;
   
  SDL_Event e;
  for(;;) {
    while(SDL_PollEvent(&e)) {
       switch (e.type) {
          case SDL_QUIT:
            exit(0);
          case SDL_KEYDOWN:
            switch (e.key.keysym.sym) {
              case SDLK_LEFT:
                move_piece(&g, LEFT); break;
              case SDLK_RIGHT:
                move_piece(&g, RIGHT); break;
              case SDLK_DOWN:
                move_piece(&g, DOWN); break;
              case SDLK_SPACE:
                move_piece(&g, HARD_DROP); break;
              case SDLK_UP:
                move_piece(&g, ROTATE_CW); break;
              case 'c':
                hold_piece(&g); break;
              case 'x':
                move_piece(&g, ROTATE_CW); break;
              case 'z':
                move_piece(&g, ROTATE_CCW); break;
              case 'r':
                cetris_stop_game(&g);
                main_board.count_down = 3;
                break;
            }
            break;
          case SDL_KEYUP:
            switch (e.key.keysym.sym) {
              case SDLK_LEFT:
                unhold_move(&g, LEFT); break;
              case SDLK_RIGHT:
                unhold_move(&g, RIGHT); break;
              case SDLK_DOWN:
                unhold_move(&g, DOWN); break;
              case SDLK_SPACE:
                unhold_move(&g, HARD_DROP); break;
              case SDLK_UP:
                unhold_move(&g, ROTATE_CW); break;
              case 'x':
                unhold_move(&g, ROTATE_CW); break;
              case 'z':
                unhold_move(&g, ROTATE_CCW); break;
            }
            break;
       }
    }

    draw(&screen);
   
    if (main_board.count_down < 0 && g.waiting) {
	    cetris_start_game(&g);
    }

    if (g.line_event > 0) {
      int index = g.line_combo - 1;
      if (index > 3) index = 3;
      int success = SDL_QueueAudio(audio_device, clear_sound[index].wav_buffer,  clear_sound[index].wav_length);
      SDL_PauseAudioDevice(audio_device, 0);
      g.line_event--;
      if (g.lock_event) {
        g.lock_event = 0;
      }
    }

    if (g.lock_event > 0) {
      int success = SDL_QueueAudio(audio_device, lock_sound.wav_buffer,  lock_sound.wav_length);
      SDL_PauseAudioDevice(audio_device, 0);
      g.lock_event--;
    }

    SDL_Delay(1000 / FRAME_RATE);
  }

}
