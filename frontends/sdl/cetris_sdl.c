#define SDL_MAIN_HANDLED
#ifdef _WIN32
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#define format_str sprintf_s
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
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
#define FRAME_RATE 60

void setup_sdl(cetris_ui *ui) {
  SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
  ui->window = SDL_CreateWindow("cetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H, SDL_WINDOW_SHOWN);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  SDL_SetHint(SDL_HINT_RENDER_DRIVER,"opengl");
  ui->render = SDL_CreateRenderer(ui->window, -1, SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_ACCELERATED);
  SDL_SetRenderDrawBlendMode(ui->render, SDL_BLENDMODE_BLEND);
  SDL_RenderSetLogicalSize(ui->render, W, H);
  if (!ui->render) exit(fprintf(stderr, "err: could not create SDL renderer\n")); 

  TTF_Init();
  ui->font_count = 0;

  IMG_Init(IMG_INIT_PNG);

  ui->skin.clear_sound = (audio_clip_t *)malloc(sizeof(audio_clip_t) * ui->skin.clear_sound_count);
  for (int i = 0; i < ui->skin.clear_sound_count; i++) {
    char name[25];
    format_str(name, 25, "data/bclear_%i.wav", i);

    SDL_LoadWAV(name, &ui->skin.clear_sound[i].wav_spec, 
        &ui->skin.clear_sound[i].wav_buffer, &ui->skin.clear_sound[i].wav_length);
  }

  SDL_LoadWAV("data/lock.wav", &ui->skin.lock_sound.wav_spec,
      &ui->skin.lock_sound.wav_buffer, &ui->skin.lock_sound.wav_length);

  SDL_LoadWAV("data/tetris.wav", &ui->skin.tetris_sound.wav_spec, 
    &ui->skin.tetris_sound.wav_buffer, &ui->skin.tetris_sound.wav_length);

  ui->audio_device = SDL_OpenAudioDevice(NULL, 0, &ui->skin.clear_sound[0].wav_spec, NULL, 0);
  if (ui->audio_device == 0) printf("failed to open audio device\n");
}

TTF_Font* get_font(cetris_ui *ui, int size) {
  for (int i = 0; i < ui->font_count; i++) {
    if (ui->fonts[i].size == size) {
      return ui->fonts[i].font;
    }
  }
  ui->fonts[ui->font_count].font = TTF_OpenFont("data/OpenSans-Regular.ttf", size);
  TTF_SetFontHinting(ui->fonts[ui->font_count].font, TTF_HINTING_MONO);
  ui->fonts[ui->font_count].size = size;
  return ui->fonts[ui->font_count++].font;
}

void draw_text(cetris_ui *ui, char* string, int x, int y, TTF_Font* font, SDL_Color color) {
  SDL_Surface *surface;
  surface = TTF_RenderText_Solid(font, string, color);

  SDL_Rect message;
  message.x = x;
  message.y = y;
  message.w = surface->w;
  message.h = surface->h;
  SDL_Texture* tex = SDL_CreateTextureFromSurface(ui->render, surface); 
  
  SDL_RenderCopy(ui->render, tex, NULL, &message);
  SDL_DestroyTexture(tex);
  SDL_FreeSurface(surface);
}

SDL_Texture* load_png(cetris_ui* ui, char* file) {
  SDL_Surface* loaded_surface = IMG_Load(file);
  return SDL_CreateTextureFromSurface(ui->render, loaded_surface);
}

void draw_image(cetris_ui *ui, SDL_Texture *i, int x, int y, int width, int height) {
  SDL_Rect dest = {x, y, width, height};
  SDL_RenderCopy(ui->render, i, NULL, &dest);
}

void draw_block(cetris_ui *ui, int x, int y, int width, int height, SDL_Color c, SDL_Color off) {
  SDL_Rect b = {x + 1, y + 1, width - 1, height - 1};
  SDL_SetRenderDrawColor(ui->render, c.r, c.g, c.b, c.a);
  SDL_RenderFillRect(ui->render, &b);
  SDL_RenderDrawRect(ui->render, &b);
  SDL_SetRenderDrawColor(ui->render, off.r, off.g, off.b, off.a);
  b.y--; b.x--; b.w+=2; b.h+=2;
  SDL_RenderDrawRect(ui->render, &b);
}

void draw_board(cetris_ui *ui, SDL_Texture *m, game_board_t* board, int x, int y, int w, int h) {
  SDL_SetRenderTarget(ui->render, m);
  SDL_RenderClear(ui->render);

  SDL_Rect background = {0, 0, w, h + 5};

  SDL_SetRenderDrawColor(ui->render, 
      board->scheme.off.r - (int)(fmod((double)board->count_down, (double)1.0) * 50), 
      board->scheme.off.g, board->scheme.off.b, board->scheme.off.a);

  SDL_RenderFillRect(ui->render, &background);
  SDL_RenderDrawRect(ui->render, &background);

  SDL_SetRenderDrawColor(ui->render, board->scheme.main.r, 
      board->scheme.main.g, board->scheme.main.b, board->scheme.main.a);

  int y_offset = 5;

  int board_x = board->game.config.board_x;
  int board_y = board->game.config.board_y;
  int board_visible = board_y - board->game.config.board_visible;

  int block_width = w / board->game.config.board_x;
  int block_height = h / board->game.config.board_visible;

  for (int s = 0; s < board_x + 1; s++) {
    int rx = (s * block_width);
    SDL_RenderDrawLine(ui->render, rx, 1, rx, h + 5);
  }

  for (int j = 0; j < board->game.config.board_visible + 1; j++) {
    int ry = y_offset + (j * block_height);
    SDL_RenderDrawLine(ui->render, 0, ry, w, ry);
  }

  for (int s = board->game.highest_line; s < board_y; s++) {
    for (int j = 0; j < board_x; j++) {
      if (board->game.board[j][s] & SLOT_OCCUPIED) {
        if (board->game.line_remove_tick[s]) {
          if (board->game.tick % 2 == 0) {
            continue;
          }
        }

        int block_x = (j * block_width);
        int block_y = (y_offset + ((s - board_visible) * block_height));
        SDL_Color mino_color = mino_colors[(board->game.board[j][s] >> 5)];

        draw_block(ui, block_x, block_y, block_width, block_height,
            mino_color, board->scheme.off);
      }
    }
  }

  if (!board->game.game_over) {
    for (int s = 0; s < 4; s++) {
      for (int j = 0; j < 4; j++) {
        if ((board->game.current.m[s]>>(3 - j))&1) {
          int block_x = ((j + board->game.current.pos.x) * block_width);
          int block_y = y_offset + ((s + board->game.current.pos.y) - board_visible) * block_height;

          SDL_Color mino_color = mino_colors[board->game.current.t];
          draw_block(ui, block_x, block_y, block_width, block_height,
              mino_color, board->scheme.off);
          
          int ghost_y = y_offset + ((s + board->game.current.ghost_y) - board_visible) * block_height;
          if (ghost_y != block_y) {
            mino_color.a -= 150;

            draw_block(ui, block_x, ghost_y, block_width, block_height,
                mino_color, board->scheme.off);
          }
        }
      }
    }
  }

  if (board->count_down > 0) 
    board->count_down-=(1.0f/FRAME_RATE);

  SDL_SetRenderTarget(ui->render, NULL);

  SDL_Rect dest = {x, y, w, h};
  //SDL_RenderCopyEx(render, m, NULL, &dest, 0, NULL, SDL_FLIP_NONE); 
  SDL_RenderCopy(ui->render, m, NULL, &dest);  
}

void draw_held_piece(cetris_ui *ui, SDL_Texture *m, game_board_t* board, int x, int y, int w, int h) {
  if (w < 8) return; 
  if (h < 8) return;

  SDL_SetRenderTarget(ui->render, m);
  SDL_RenderClear(ui->render);

  SDL_RenderClear(ui->render);

  int block_width = (w - 4) / 4;
  int block_height = (h - 4) / 4;

  // make sure blocks are square
  if (block_width > block_height) {
    block_width = block_height;
  } else {
    block_height = block_width;
  }

  SDL_Rect hold = {0, 0, w, h};

  SDL_SetRenderDrawColor(ui->render, board->scheme.off.r, 
      board->scheme.off.g, board->scheme.off.b, board->scheme.off.a);

  SDL_RenderFillRect(ui->render, &hold);
  SDL_RenderDrawRect(ui->render, &hold);

  if (board->game.piece_held) {
    for (int s = 0; s < 4; s++) {
      for (int j = 0; j < 4; j++) {
        if ((board->game.held.m[s]>>(3 - j))&1) {
          int block_x = 2 + ((j) * block_width);
          int block_y = (s * block_height);
          if (board->game.held.t == MINO_I) {
            block_y += block_height / 2;
          } else if (board->game.held.t != MINO_O) {
            block_x += block_width / 2;
          }

          SDL_Color mino_color = mino_colors[board->game.held.t];
          draw_block(ui, block_x, block_y, block_width, block_height,
              mino_color, board->scheme.off);

        }
      }
    }
  }

  SDL_SetRenderTarget(ui->render, NULL);

  SDL_Rect dest = {x, y, w, h};
  //SDL_RenderCopyEx(render, m, NULL, &dest, 0, NULL, SDL_FLIP_NONE);
  SDL_RenderCopy(ui->render, m, NULL, &dest); 
}

void draw_piece_queue(cetris_ui* ui, SDL_Texture *m, game_board_t* board, int x, int y, int w, int h) {
  if (w < 8) return;
  if (h < 32) return;

  SDL_SetRenderTarget(ui->render, m);

  SDL_RenderClear(ui->render);

  int block_width = ((w - 4) / 4);
  int block_height = ((h - 5) / 5) / 3;

  // make sure blocks are square
  if (block_width > block_height) {
    block_width = block_height;
  } else {
    block_height = block_width;
  }

  SDL_SetRenderDrawColor(ui->render, board->scheme.off.r, 
      board->scheme.off.g, board->scheme.off.b, board->scheme.off.a);

  SDL_Rect queue = {0, 0, w, h};
  SDL_RenderFillRect(ui->render, &queue);
  SDL_RenderDrawRect(ui->render, &queue);

  for (int i = 0; i < 5; i++) {
    int index = (board->game.current_index + i);

    uint8_t mino;
    if (index <= 6) {
      mino = board->game.piece_queue[index];
    } else {
      index = index % 7;
      mino = board->game.next_queue[index];
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
          draw_block(ui, block_x, block_y, block_width, block_height,
              mino_color, board->scheme.off);

        }
      }
    }
  }

  SDL_SetRenderTarget(ui->render, NULL);

  SDL_Rect dest = {x, y, w, h};
  //SDL_RenderCopyEx(render, m, NULL, &dest, 0, NULL, SDL_FLIP_NONE);
  SDL_RenderCopy(ui->render, m, NULL, &dest);  

}

