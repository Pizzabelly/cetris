#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <cetris.h>

#define W 480
#define H 640

#define X_OFFSET 40
#define Y_OFFSET 30

SDL_Renderer* render;
TTF_Font* font;

cetris_game g;

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

#ifdef _WIN32
DWORD WINAPI game_loop(void* data) {
  while(1) {
    Sleep((1.0/60.0) * 1000.0);
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
  SDL_Window* win = SDL_CreateWindow("cetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H, SDL_WINDOW_SHOWN);
  render = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_ACCELERATED);
  SDL_RenderSetLogicalSize(render, W, H);
  if (!render) exit(fprintf(stderr, "err: could not create SDL renderer\n")); 

  //TTF_Init();
  //font = TTF_OpenFont("LiberationMono-Regular.ttf", 18);
}

void draw() {
  SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
  SDL_RenderClear(render);

  SDL_Rect b = {0, 0, 25, 25}; 

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if ((g.current.m[y]>>(3 - x))&1) {
        b.x = X_OFFSET + ((x + g.current.pos.x) * 25);
        b.y = (Y_OFFSET + ((y + g.current.pos.y) * 25)) - (CETRIS_BOARD_VISABLE * 25);

        uint8_t (*color)[3] = &(colors[g.current.t]);
        SDL_SetRenderDrawColor(render, (*color)[0], (*color)[1], (*color)[2], 255);

        SDL_RenderDrawRect(render, &b);

        b.y = (Y_OFFSET + ((y + g.current.ghost_y) * 25)) - (CETRIS_BOARD_VISABLE * 25);
  
        SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
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
        b.x = X_OFFSET + (x * 25);
        b.y = (Y_OFFSET + (y * 25)) - (CETRIS_BOARD_VISABLE * 25);
 
        uint8_t (*color)[3] = &(colors[(g.board[x][y] >> 5)]);
        SDL_SetRenderDrawColor(render, (*color)[0], (*color)[1], (*color)[2], 255);

        if (g.line_remove_tick[y]) {
          if (g.tick % 2 == 0) {
            SDL_RenderDrawRect(render, &b);
          }
        } else {
          SDL_RenderDrawRect(render, &b);
        }
      }
    }
  }
  SDL_RenderPresent(render);
}

int main(void) {
  setup();

  init_game(&g);

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
              move_piece(&g, DOWN); break;
            case SDLK_SPACE:
              move_piece(&g, HARD_DROP); break;
            case SDLK_UP:
              move_piece(&g, ROTATE_CW); break;
          }
      }
    }
    draw();
    SDL_Delay(1000 / 60);
  }

}
