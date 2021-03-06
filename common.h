#pragma once
#include <stdint.h>
#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint32_t

#define s8  int8_t
#define s16 int16_t
#define s32 int32_t
#define s64 int32_t

#define UNUSED __attribute__((unused))

#define MIN_MAX(type) \
static inline \
type min_##type (type const x, type const y) \
{ return y < x ? y : x;} \
static inline \
type max_##type (type const x, type const y) \
{ return y > x ? y : x;}

MIN_MAX(int)
MIN_MAX(unsigned)
MIN_MAX(float)
MIN_MAX(double)
MIN_MAX(u8)

#undef MIN_MAX

