#define BOARD_X 10
#define BOARD_Y 20

#define CETRIS_HZ 60
#define CETRIS_DAS_DELAY 11
#define CETRIS_DAS_PERIOD 5
#define CETRIS_LINE_CLEAR_DELAY 40

typedef struct {
  int x;
  int y;
} vec2;

typedef enum {
  O, I, S, Z, L, J, T
} type;

typedef int piece_matrix[4][4];

struct tetrimino {
  type t;
  piece_matrix mat;
  vec2 pos;
};

typedef struct {
  int occupied;
  int constant;
  int remove_tick;
} slot;

enum movements {
  DOWN = 1,
  LEFT = 2,
  RIGHT = 3
};

struct cetris_game {
  slot board[BOARD_X][BOARD_Y];

  struct tetrimino piece_queue[7];

  struct tetrimino current;
  int current_index;

  enum movements move_queue[20];
  int move_queue_count;
  int move_queue_pos;

  int tick;

  int lines;
  int level;

  /* long int just incase */
  long int score;
};

void init_game(struct cetris_game* g);
void update_game_tick(struct cetris_game* g);
void move_down(struct cetris_game* g);
void move_left(struct cetris_game* g);
void move_right(struct cetris_game* g);
void move_hard_drop(struct cetris_game* g);
void rotate_clockwise(struct cetris_game* g);