void draw_timer(cetris_ui *ui, game_board_t *board, int x, int y) {
  char *buf = malloc(50);
  long double second = board->game.timer / 1000000.0f;
  if (second > 60.0f) {
    int minute = (int)(second / 60.0f);
    second -= (minute * 60.0f);
    format_str(buf, 50, "%02d:%09.6Lf", minute, second);
  } else {
    format_str(buf, 50, "%.6Lf", second);
  }
  draw_text(ui, buf, x, y, get_font(ui, 25), board->scheme.text);

  //format_str(buf, 50, "lines remaining: %i", 20 - g.lines);
  //draw_text(buf, 20, H - 60, false);

  free(buf);
}

void draw(cetris_ui* ui) {
  SDL_SetRenderDrawColor(ui->render, ui->colors.main.r, 
      ui->colors.main.g, ui->colors.main.b, ui->colors.main.a);

  SDL_RenderClear(ui->render);

  switch (ui->current_game) {
    case SOLO:
      draw_image(ui, ui->solo.background, 0, 0, W, H);
      draw_board(ui, ui->solo.main, &ui->solo.game_board, (W / 2) - 125, (H / 2) - 250, 250, 500);
      draw_held_piece(ui, ui->solo.hold, &ui->solo.game_board, (W / 2) - 230, (H / 2) - 250, 100, 100);
      draw_piece_queue(ui, ui->solo.queue, &ui->solo.game_board, (W / 2) + 130, (H / 2) - 250, 100, 450);
      draw_timer(ui, &ui->solo.game_board, 20, 20);
      draw_image(ui, ui->solo.game_background, (W / 2) - 125, (H / 2) - 250, 250, 500);
      break;
  }

  SDL_RenderPresent(ui->render);
}

