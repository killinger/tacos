#pragma once
#include <cstdint>

#define INPUT_DOWN 0x01
#define INPUT_LEFT 0x02
#define INPUT_RIGHT 0x04
#define INPUT_UP 0x08
#define INPUT_A 0x10
#define INPUT_B 0x20
#define INPUT_C 0x40
#define INPUT_D 0x80
#define INPUT_DIRECTIONS (INPUT_DOWN | INPUT_LEFT | INPUT_RIGHT | INPUT_UP)
#define INPUT_RELEASE_BIT 0x01
#define INPUT_CONSUME_BIT 0x02

#define SCRIPT_TYPE_CONTINUOUS 0
#define SCRIPT_TYPE_ATTACK 1

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;