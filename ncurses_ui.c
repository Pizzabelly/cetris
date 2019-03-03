#include <stdlib.h>
#include <ncurses.h>
#include <locale.h>

#include "cetris.h"

#define BLOCK "[]"
#define PLAY_FIELD_STR  "       /--------------------\\    /--------\\\n"\
                        "       |                    |    |        |\n"\
                        "       |                    |    \\--------/\n"\
                        "       |                    |              \n"\
                        "       |                    |              \n"\
                        "       |                    |              \n"\
                        "       |                    |              \n"\
                        "       |                    |              \n"\
                        "       |                    |              \n"\
                        "       |                    |              \n"\
                        "       |                    |              \n"\
                        "       |                    |              \n"\
                        "       |                    |              \n"\
                        "       |                    |              \n"\
                        "       |                    |              \n"\
                        "       |                    |              \n"\
                        "       |                    |              \n"\
                        "       |                    |              \n"\
                        "       |                    |              \n"\
                        "       |                    |              \n"\
                        "       |                    |              \n"\
                        "       \\--------------------/"

#define X_OFFSET 8
#define Y_OFFSET 0

void curses_init() {
  setlocale(LC_CTYPE, "");
  initscr();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  timeout(1000 / CETRIS_HZ);

  start_color();
  init_pair(0, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  init_pair(2, COLOR_BLUE, COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_BLACK);
  clear();
}

void draw_board(struct cetris_game* g) {
  mvaddstr(0, 0, PLAY_FIELD_STR);
  for (int x = 0; x < BOARD_X; x++) {
    for (int y = 0; y < BOARD_Y; y++) {
      if (g->board[x][y].occupied) {
        if (g->board[x][y].remove_tick > 0) {
          if (g->tick % 2 == 0) {
            mvaddstr(y + 1, x * 2 + X_OFFSET, BLOCK);
          }
        } else {
          mvaddstr(y + 1, x * 2 + X_OFFSET, BLOCK);
        }
      }
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
      case KEY_LEFT:
        move_left(&game); break;
      case KEY_RIGHT:
        move_right(&game); break;
      case KEY_DOWN:
        move_down(&game); break;
      case KEY_UP:
        rotate_clockwise(&game); break;
      case ' ':
        move_hard_drop(&game);
    }
    update_game_tick(&game);
    draw_board(&game);
    refresh();
  }
  return 0;
}

