#define SDL_MAIN_HANDLED
#ifdef _WIN32
#include <SDL.h>
#include <windows.h>
#include <profileapi.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <pthread.h>
#include <unistd.h>
#endif
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <time.h>

#include <cetris.h>
#include <ini.h>

#define W 570
#define H 640

#define BLOCK_SIZE 25

#define X_OFFSET (W/2) - (BLOCK_SIZE * 5)
#define Y_OFFSET (H/2) - (BLOCK_SIZE * 10)

SDL_Renderer* render;
TTF_Font* font;
SDL_Window *window;
SDL_Surface *screen;

bool dark_mode;
SDL_Color main_color;
SDL_Color off;
SDL_Color text;

float count_down;
bool game_running;

long long timer;

cetris_game g;

uint8_t colors[7][3] = {
  {253,253,150},    // Yellow
  {174,198,207},   // Aqua
  {255,105,97},  // Red
  {170,221,119},   // Olive
  {255,179,71},    // Orange
  {119,158,203},   // Navy
  {177,156,217}    // Purple   
};

#ifdef _WIN32
DWORD WINAPI game_loop(void* data) {
  LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
  LARGE_INTEGER Frequency;
  QueryPerformanceFrequency(&Frequency);
  QueryPerformanceCounter(&StartingTime);
  while(1) {
    if (!game_running) break;
    QueryPerformanceCounter(&EndingTime); 
    ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
    ElapsedMicroseconds.QuadPart *= 1000000;
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
    timer = ElapsedMicroseconds.QuadPart;
    g.tick = ElapsedMicroseconds.QuadPart / 1000;
    if (!update_game_tick(&g)) {
	    break;
    }
    Sleep(1);
  }
  return 0;
}
#else
void *game_loop(void) {
  timer = 0;
  long nsec = 1000;
  while(1) {
    if (timer % 1000 == 0) {
      g.tick++;
      update_game_tick(&g);
    }
    nanosleep((const struct timespec[]){{0, nsec}}, NULL);
    timer++;
  }
  return 0;
}
#endif

void setup() {
  SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
  window = SDL_CreateWindow("cetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H, SDL_WINDOW_SHOWN);
  screen = SDL_GetWindowSurface(window);
  //SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  render = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_ACCELERATED);
  SDL_RenderSetLogicalSize(render, W, H);
  if (!render) exit(fprintf(stderr, "err: could not create SDL renderer\n")); 

  TTF_Init();
  font = TTF_OpenFont("OpenSans-Regular.ttf", 20);
  //TTF_SetFontHinting(font, TTF_HINTING_MONO);
}

void draw_text(char* string, int x, int y) {
  SDL_Surface* surface = TTF_RenderText_Solid(font, string, text);
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

void load_colors() {
	if (dark_mode) {
    main_color = (SDL_Color){100, 100, 100, 255};
    off =  (SDL_Color){50, 50, 50, 255};
    text = (SDL_Color){240, 240, 240, 255};
  	SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_ADD);
	} else {
		main_color = (SDL_Color){255, 255, 255, 255};
		off =  (SDL_Color){235, 235, 235, 255};
		text = (SDL_Color){10, 10, 10, 255};
  	SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);
	}
}


