#include <stdio.h>

#include "cetris.h"

int main(void) {
  cetris_game game;

  init_game(&game, NULL);

  printf("memory usage of game instance: %li\n", sizeof(game));

  return 0;
}
