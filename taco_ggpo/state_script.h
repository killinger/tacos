#pragma once
#include <string>
#include "defs.h"
#include "script_elements.h"

#define SCRIPT_AIRBORNE 0x01
#define SCRIPT_RESTING 0x02

struct state_script
{
	std::string		Name;
	float			ScalingXV;
	float			ScalingYV;
	float			ScalingXA;
	float			ScalingYA;
	float			Friction;
	uint32			Flags;
	int8			FirstHitboxFrame;
	int8			LastHitboxFrame;
	uint8			TotalFrames;
	frame_elements	Elements;
};
