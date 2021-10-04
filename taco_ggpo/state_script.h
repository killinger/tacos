#pragma once
#include <string>
#include "defs.h"
#include "script_elements.h"

#define SCRIPT_AIRBORNE 0x01
#define SCRIPT_RESTING 0x02
#define SCRIPT_ENDS_CROUCHING 0x04

#define STATUS_COUNTERHIT 0x01
#define STATUS_CROUCHING 0x02
#define STATUS_AIRBORNE 0x04

#define CANCEL_EXECUTE 0x01
#define CANCEL_BUFFER_HIT 0x02

struct state_script
{
	frame_elements	Elements;
	std::string		Name;
	float			ScalingXV;
	float			ScalingYV;
	float			ScalingXA;
	float			ScalingYA;
	uint32			Flags;
	uint8			TotalFrames;
	uint8			AtkLvl;
	int8			FirstHitboxFrame;
	int8			LastHitboxFrame;
};