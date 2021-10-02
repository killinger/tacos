#pragma once
#include "defs.h"
#include "math.h"

struct collision_box
{
	float X;
	float Y;
	float Width;
	float Height;
};

bool BoxIntersection(collision_box* Box0, collision_box* Box1, collision_box* Result);