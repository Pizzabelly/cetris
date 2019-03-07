#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <locale.h>

#include "cetris.h"

#ifdef ASCII_COMPATIBLE
#define BLOCK "[]"
#define PLAY_FIELD_STR  "       /--------------------\\    /----------------\\\n"\
                        "       |                    |    |                |\n"\
                        "       |                    |    \\----------------/\n"\
                        "       |                    |                      \n"\
                        "       |                    |                      \n"\
                        "       |                    |                      \n"\
                        "       |                    |       /---------\\   \n"\
                        "       |                    |       |         |    \n"\
                        "       |                    |       |         |    \n"\
                        "       |                    |       |         |    \n"\
                        "       |                    |       |         |    \n"\
                        "       |                    |       \\---------/   \n"\
                        "       |                    |                      \n"\
                        "       |                    |                      \n"\
                        "       |                    |                      \n"\
                        "       |                    |                      \n"\
                        "       |                    |                      \n"\
                        "       |                    |                      \n"\
                        "       |                    |                      \n"\
                        "       |                    |                      \n"\
                        "       |                    |                      \n"\
                        "       \\--------------------/"
#else
#define BLOCK "[]"
#define PLAY_FIELD_STR  "       ┏━━━━━━━━━━━━━━━━━━━━┓  ┏━━━━━━━━━━━━━━━┓  \n"\
                        "       ┃                    ┃  ┃               ┃  \n"\
                        "       ┃                    ┃  ┗━━━━━━━━━━━━━━━┛  \n"\
                        "       ┃                    ┃                     \n"\
                        "       ┃                    ┃                     \n"\
                        "       ┃                    ┃    ┏━━━━━━━━━━━┓    \n"\
                        "       ┃                    ┃    ┃           ┃    \n"\
                        "       ┃                    ┃    ┃           ┃    \n"\
                        "       ┃                    ┃    ┃           ┃    \n"\
                        "       ┃                    ┃    ┃           ┃    \n"\
                        "       ┃                    ┃    ┗━━━━━━━━━━━┛    \n"\
                        "       ┃                    ┃                     \n"\
                        "       ┃                    ┃                     \n"\
                        "       ┃                    ┃                     \n"\
                        "       ┃                    ┃                     \n"\
                        "       ┃                    ┃                     \n"\
                        "       ┃                    ┃                     \n"\
                        "       ┃                    ┃                     \n"\
                        "       ┃                    ┃                     \n"\
                        "       ┃                    ┃                     \n"\
                        "       ┃                    ┃                     \n"\
                        "       ┗━━━━━━━━━━━━━━━━━━━━┛                        "
#endif

#define X_OFFSET 8
#define Y_OFFSET 0

struct cetris_game game;

void curses_init() {
  setlocale(LC_CTYPE, "");
  initscr();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  timeout(1000 / CETRIS_HZ);

  start_color();
  init_pair(COLOR_NONE, COLOR_BLACK, COLOR_BLACK);
  init_pair(COLOR_O, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_Z, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_S, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_T, COLOR_WHITE, COLOR_BLACK);
  init_pair(COLOR_L, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_I, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_J, COLOR_YELLOW, COLOR_BLACK);
  clear();
}

void draw_board() {
  mvaddstr(0, 0, PLAY_FIELD_STR);
  for (int x = 0; x < BOARD_X; x++) {
    for (int y = BOARD_VISABLE; y < BOARD_Y; y++) {
      if (game.board[x][y].occupied) {
	      attron(COLOR_PAIR(game.board[x][y].c));
        if (game.board[x][y].remove_tick > 0) {
          if (game.tick % 2 == 0) {
            mvaddstr((y - BOARD_VISABLE) + 1, x * 2 + X_OFFSET, BLOCK);
          }
        } else {
          mvaddstr((y - BOARD_VISABLE) + 1, x * 2 + X_OFFSET, BLOCK);
        }
	      attroff(COLOR_PAIR(game.board[x][y].c));
      }
    }

    int index = game.current_index;
	  attron(COLOR_PAIR(game.piece_queue[index].c));
    for (int x = 0; x < 4; x++) {
      for (int y = 0; y < 4; y++) {
        if (game.piece_queue[index].mat[y][x]) {
          if (game.piece_queue[index].t == I) {
            mvaddstr(6 + y, (x * 2) + 36, BLOCK);
          } else {
            mvaddstr(6 + y, (x * 2) + 37, BLOCK);
          }
        }
      }
    }
	  attroff(COLOR_PAIR(game.piece_queue[index].c));

    attron(A_BOLD);

    char score[50];
    sprintf(score, "%li", game.score);
    mvaddstr(1, (39 + X_OFFSET) - strlen(score), score);

    char level[20];
    sprintf(level, "%i", game.level);
    mvaddstr(3, 37, "Level"); 
    mvaddstr(4, 40 - strlen(level), level); 

    if (game.game_over) {
      mvaddstr(10, 6 + X_OFFSET, "GAME OVER");
      mvaddstr(11, 5 + X_OFFSET, "r to restart");
    }

    attroff(A_BOLD);
  }
}

int main(void) {
  curses_init();

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
        move_hard_drop(&game); break;
      case 'r':
        if (game.game_over) {
          init_game(&game);
        }
        break;
    }
    update_game_tick(&game);
    erase();
    draw_board(&game);
    refresh();
  }
  return 0;
}

