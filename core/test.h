#include "cetris.h"

typedef enum {
  TSPIN,
  TSPIN_NO_LINES
} test;

void apply_test_board( cetris_game* g, test t);
