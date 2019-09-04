#pragma once

#include <stdint.h>

#define u8  uint8_t
#define u32 uint32_t
#define u64 uint64_t
#define s8  int8_t
#define s32 int32_t

typedef struct {
  s8 x;
  s8 y;
} vec2;

typedef u8 piece_matrix[4][4];