void load_config(cetris_ui *ui, game_board_t *board) {
  
  ui->config.keys = (key_bindings_t){
    .key_down = SDLK_DOWN,
    .key_right = SDLK_RIGHT,
    .key_left = SDLK_LEFT,
    .key_rotate_cw = SDLK_UP,
    .key_rotate_ccw = 'x',
    .key_drop = SDLK_SPACE,
    .key_hold = 'c',
    .key_restart = 'r'
  };

  board->conf = tetris_ds_config;
  board->conf.levels = &tetris_worlds_levels[0];
  board->conf.win_condition = twenty_line_sprint; 
  board->conf.wait_on_clear = 0;

  ini_parser p;
  if (load_ini_file(&p, "config.ini")) {
    int das = atoi(get_ini_value(&p, "das", "das"));
    board->conf.das_das = das;

    int arr = atoi(get_ini_value(&p, "das", "arr"));
    board->conf.das_arr = arr;

    char* drop_delay = get_ini_value(&p, "game", "drop_delay");
    if (drop_delay) board->conf.drop_period = atoi(drop_delay);
    
    char* next_piece_delay = get_ini_value(&p, "game", "next_piece_delay");
    if (next_piece_delay) board->conf.next_piece_delay = atoi(next_piece_delay);

    char* lock_delay = get_ini_value(&p, "game", "lock_delay");
    if (lock_delay) board->conf.lock_delay = atoi(lock_delay);

    char* force_lock = get_ini_value(&p, "game", "force_lock");
    if (force_lock) board->conf.force_lock = atoi(force_lock);

    char* wait_on_clear = get_ini_value(&p, "game", "wait_on_clear");
    if (wait_on_clear) board->conf.wait_on_clear = atoi(wait_on_clear);

    char* line_delay_clear = get_ini_value(&p, "game", "line_clear_delay");
    if (line_delay_clear) board->conf.line_delay_clear = atoi(line_delay_clear);

    char *dark = get_ini_value(&p, "ui", "dark_mode");
    if (dark && atoi(dark)) {
      ui->colors = dark_mode;
    } else {
      ui->colors = light_mode;
    }
  }
}

