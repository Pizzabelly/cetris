#pragma once

#include <stdint.h>

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define i8  int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t

typedef struct {
  i8 x;
  i8 y;
} vec2;

typedef u8 piece_matrix[4][4];

typedef enum {
  DOWN       = 1,
  USER_DOWN  = 2,
  RIGHT      = 3,
  LEFT       = 4,
  ROTATE_CCW = 5,
  ROTATE_CW  = 6,
  HARD_DROP  = 7 
} input_t;

vec2 new_vec2(i8 x, i8 y);
