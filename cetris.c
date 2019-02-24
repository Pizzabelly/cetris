#include <stdio.h>
#include <locale.h>
#include <ncurses.h>

#include "cetris.h"

void add_piece(type t) {
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      if (pieces[t][y][x]) {
        g.board[x + 2][y] = 1;
      }
    }
  }
}
