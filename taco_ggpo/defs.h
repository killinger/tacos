#pragma once
#include <cstdint>

#define Kilobytes(KB) KB << 10
#define	Megabytes(MB) MB << 20
#define	Gigabytes(GB) GB << 30

#define INPUT_DOWN 0x01
#define INPUT_LEFT 0x02
#define INPUT_BACK 0x02
#define INPUT_RIGHT 0x04
#define INPUT_FORWARD 0x04
#define INPUT_UP 0x08
#define INPUT_A 0x10
#define INPUT_B 0x20
#define INPUT_C 0x40
#define INPUT_D 0x80
#define INPUT_RUN 0x100
#define INPUT_DIRECTIONS 0xF
#define INPUT_BUTTONS 0x1F0
#define BUTTON_COUNT 5

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

struct point
{
	point();
	point(float x, float y, float z) :
		X(x), Y(y), Z(z) { }
	float X;
	float Y;
	float Z;
};