void load_solo(cetris_ui *ui) {
  load_config(ui, &ui->solo.game_board);
  ui->solo.game_board.scheme = ui->colors;

  ui->solo.main = SDL_CreateTexture(ui->render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 251, 506);
  ui->solo.queue = SDL_CreateTexture(ui->render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 100, 450);
  ui->solo.hold = SDL_CreateTexture(ui->render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 100, 100);
  ui->solo.background = load_png(ui, "data/background.png");
  ui->solo.game_background = load_png(ui, "data/loli.png");
  SDL_SetTextureAlphaMod(ui->solo.game_background, 40);
  SDL_SetTextureBlendMode(ui->solo.game_background, SDL_BLENDMODE_BLEND);

  ui->solo.game_board.count_down = 3;

  init_game(&ui->solo.game_board.game, &ui->solo.game_board.conf);
}

void handle_key(SDL_Event e, cetris_ui *ui, game_board_t* board) {
  int sym; 
  switch (e.type) {
    case SDL_QUIT: exit(0);
    case SDL_KEYDOWN:
      sym = e.key.keysym.sym; 
      if (sym == ui->config.keys.key_left) {
        move_piece(&board->game, LEFT);
      } else if (sym == ui->config.keys.key_right) {
        move_piece(&board->game, RIGHT);
      } else if (sym == ui->config.keys.key_down) {
        move_piece(&board->game, DOWN);
      } else if (sym == ui->config.keys.key_drop) {
        move_piece(&board->game, HARD_DROP);
      } else if (sym == ui->config.keys.key_hold) {
        hold_piece(&board->game);
      } else if (sym == ui->config.keys.key_rotate_cw) {
        move_piece(&board->game, ROTATE_CW);
      } else if (sym == ui->config.keys.key_rotate_ccw) {
        move_piece(&board->game, ROTATE_CCW);
      } else if (sym == ui->config.keys.key_restart) {
        cetris_stop_game(&board->game);
        board->count_down = 3;
      }
      break;
    case SDL_KEYUP:
      sym = e.key.keysym.sym; 
      if (sym == ui->config.keys.key_left) {
        unhold_move(&board->game, LEFT);
      } else if (sym == ui->config.keys.key_right) {
        unhold_move(&board->game, RIGHT);
      } else if (sym == ui->config.keys.key_down) {
        unhold_move(&board->game, DOWN);
      } else if (sym == ui->config.keys.key_drop) {
        unhold_move(&board->game, HARD_DROP);
      } else if (sym == ui->config.keys.key_rotate_cw) {
        unhold_move(&board->game, ROTATE_CW);
      } else if (sym == ui->config.keys.key_rotate_ccw) {
        unhold_move(&board->game, ROTATE_CCW);
      }
      break;
  }
}