void draw() {
  SDL_Texture *m = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, W, H);
  SDL_SetRenderTarget(render, m);
  
  SDL_SetRenderDrawColor(render, main_color.r, main_color.g, main_color.b, 255);
  SDL_RenderClear(render);

  SDL_Rect board = {X_OFFSET + 1, Y_OFFSET, 250, 500};
  SDL_Rect queue = {W - (W / 4), Y_OFFSET, (BLOCK_SIZE * 4) + 20, BLOCK_SIZE * 5 * 3};
  SDL_Rect hold = {30, Y_OFFSET, BLOCK_SIZE * 4, BLOCK_SIZE * 4};
  SDL_SetRenderDrawColor(render, off.r - (fmod((double)count_down, (double)1.0) * 50), off.g, off.b, 255);
  SDL_RenderFillRect(render, &board);
  SDL_RenderFillRect(render, &queue);
  SDL_RenderFillRect(render, &hold);
  SDL_RenderDrawRect(render, &board);
  SDL_RenderDrawRect(render, &queue);
  SDL_RenderDrawRect(render, &hold);

  SDL_SetRenderDrawColor(render, main_color.r, main_color.g, main_color.b, 255);
  for (int x = 0; x < CETRIS_BOARD_X + 1; x++) {
    int rx = X_OFFSET + 1 + (x * BLOCK_SIZE);
    SDL_RenderDrawLine(render, rx, Y_OFFSET + 1, rx, Y_OFFSET + 500);
  }
  for (int y = 0; y < CETRIS_BOARD_Y - CETRIS_BOARD_VISABLE + 1; y++) {
    int ry = Y_OFFSET + (y * BLOCK_SIZE);
    SDL_RenderDrawLine(render, X_OFFSET + 1, ry, X_OFFSET + 250, ry);
  }

  SDL_Rect b = {0, 0, BLOCK_SIZE, BLOCK_SIZE}; 
  SDL_Rect w = {0, 0, BLOCK_SIZE + 1, BLOCK_SIZE + 1}; 

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if ((g.current.m[y]>>(3 - x))&1) {
        b.x = X_OFFSET + ((x + g.current.pos.x) * BLOCK_SIZE) + 1;
        b.y = (Y_OFFSET + ((y + g.current.pos.y) * BLOCK_SIZE)) - (CETRIS_BOARD_VISABLE * BLOCK_SIZE);
  
        uint8_t (*color)[3] = &(colors[g.current.t]);
        SDL_SetRenderDrawColor(render, (*color)[0], (*color)[1], (*color)[2], 255);
  
        SDL_RenderFillRect(render, &b);
        SDL_RenderDrawRect(render, &b);

        SDL_SetRenderDrawColor(render, off.r, off.g, off.b, 255);
        w.y = b.y - 1;
        w.x = b.x - 1;
        SDL_RenderDrawRect(render, &w);

        b.y = (Y_OFFSET + ((y + g.current.ghost_y) * BLOCK_SIZE)) - (CETRIS_BOARD_VISABLE * BLOCK_SIZE);

        w.y = b.y - 1;
        SDL_RenderDrawRect(render, &w);
  
        SDL_SetRenderDrawColor(render, (*color)[0], (*color)[1], (*color)[2], 160);
        SDL_RenderFillRect(render, &b);
        SDL_RenderDrawRect(render, &b);

      }

      if (g.piece_held) {
        if ((g.held.m[y]>>(3 - x))&1) {
          b.x = 40 + ((x) * BLOCK_SIZE);
          b.y = Y_OFFSET + ((y) * BLOCK_SIZE);
          if (g.held.t == MINO_I) {
            b.x -= 10;
            b.y += 10;
          }
          if (g.held.t == MINO_O) b.x -= 10;

          uint8_t (*color)[3] = &(colors[g.held.t]);
          SDL_SetRenderDrawColor(render, (*color)[0], (*color)[1], (*color)[2], 255);
    
          SDL_RenderFillRect(render, &b);
          SDL_RenderDrawRect(render, &b);

          SDL_SetRenderDrawColor(render, off.r, off.g, off.b, 255);
          w.y = b.y - 1;
          w.x = b.x - 1;
          SDL_RenderDrawRect(render, &w);
        }
      }

      for (int i = 0; i < 5; i++) {
        int index = (g.current_index + i);

        uint8_t mino;
        if (index <= 6) {
          mino = g.piece_queue[index];
        } else {
          index = index % 7;
          mino = g.next_queue[index];
        }

        uint8_t (*color)[3] = &(colors[mino]);
        uint8_t res = (default_matrices[mino][y]>>(3 - x))&1;

        if (res) {
          b.x = 20 + (W - (W / 4)) + (x * BLOCK_SIZE);
          b.y = (Y_OFFSET - 10) + (BLOCK_SIZE * i * 3) + (y * BLOCK_SIZE);
          if (mino == MINO_I) {
            b.x -= 10;
            b.y += 10;
          }
          if (mino == MINO_O) b.x -= 10;
          SDL_SetRenderDrawColor(render, (*color)[0], (*color)[1], (*color)[2], 255);
  
          SDL_RenderFillRect(render, &b);
          SDL_RenderDrawRect(render, &b);

          SDL_SetRenderDrawColor(render, off.r, off.g, off.b, 255);
          w.y = b.y - 1;
          w.x = b.x - 1;
          SDL_RenderDrawRect(render, &w);
        }
      }
    }
  }

  for (int x = 0; x < CETRIS_BOARD_X; x++) {
    for (int y = g.highest_line; y < CETRIS_BOARD_Y; y++) {
      if (g.board[x][y] & SLOT_OCCUPIED) {
        b.x = X_OFFSET + (x * BLOCK_SIZE) + 1;
        b.y = (Y_OFFSET + (y * BLOCK_SIZE)) - (CETRIS_BOARD_VISABLE * BLOCK_SIZE);
 
        if (g.line_remove_tick[y]) {
          if ((int)g.tick % 2 == 0) {
            continue;
          }
        }

        SDL_SetRenderDrawColor(render, off.r, off.g, off.b, 255);
        w.y = b.y - 1;
        w.x = b.x - 1;
        SDL_RenderDrawRect(render, &w);

        uint8_t (*color)[3] = &(colors[(g.board[x][y] >> 5)]);
        SDL_SetRenderDrawColor(render, (*color)[0], (*color)[1], (*color)[2], 255);
        SDL_RenderFillRect(render, &b);

        SDL_RenderDrawRect(render, &b);
      }
    }
  }

  char *buf = malloc(200);
  long double second = timer / 1000000.0f;
  if (second > 60.0f) {
    int minute = (int)(second / 60.0f);
    second -= (minute * 60.0f);
    sprintf(buf, "%02d:%09.6Lf", minute, second);
  } else {
    sprintf(buf, "%.6Lf", second);
  }
  draw_text(buf, 20, H - 40);

  sprintf(buf, "lines remaining: %i", 20 - g.lines);
  draw_text(buf, 20, H - 60);

  free(buf);

  SDL_SetRenderTarget(render, NULL);

  SDL_Point center = {W / 2, H / 2};
  SDL_RenderCopyEx(render, m, NULL, NULL, 0, &center, SDL_FLIP_NONE); 

  SDL_DestroyTexture(m);

  if (count_down > 0) {
	  count_down-=(1.0/60.0);
  }
}

