#include <stdio.h>

#include "../lib/cetris.h"

int main(void) {
  ctrs_game game;

  ctrs_init_game(&game);

  printf("memory usage of game instance: %li\n", sizeof(game));

  return 0;
}
