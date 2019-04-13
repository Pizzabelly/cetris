struct cetris_game;

enum tests {
  TSPIN,
  TSPIN_NO_LINES
};

void apply_test_board(struct cetris_game* g, enum tests t);
