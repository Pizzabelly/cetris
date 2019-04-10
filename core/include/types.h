#pragma once

#include <stdint.h>

typedef uint8_t piece_matrix[4][4];

typedef struct {
  int8_t x;
  int8_t y;
} vec2;

typedef enum {
  DOWN       = 1,
  RIGHT      = 2,
  LEFT       = 3,
  ROTATE_CCW = 4,
  ROTATE_CW  = 5,
  HARD_DROP  = 6
} input_t;
