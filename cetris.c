#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "cetris.h"

/* function  prototypes */
void init_game(struct cetris_game* g);
void update_game_tick(struct cetris_game* g);
void move_down(struct cetris_game* g);
void move_left(struct cetris_game* g);
void move_right(struct cetris_game* g);
void rotate_clockwise(struct cetris_game* g);
static void init_queue(struct cetris_game* g);
static void suffle_queue(struct cetris_game* g);
static void next_piece(struct cetris_game* g);
static void move_current(struct cetris_game* g, vec2 offset);
static void overlay_current(struct cetris_game* g);
static int check_new_matrix(struct cetris_game* g, piece_matrix m);
static void wipe_board(struct cetris_game* g);
static void set_constants(struct cetris_game* g);
static void rotate_matrix(struct cetris_game* g);

static const piece_matrix default_matrices[7] = {
  { 
    { 0, 0, 0, 0 },
    { 0, 1, 1, 0 }, 
    { 0, 1, 1, 0 },
    { 0, 0, 0, 0 }
  }, 

  { 
    { 0, 0, 0, 0 },
    { 1, 1, 1, 1 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 }
  },

  { 
    { 0, 0, 0, 0 },
    { 0, 1, 1, 0 },
    { 1, 1, 0, 0 },
    { 0, 0, 0, 0 }
  },

  { 
    { 0, 0, 0, 0 },
    { 1, 1, 0, 0 },
    { 0, 1, 1, 0 },
    { 0, 0, 0, 0 }
  },
  
  { 
    { 0, 0, 0, 0 },
    { 0, 0, 1, 0 },
    { 1, 1, 1, 0 },
    { 0, 0, 0, 0 }
  },

  { 
    { 0, 0, 0, 0 },
    { 1, 0, 0, 0 },
    { 1, 1, 1, 0 },
    { 0, 0, 0, 0 }
  },

  { 
    { 0, 0, 0, 0 },
    { 0, 1, 0, 0 },
    { 1, 1, 1, 0 },
    { 0, 0, 0, 0 }
  }
};

void init_game(struct cetris_game* g) {
  srand(time(NULL));
  memset(g->board, 0, sizeof(slot) * BOARD_X * BOARD_Y);
  g->tick = 0; g->current_index = 0;
  init_queue(g);
  suffle_queue(g);
  next_piece(g);
}

void update_game_tick(struct cetris_game* g) {
  overlay_current(g);
  int i = g->tick % CETRIS_TICKRATE;
  int s = CETRIS_TICKRATE / 2;
  if (i % s == 0) {
    move_down(g);
  }
  g->tick++;
}

void init_queue(struct cetris_game* g) {
  for (int i = 0; i < 7; i++) {
    switch (i) {
      case 0: g->piece_queue[i].t = O; break;
      case 1: g->piece_queue[i].t = I; break;
      case 2: g->piece_queue[i].t = S; break;
      case 3: g->piece_queue[i].t = Z; break;
      case 4: g->piece_queue[i].t = L; break;
      case 5: g->piece_queue[i].t = J; break;
      case 6: g->piece_queue[i].t = T; break;
    }
    memcpy(g->piece_queue[i].mat, default_matrices[i], sizeof(piece_matrix));
    g->piece_queue[i].pos = (vec2){5, 0};
  }
}

void suffle_queue(struct cetris_game* g) {
  for (int i = 0; i < 7; i++) {
    struct tetrimino t = g->piece_queue[i];
    int rand_index = rand() % 7;
    g->piece_queue[i] = g->piece_queue[rand_index];
    g->piece_queue[rand_index] = t;
  }
}

void next_piece(struct cetris_game* g) {
  if (g->current_index == 6) {
    g->current_index = 0;
    suffle_queue(g);
  }
  g->current = g->piece_queue[g->current_index];
  g->current_index++;
}

void move_current(struct cetris_game* g, vec2 offset) {
  g->current.pos.y += offset.y;
  g->current.pos.x += offset.x;
  int check = check_new_matrix(g, g->current.mat); 
  if (check <= 0) {
    g->current.pos.y -= offset.y;
    g->current.pos.x -= offset.x;
    if (check == -1) {
      set_constants(g);
      next_piece(g);
    }
  }
}

void move_down(struct cetris_game* g) {
  move_current(g, (vec2){0, 1});
}

void move_right(struct cetris_game* g) {
  move_current(g, (vec2){1, 0});
}

void move_left(struct cetris_game* g) {
  move_current(g, (vec2){-1, 0});
}

void rotate_matrix(struct cetris_game* g) {
  if (g->current.t == O) return;
  piece_matrix m;
  memset(m, 0, sizeof(piece_matrix));
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      if (g->current.mat[y][x]) {
        int new_x = 1 - (y - 2);
        int new_y = 2 + (x - 1);
        if (g->current.t == I) new_y--;
        m[new_y][new_x] = 1;
      }
    }
  }
  if (check_new_matrix(g, m) > 0) {
    memcpy(g->current.mat, m, sizeof(piece_matrix));
  }
}

void rotate_clockwise(struct cetris_game* g) {
  rotate_matrix(g);
}

int check_new_matrix(struct cetris_game* g, piece_matrix m) {
  vec2 relative;
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      relative = (vec2){g->current.pos.x + x, g->current.pos.y + y};
      if (m[y][x]) { 
        if (relative.x > BOARD_X - 1 || relative.x < 0) return 0;
        if (relative.y > BOARD_Y - 1 || relative.y < 0) return -1;
        if (g->board[relative.x][relative.y].occupied && 
            g->board[relative.x][relative.y].constant) return -1;
      }
    }
  }
  return 1;
}

void set_constants(struct cetris_game* g) {
  for (int x = 0; x < BOARD_X; x++) { 
    for (int y = 0; y < BOARD_Y; y++) {
      if (g->board[x][y].occupied) g->board[x][y].constant = 1;
    }
  }
}

void wipe_board(struct cetris_game* g) {
  for (int y = 0; y < BOARD_Y; y++) {
    int clear_line = 1;
    for (int x = 0; x < BOARD_X; x++) {
      if (!g->board[x][y].constant) g->board[x][y].occupied = 0;
      if (!g->board[x][y].occupied) clear_line = 0;
    }
    if (clear_line) {
      for (int x = 0; x < BOARD_X; x++) {
        g->board[x][y].occupied = 0;
        g->board[x][y].constant = 0;
      }
      for (int s = y - 1; s >= 0; s--) {
        for (int x = 0; x < BOARD_X; x++) {
          g->board[x][s + 1] = g->board[x][s];
        }
      }
    }
  }
}

void overlay_current(struct cetris_game* g) {
  wipe_board(g);
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) { 
      vec2 relative = (vec2){g->current.pos.x + x, g->current.pos.y + y};
      if (g->current.mat[y][x]) g->board[relative.x][relative.y].occupied = 1;
    }
  }
}
