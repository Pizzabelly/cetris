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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <time.h>

#include <cetris.h>
#include <ini.h>

#define W 480
#define H 640

#define BLOCK_SIZE 25

#define X_OFFSET (W/2) - (BLOCK_SIZE * 5)
#define Y_OFFSET (H/2) - (BLOCK_SIZE * 10)

SDL_Renderer* render;
TTF_Font* font;
SDL_Window *window;
SDL_Surface *screen;

cetris_game g;

/*
uint8_t colors[8][3] = {
  {0, 0, 0},     // Black
  {127,219,255}, // Aqua
  {61,153,112},  // Olive
  {177,13,201},  // Purple
  {240,18,190},  // Fuchsia
  {255,133,27},  // Orange
  {0,31,63},     // Navy
  {255,220,0}    // Yellow 
};
*/

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
  LONGLONG nsec = (LONGLONG)(10000000L/CETRIS_HZ);
  while(1) {
    ElapsedMicroseconds.QuadPart = 0;
    QueryPerformanceCounter(&StartingTime);
    while (ElapsedMicroseconds.QuadPart <= nsec) {
        QueryPerformanceCounter(&EndingTime); 
	ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
    }
    update_game_tick(&g);
  }
  return 0;
}
#else
void *game_loop(void) {
  long nsec = (long)(1000000000L/CETRIS_HZ);
  while(1) {
    nanosleep((const struct timespec[]){{0, nsec}}, NULL);
    update_game_tick(&g);
  }
  return 0;
}
#endif

void setup() {
  SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
  window = SDL_CreateWindow("cetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H, SDL_WINDOW_SHOWN);
  screen = SDL_GetWindowSurface(window);
  render = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_ACCELERATED);
  SDL_RenderSetLogicalSize(render, W, H);
  if (!render) exit(fprintf(stderr, "err: could not create SDL renderer\n")); 
  SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);

  TTF_Init();
  font = TTF_OpenFont("LiberationMono-Regular.ttf", 18);
}

void draw_text(char* string, int x, int y) {
  SDL_Color black = {0, 0, 0, 255};
  SDL_Surface* surface = TTF_RenderText_Solid(font, string, black);
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

void draw() {
  SDL_Texture *m = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, W, H);
  SDL_SetRenderTarget(render, m);

  SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
  SDL_RenderClear(render);

  SDL_Rect board = {X_OFFSET, Y_OFFSET, 250, 500};
  SDL_SetRenderDrawColor(render, 235, 235, 235, 255);
  SDL_RenderFillRect(render, &board);
  SDL_RenderDrawRect(render, &board);

  SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
  for (int x = 0; x < CETRIS_BOARD_X + 1; x++) {
    int rx = X_OFFSET + 1 + (x * BLOCK_SIZE);
    SDL_RenderDrawLine(render, rx, Y_OFFSET + 1, rx, Y_OFFSET + 500);
  }
  for (int y = 0; y < CETRIS_BOARD_Y - CETRIS_BOARD_VISABLE + 1; y++) {
    int ry = Y_OFFSET + (y * BLOCK_SIZE);
    SDL_RenderDrawLine(render, X_OFFSET + 1, ry, X_OFFSET + 250, ry);
  }

  SDL_Rect b = {0, 0, BLOCK_SIZE - 1, BLOCK_SIZE - 1}; 
  SDL_Rect w = {0, 0, BLOCK_SIZE + 1, BLOCK_SIZE + 1}; 

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if ((g.current.m[y]>>(3 - x))&1) {
        b.x = X_OFFSET + ((x + g.current.pos.x) * BLOCK_SIZE);
        b.y = (Y_OFFSET + ((y + g.current.pos.y) * BLOCK_SIZE)) - (CETRIS_BOARD_VISABLE * BLOCK_SIZE);
  
        uint8_t (*color)[3] = &(colors[g.current.t]);
        SDL_SetRenderDrawColor(render, (*color)[0], (*color)[1], (*color)[2], 255);
  
        SDL_RenderFillRect(render, &b);
        SDL_RenderDrawRect(render, &b);

        SDL_SetRenderDrawColor(render, 240, 240, 240, 255);
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
      //if ((default_matrices[game.piece_queue[g.current_index]][y]>>(3 - x))&1) {
      //  mvaddstr(6 + y, (x * 2) + 36, BLOCK);
      //}
    }
  }

  for (int x = 0; x < CETRIS_BOARD_X; x++) {
    for (int y = g.highest_line; y < CETRIS_BOARD_Y; y++) {
      if (g.board[x][y] & SLOT_OCCUPIED) {
        b.x = X_OFFSET + (x * BLOCK_SIZE);
        b.y = (Y_OFFSET + (y * BLOCK_SIZE)) - (CETRIS_BOARD_VISABLE * BLOCK_SIZE);
 
        if (g.line_remove_tick[y]) {
          if ((int)g.tick % 2 == 0) {
            continue;
          }
        }

        SDL_SetRenderDrawColor(render, 240, 240, 240, 255);
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

  long double time = g.tick / 60.0f;
  char *buf = malloc(200);
  sprintf(buf, "%.18Lf", time);
  draw_text(buf, 20, 20);
  free(buf);

  SDL_SetRenderTarget(render, NULL);
  SDL_Point center = {W / 2, H / 2};
  SDL_RenderCopyEx(render, m, NULL, NULL, 0, &center, SDL_FLIP_NONE); 

  SDL_RenderPresent(render);
}

int main(void) {
  setup();

  init_game(&g);
  
  ini_parser p;
  load_ini_file(&p, "config.ini");
  
  int das = atoi(get_ini_value(&p, "das", "das"));
  g.config.das_das = das * .06f;

  int arr = atoi(get_ini_value(&p, "das", "arr"));
  g.config.das_arr = arr * .06f;

  g.config.drop_period = atoi(get_ini_value(&p, "game", "drop_delay")) * .06f;
  g.config.next_piece_delay = atoi(get_ini_value(&p, "game", "next_piece_delay"));
  g.config.lock_delay = atoi(get_ini_value(&p, "game", "lock_delay"));
  g.config.wait_on_clear = atoi(get_ini_value(&p, "game", "wait_on_clear"));
  g.config.line_delay_clear = atoi(get_ini_value(&p, "game", "line_clear_delay"));

#ifdef _WIN32
  HANDLE thread = CreateThread(NULL, 0, game_loop, NULL, 0, NULL);
#else
  pthread_t thread;
  pthread_create(&thread, NULL, (void*)game_loop, (void*)0);
#endif

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
              move_piece(&g, USER_DOWN); break;
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
          }
          break;
        case SDL_KEYUP:
          switch (e.key.keysym.sym) {
            case SDLK_LEFT:
              unhold_piece(&g, LEFT); break;
            case SDLK_RIGHT:
              unhold_piece(&g, RIGHT); break;
            case SDLK_DOWN:
              unhold_piece(&g, USER_DOWN); break;
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
    SDL_Delay(1000 / 144);
  }

}