void handle_game_events(cetris_ui *ui, game_board_t *board) {
  if (board->count_down < 0 && board->game.waiting) {
    cetris_start_game(&board->game);
  }

  if (board->game.line_event > 0) {
    int index;
    if (ui->skin.random_audio) {
      index = rand() % 5;
    } else {
      index = board->game.line_combo - 1;
    }
    if (index > ui->skin.clear_sound_count) 
      index = ui->skin.clear_sound_count - 1;

    SDL_QueueAudio(ui->audio_device, ui->skin.clear_sound[index].wav_buffer,  
        ui->skin.clear_sound[index].wav_length);

    SDL_PauseAudioDevice(ui->audio_device, 0);

    board->game.line_event--;
    if (board->game.lock_event) 
      board->game.lock_event = 0;
  }

  if (board->game.tetris_event > 0) {
    SDL_QueueAudio(ui->audio_device, ui->skin.tetris_sound.wav_buffer,  
        ui->skin.tetris_sound.wav_length);

    SDL_PauseAudioDevice(ui->audio_device, 0);
    board->game.tetris_event--;
  }

  if (board->game.lock_event > 0) {
    SDL_QueueAudio(ui->audio_device, ui->skin.lock_sound.wav_buffer, 
        ui->skin.lock_sound.wav_length);

    SDL_PauseAudioDevice(ui->audio_device, 0);
    board->game.lock_event--;
  }
}

int main(void) {
  cetris_ui ui;
  ui.skin.clear_sound_count = 5;
  ui.skin.random_audio = true;

  setup_sdl(&ui);

  ui.current_game = SOLO;

  load_solo(&ui);
   
  SDL_Event e;
  for(;;) {
    while(SDL_PollEvent(&e)) {
      switch (ui.current_game) {
        case SOLO:
          handle_key(e, &ui, &ui.solo.game_board);
          break;
      }
    }

    switch (ui.current_game) {
      case SOLO:
        handle_game_events(&ui, &ui.solo.game_board);
        break;
    }

    draw(&ui);

    SDL_Delay(1000 / FRAME_RATE);
  }

  return 0;
}
