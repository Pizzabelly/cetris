#include <SDL2/SDL.h>

#include "cetris.h"

#define W 360 
#define H 640 

struct cetris_game g;

SDL_Renderer* render;
SDL_Texture*  block;
SDL_Event e;

typedef struct {
  int r;
  int g;
  int b;
} sdl_color;

sdl_color colors[8] = {
  {0, 0, 0},     // Black
  {127,219,255}, // Aqua
  {61,153,112},  // Olive
  {177,13,201},  // Purple
  {240,18,190},  // Fuchsia
  {255,133,27},  // Orange
  {0,31,63},     // Navy
  {255,220,0}    // Yellow 
};

void setup() {
  SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
  SDL_Window* win = SDL_CreateWindow("Cetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H, SDL_WINDOW_SHOWN);
  render = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC);  
  if (!render) exit(fprintf(stderr, "[Error] Could not create SDL renderer\n")); 
  SDL_Surface* s = SDL_LoadBMP("block.bmp");
  block = SDL_CreateTextureFromSurface(render, s);
  SDL_FreeSurface(s);
}

void draw_stuff() {
  SDL_RenderClear(render);

  SDL_SetRenderDrawColor(render, 255, 255, 255, 124);
  for (int x = 0; x < BOARD_X + 1; x++) {
    SDL_RenderDrawLine(render, 1 + (x * 25), 1, 1 + (x * 25), 500);
  }
  for (int y = 0; y < BOARD_Y - BOARD_VISABLE + 1; y++) {
    SDL_RenderDrawLine(render, 1, 1 + (y * 25), 250, 1 + (y * 25));
  }
  SDL_SetRenderDrawColor(render, 0, 0, 0, 255);

  sdl_color c;
  SDL_Rect r = {0, 0, 25, 25};
  for (int x = 0; x < BOARD_X; x++) {
    for (int y = BOARD_VISABLE; y < BOARD_Y; y++) {
      if (g.board[x][y].occupied) {
        c = colors[g.board[x][y].c];
        SDL_SetTextureColorMod(block, c.r, c.g, c.b);
        r.x = x * 25;
        r.y = (y - BOARD_VISABLE) * 25;
        SDL_RenderCopy(render, block, NULL, &r);
      }
    }
  }
  SDL_RenderPresent(render);
}

int main(void) {
  init_game(&g);
  setup();
  for(;;) {
    while(SDL_PollEvent(&e)) {
      switch (e.type) {
        case SDL_QUIT:
          exit(0);
        case SDL_KEYDOWN:
          switch (e.key.keysym.sym) {
            case SDLK_LEFT:
              move_left(&g); break;
            case SDLK_RIGHT:
              move_right(&g); break;
            case SDLK_DOWN:
              move_down(&g); break;
            case SDLK_SPACE:
              move_hard_drop(&g); break;
            case SDLK_UP:
              rotate_clockwise(&g); break;
          }
      }
    }
    update_game_tick(&g);
    draw_stuff();
    SDL_Delay(1000 / CETRIS_HZ);
  }
}
