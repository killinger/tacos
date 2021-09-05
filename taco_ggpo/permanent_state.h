#pragma once
#include "defs.h"

enum player_type
{
	PLAYER_TYPE_LOCAL,
	PLAYER_TYPE_REMOTE,
	PLAYER_TYPE_DUMMY
};

struct player_info
{
	player_type Type;
};

struct permanent_state
{
	player_info Players[2];
};