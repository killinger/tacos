#pragma once
#include "defs.h"

enum
{
	PLAYER_TYPE_LOCAL,
	PLAYER_TYPE_REMOTE,
	PLAYER_TYPE_DUMMY
};

struct player_info
{
	int32 Type;
};

struct permanent_state
{
	player_info Players[2];
};