#pragma once
#include "defs.h"
#include <string>
#include <vector>

// TODO: these are half assed

enum
{
	RESTRICTION_NONE = 0,
	RESTRICTION_SIMILAR,
	RESTRICTION_EXACT
};

enum
{
	MOTION_NORMAL = 0,
	MOTION_CHARGE
};


struct motion_entry
{
	uint32 Input;
	uint32 Type;
	uint32 Time;
	uint32 Restriction;
};

struct command
{
	std::vector<motion_entry>	Motion;
	uint32						Type;
	uint32						Direction;
	int32						Button;
	uint32						Restriction;
	std::string					Trigger;

	command()
	{
		Type = 0;
		Direction = 0;
		Button = -1,
		Restriction = 0;
	}
};