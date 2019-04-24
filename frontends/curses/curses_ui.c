/* very simple curses interface, DAS is not functional because you cant detect 
 * key up presses on the terminal, there are 2 character sets, ASCII_COMPATIBLE
 * for no UTF-8 chars and the normal which will look better on a platform that can
 * support UTF-8. Color support is limited because I havent figured out colors yet */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#ifdef _WIN32
#include "win\curses.h"
#include <windows.h>
#else
#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#endif
#include <locale.h>

#include "cetris.h"

#define WIN_COL 55 
#define WIN_LINE 25 

#ifdef ASCII_COMPATIBLE
#define BLOCK "[]"
#define PLAY_FIELD_STR  "       /--------------------\\   /----------------\\ \n"\
                        "       |                    |   |                |   \n"\
                        "       |                    |   \\----------------/  \n"\
                        "       |                    |                        \n"\
                        "       |                    |                        \n"\
                        "       |                    |      /---------\\      \n"\
                        "       |                    |      |         |       \n"\
                        "       |                    |      |         |       \n"\
                        "       |                    |      |         |       \n"\
                        "       |                    |      |         |       \n"\
                        "       |                    |      \\---------/      \n"\
                        "       |                    |                        \n"\
                        "       |                    |                        \n"\
                        "       |                    |                        \n"\
                        "       |                    |                        \n"\
                        "       |                    |                        \n"\
                        "       |                    |                        \n"\
                        "       |                    |                        \n"\
                        "       |                    |                        \n"\
                        "       |                    |                        \n"\
                        "       |                    |                        \n"\
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
#define Y_OFFSET 1

cetris_game game;
bool is_paused = false;

void curses_init() {
  setlocale(LC_CTYPE, "");
  initscr();
  curs_set(0);
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, true);

#ifdef _WIN32 // only resize manually on windows
  resize_term(WIN_LINE, WIN_COL);
#endif

  start_color();
  init_pair(COLOR_NONE, COLOR_BLACK, COLOR_BLACK);
  init_pair(COLOR_O, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_Z, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_S, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_T, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_L, COLOR_WHITE, COLOR_BLACK); // should be orange
  init_pair(COLOR_I, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_J, COLOR_BLUE, COLOR_BLACK);
  clear();
}

#ifdef _WIN32
DWORD WINAPI game_loop(void* data) {
  while(1) {
    Sleep((1.0/60.0) * 1000.0);
    if (!is_paused) update_game_tick(&game);
  }
  return 0;
}
#else
void *game_loop(void) {
  while(1) {
    usleep((1.0/60.0) * 1000000.0);
    if (!is_paused) update_game_tick(&game);
  }
  return 0;
}
#endif


void draw_board() {
  mvaddstr(0, 0, PLAY_FIELD_STR);
  for (int x = 0; x < CETRIS_BOARD_X; x++) {
    for (int y = CETRIS_BOARD_VISABLE; y < CETRIS_BOARD_Y; y++) {
      int draw_y = y - CETRIS_BOARD_VISABLE + Y_OFFSET;
      int draw_x = x * 2 + X_OFFSET;
      if (game.board[x][y].ghost) {
        attron(A_DIM);
        mvaddstr(draw_y, draw_x, BLOCK);
        attroff(A_DIM);
      }
      if (game.board[x][y].occupied) {
	      attron(COLOR_PAIR(game.board[x][y].c) | A_BOLD);
        if (game.board[0][y].remove_tick) {
          if (game.tick % 2 == 0) {
            mvaddstr(draw_y, draw_x, BLOCK);
          }
        } else {
          mvaddstr(draw_y, draw_x, BLOCK);
        }
	      attroff(COLOR_PAIR(game.board[x][y].c) | A_BOLD);
      }
    }

    int index = game.current_index;
	  attron(COLOR_PAIR(game.piece_queue[index].c));
    for (int x = 0; x < 4; x++) {
      for (int y = 0; y < 4; y++) {
        if (game.piece_queue[index].m[y][x]) {
          if (game.piece_queue[index].t == I || game.piece_queue[index].t == O) {
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
    sprintf(score, "%i", game.score);
    mvaddstr(1, (39 + X_OFFSET) - strlen(score), score);

    char level[20];
    sprintf(level, "%i", game.level);
    mvaddstr(3, 37, "Level"); 
    mvaddstr(4, 40 - strlen(level), level); 

    if (game.game_over) {
      mvaddstr(10, 5 + X_OFFSET, "GAME OVER");
      mvaddstr(11, 4 + X_OFFSET, "r to restart");
    }

    if (is_paused) {
      mvaddstr(10, 7 + X_OFFSET, "paused");
    }

    attroff(A_BOLD);
  }
}

int main(void) {
  curses_init();

  init_game(&game);

#ifdef _WIN32
  HANDLE thread = CreateThread(NULL, 0, game_loop, NULL, 0, NULL);
#else
  pthread_t thread;
  pthread_create(&thread, NULL, (void*)game_loop, (void*)0);
#endif

  while(1) {
    int keys[25]; int key_count = 0;
    while((keys[key_count] = getch()) != ERR) key_count++;
    for (int i = 0; i < key_count; i++) {
      switch (keys[i]) {
        case 'q': endwin(); exit(1);
        case 27: // esc or alt
          is_paused = !is_paused; break;
        case 'r':
          if (game.game_over) {
            init_game(&game);
          }
          break;
      }
      if (is_paused) continue; // dont allow input if paused
      switch (keys[i]) {
        case KEY_LEFT:
          move_piece(&game, LEFT, false); break;
        case KEY_RIGHT:
          move_piece(&game, RIGHT, false); break;
        case KEY_DOWN:
          move_piece(&game, DOWN, false); break;
        case KEY_UP:
        case 'x':
          move_piece(&game, ROTATE_CW, false); break;
        case '^':
        case 'z':
          move_piece(&game, ROTATE_CCW, false); break;
        case ' ':
          move_piece(&game, HARD_DROP, false); break;
        case KEY_SLEFT:
        case 'c':
          hold_piece(&game); break;
      }
    }
    erase();
    draw_board();
    refresh();
  }
  return 0;
}