void start_game() {
  g.waiting = false;
#ifdef _WIN32
  HANDLE thread = CreateThread(NULL, 0, game_loop, NULL, 0, NULL);
#else
  pthread_t thread;
  pthread_create(&thread, NULL, (void*)game_loop, (void*)0);
#endif
}

int main(void) {
  setup();

  ini_parser p;
  load_ini_file(&p, "config.ini");

  cetris_config config;
  
  int das = atoi(get_ini_value(&p, "das", "das"));
  config.das_das = das;

  int arr = atoi(get_ini_value(&p, "das", "arr"));
  config.das_arr = arr;

  config.drop_period = atoi(get_ini_value(&p, "game", "drop_delay"));
  config.next_piece_delay = atoi(get_ini_value(&p, "game", "next_piece_delay"));
  config.lock_delay = atoi(get_ini_value(&p, "game", "lock_delay"));
  config.force_lock = atoi(get_ini_value(&p, "game", "force_lock"));
  config.wait_on_clear = atoi(get_ini_value(&p, "game", "wait_on_clear"));
  config.line_delay_clear = atoi(get_ini_value(&p, "game", "line_clear_delay"));

  dark_mode = atoi(get_ini_value(&p, "ui", "dark_mode"));
  load_colors();
   
  count_down = 3;  
  init_game(&g, &config);
  g.waiting = true;
   
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
	      game_running = false;
	      init_game(&g, &config);
  	      g.waiting = true;
	      count_down = 3;
	      break;
          }
          break;
        case SDL_KEYUP:
          switch (e.key.keysym.sym) {
            case SDLK_LEFT:
              unhold_piece(&g, LEFT); break;
            case SDLK_RIGHT:
              unhold_piece(&g, RIGHT); break;
            case SDLK_DOWN:
              unhold_piece(&g, DOWN); break;
            case SDLK_SPACE:
              unhold_piece(&g, HARD_DROP); break;
            case SDLK_UP:
              unhold_piece(&g, ROTATE_CW); break;
            case 'x':
              unhold_piece(&g, ROTATE_CW); break;
            case 'z':
              unhold_piece(&g, ROTATE_CCW); break;
          }
      }
    }
    draw();

    if (count_down < 0 && !game_running) {
	    start_game();
	    game_running = true;
    }
    SDL_RenderPresent(render);
    SDL_Delay(1000 / 60);
  }

}
