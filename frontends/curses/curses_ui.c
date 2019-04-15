#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define ASCII_COMPATIBLE
#include "win\curses.h"
#include <windows.h>
#else
#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#endif
#include <locale.h>

#include "cetris.h"

#ifdef ASCII_COMPATIBLE
#define BLOCK "[]"
#define PLAY_FIELD_STR  "       /--------------------\\   /----------------\\\n"\
                        "       |                    |   |                |\n"\
                        "       |                    |   \\----------------/\n"\
                        "       |                    |                      \n"\
                        "       |                    |                      \n"\
                        "       |                    |      /---------\\   \n"\
                        "       |                    |      |         |    \n"\
                        "       |                    |      |         |    \n"\
                        "       |                    |      |         |    \n"\
                        "       |                    |      |         |    \n"\
                        "       |                    |      \\---------/   \n"\
                        "       |                    |                      \n"\
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

cetris_game game;

void curses_init() {
  setlocale(LC_CTYPE, "");
  initscr();
  curs_set(0);
  //noecho();
  keypad(stdscr, TRUE);
  //timeout(1000 / CETRIS_HZ);
  nodelay(stdscr, true);

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

#ifdef _WIN32
DWORD WINAPI game_loop(void* data) {
  while(1) {
    Sleep((1.0/60.0) * 1000.0);
    update_game_tick(&game);
  }
  return 0;
}
#else
void *game_loop(void) {
  while(1) {
    usleep((1.0/60.0) * 1000000.0);
    update_game_tick(&game);
  }
  return 0;
}
#endif


void draw_board() {
  mvaddstr(0, 0, PLAY_FIELD_STR);
  for (int x = 0; x < CETRIS_BOARD_X; x++) {
    for (int y = 0; y < CETRIS_BOARD_Y; y++) {
      if (game.board[x][y].ghost) {
        attron(A_DIM);
        mvaddstr((y) + 1, x * 2 + X_OFFSET, BLOCK);
        attroff(A_DIM);
      }
      if (game.board[x][y].occupied) {
	      attron(COLOR_PAIR(game.board[x][y].c) | A_BOLD);
        if (game.board[0][y].remove_tick) {
          if (game.tick % 2 == 0) {
            mvaddstr((y) + 1, x * 2 + X_OFFSET, BLOCK);
          }
        } else {
          mvaddstr((y) + 1, x * 2 + X_OFFSET, BLOCK);
        }
	      attroff(COLOR_PAIR(game.board[x][y].c) | A_BOLD);
      }
    }

    int index = game.current_index;
	  attron(COLOR_PAIR(game.piece_queue[index].c));
    for (int x = 0; x < 4; x++) {
      for (int y = 0; y < 4; y++) {
        if (game.piece_queue[index].m[y][x]) {
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

  int down = 0;
  while(1) {
    int keys[50]; int key_count = 0;
    int moves[8]; memset(moves, 0, sizeof(int) * 8);
    while((keys[key_count] = getch()) != ERR) key_count++;
    if (down) move_piece(&game, USER_DOWN);
    for (int i = 0; i < key_count; i++) {
      switch (keys[i]) {
        case 'q': endwin(); exit(1);
        case KEY_LEFT:
          move_piece(&game, LEFT); 
          moves[LEFT] = 1; break;
        case KEY_RIGHT:
          move_piece(&game, RIGHT);
          moves[RIGHT] = 1; break;
        case KEY_DOWN:
          down = 1;
          moves[USER_DOWN] = 1; break;
        case KEY_UP:
          move_piece(&game, ROTATE_CW);
          moves[ROTATE_CW] = 1; break;
        case ' ':
          move_piece(&game, HARD_DROP);
          moves[HARD_DROP] = 1; break;
        case 'r':
          if (game.game_over) {
            init_game(&game);
          }
          break;
      }
    }
    if (!moves[USER_DOWN]) down = 0;
    for (int i = 1; i < 8; i++) {
      if (!moves[i]) stop_holding(&game, i);
    }
    erase();
    draw_board();
    refresh();
  }
  return 0;
}
