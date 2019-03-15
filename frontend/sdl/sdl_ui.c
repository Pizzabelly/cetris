#ifdef _WIN32
#define SDL_DISABLE_IMMINTRIN_H
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#endif
#include <stdio.h>
#include <string.h>

#include "cetris.h"

#define W 430 
#define H 550 
#define BOARD_OFFSET_X 25 
#define BOARD_OFFSET_Y 20 

struct cetris_game g;

SDL_Renderer* render;
SDL_Texture*  block;
SDL_Event e;
TTF_Font* font;
TTF_Font* big_font;

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
  render = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_ACCELERATED);
  SDL_RenderSetLogicalSize(render, W, H);
  if (!render) exit(printf("[Error] Could not create SDL renderer\n")); 
  SDL_Surface* s = SDL_LoadBMP("block.bmp");
  block = SDL_CreateTextureFromSurface(render, s);
  SDL_FreeSurface(s);

  TTF_Init();
  font = TTF_OpenFont("LiberationMono-Regular.ttf", 18);
  big_font = TTF_OpenFont("LiberationMono-Regular.ttf", 30);
}

void draw_text(char* string, int x, int y, int black) {
  int w, h;
  char msg[80];
  snprintf(msg, 80, string);
  TTF_SizeText(font, msg, &w, &h);
  SDL_Surface* msgsurf;
  if (black) {
    msgsurf = TTF_RenderText_Blended(big_font, msg, (SDL_Color){20, 20, 30, 255});
  } else {
    msgsurf = TTF_RenderText_Blended(font, msg, (SDL_Color){255, 255, 255, 255});
  }
  SDL_Texture *msgtex = SDL_CreateTextureFromSurface(render, msgsurf);
  SDL_Rect fromrec = {0, 0, msgsurf->w, msgsurf->h};
  SDL_Rect torec = {x, y, msgsurf->w, msgsurf->h};
  SDL_RenderCopy(render, msgtex, &fromrec, &torec);
  SDL_DestroyTexture(msgtex);
  SDL_FreeSurface(msgsurf);
}

void draw_stuff() {
  SDL_SetRenderDrawColor(render, 100, 100, 112, 255);
  SDL_RenderClear(render);

  SDL_Rect b = {BOARD_OFFSET_X, BOARD_OFFSET_Y, 250, 500};
  SDL_SetRenderDrawColor(render, 120, 120, 132, 255);
  SDL_RenderFillRect(render, &b);
  SDL_RenderDrawRect(render, &b);


  SDL_Rect next = {294, 20, 125, 125};
  SDL_RenderFillRect(render, &next);
  SDL_RenderDrawRect(render, &next);
  int index = g.current_index;
  sdl_color nc = colors[g.piece_queue[index].c];
  SDL_SetTextureColorMod(block, nc.r, nc.g, nc.b);
  SDL_Rect p = {294, 20, 25, 25};
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      if (g.piece_queue[index].m[y][x]) {
        p.x = 319 + (x * 25);
        p.y = 30 + (y * 25);
        if (g.piece_queue[index].t == I) {
          p.x = 306 + (x * 25);
        }
        if (g.piece_queue[index].t == O) {
          p.x = 306 + (x * 25);
        }
        SDL_RenderCopy(render, block, NULL, &p);
      }
    }
  }

  SDL_SetRenderDrawColor(render, 76, 70, 72, 255);

  int y_off = 140;

  SDL_Rect s = {290, 20 + y_off, 135, 42};
  SDL_RenderFillRect(render, &s);
  SDL_RenderDrawRect(render, &s);
  draw_text("SCORE", 295, 23 + y_off, 0);
  char* score = malloc(sizeof(char) * 25);
  sprintf(score, "%li", g.score);
  draw_text(score, 294 + strlen(score), 40 + y_off, 0);
  free(score);

  SDL_Rect l = {290, 70 + y_off, 135, 42};
  SDL_RenderFillRect(render, &l);
  SDL_RenderDrawRect(render, &l);
  draw_text("LEVEL", 295, 73 + y_off, 0);
  char* level = malloc(sizeof(char) * 25);
  sprintf(level, "%i", g.level);
  draw_text(level, 294 + strlen(level), 90 + y_off, 0);
  free(level);

  SDL_Rect l1 = {290, 120 + y_off, 135, 42};
  SDL_RenderFillRect(render, &l1);
  SDL_RenderDrawRect(render, &l1);
  draw_text("LINES", 295, 123 + y_off, 0);
  char* lines = malloc(sizeof(char) * 25);
  sprintf(lines, "%i", g.lines);
  draw_text(lines, 294 + strlen(lines), 140 + y_off, 0);
  free(lines);

  SDL_SetRenderDrawColor(render, 255, 255, 255, 124);
  for (int x = 0; x < CETRIS_BOARD_X + 1; x++) {
    int rx = BOARD_OFFSET_X + 1 + (x * 25);
    SDL_RenderDrawLine(render, rx, BOARD_OFFSET_Y + 1, rx, BOARD_OFFSET_Y + 500);
  }
  for (int y = 0; y < CETRIS_BOARD_Y - CETRIS_BOARD_VISABLE + 1; y++) {
    int ry = BOARD_OFFSET_Y + (y * 25);
    SDL_RenderDrawLine(render, BOARD_OFFSET_X + 1, ry, BOARD_OFFSET_X + 250, ry);
  }
  SDL_SetRenderDrawColor(render, 0, 0, 0, 255);

  sdl_color c;
  SDL_Rect r = {0, 0, 25, 25};
  for (int x = 0; x < CETRIS_BOARD_X; x++) {
    for (int y = CETRIS_BOARD_VISABLE; y < CETRIS_BOARD_Y; y++) {
      if (g.board[x][y].occupied) {
        c = colors[g.board[x][y].c];
        SDL_SetTextureColorMod(block, c.r, c.g, c.b);
        r.x = BOARD_OFFSET_X + x * 25;
        r.y = BOARD_OFFSET_Y + (y - CETRIS_BOARD_VISABLE) * 25;
        SDL_RenderCopy(render, block, NULL, &r);
      }
    }
  }
  
  if (g.game_over) {
    draw_text("GAME OVER", 76, 220, 1);
    draw_text("r to restart", 48, 255, 1);
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
            case SDLK_r:
              if (g.game_over) init_game(&g);
          }
	  break;
        case SDL_KEYUP:
	  switch (e.key.keysym.sym) {
	    case SDLK_LEFT:
	    case SDLK_RIGHT:
	    case SDLK_DOWN:
	    case SDLK_SPACE:
	    case SDLK_UP:
	      clear_held_key(&g);
	      break;
	  }
	  break;
      }
    }
    update_game_tick(&g);
    draw_stuff();
    SDL_Delay(1000 / CETRIS_HZ);
  }
}
