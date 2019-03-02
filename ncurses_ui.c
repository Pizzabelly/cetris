#include <stdlib.h>
#include <ncurses.h>
#include <locale.h>

#include "cetris.h"

#define BLOCK "[]"
#define BOARD_TOP    "/--------------------\\"
#define BOARD_MID    "|                    |"
#define BOARD_BOTTOM "\\--------------------/"

//static int term_x, term_y;

void curses_init() {
  setlocale(LC_CTYPE, "");
  initscr();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  timeout(1000 / 60);

  //resize_term(20, 50);

  start_color();
  init_pair(0, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  init_pair(2, COLOR_BLUE, COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_BLACK);
  clear();
}

void draw_board(struct cetris_game* g) {
  for (int y = 0; y < BOARD_Y + 2; y++) {
    if (y == 0) 
      mvaddstr(y, 10, BOARD_TOP);
    else if (y == BOARD_Y + 1) 
      mvaddstr(y, 10, BOARD_BOTTOM);
    else 
      mvaddstr(y, 10, BOARD_MID);
  }
  for (int x = 0; x < BOARD_X; x++) {
    for (int y = 0; y < BOARD_Y; y++) {
      if (g->board[x][y].occupied) 
        mvaddstr(y + 1, x * 2 + 11, BLOCK);
    }
  }
}

int main(void) {
  curses_init();

  struct cetris_game game;
  init_game(&game);

  int c;
  while(1) {
    c = getch();
    switch (c) {
      case 'q': endwin(); exit(1);
      case 'a':
        move_left(&game); break;
      case 'd':
        move_right(&game); break;
      case 's':
        move_down(&game); break;
      case 'w':
        rotate_clockwise(&game); break;
    }
    update_game_tick(&game);
    draw_board(&game);
    refresh();
  }
  return 0;
}

