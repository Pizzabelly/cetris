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
  RIGHT      = 2,
  LEFT       = 3,
  ROTATE_CCW = 4,
  ROTATE_CW  = 5,
  HARD_DROP  = 6
} input_t;
