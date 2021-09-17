#pragma once
#include "defs.h"
#include "input_buffer.h"

enum player_type
{
	PLAYER_TYPE_LOCAL,
	PLAYER_TYPE_REMOTE,
	PLAYER_TYPE_DUMMY
};

struct player_info
{
	player_type		Type;
	input_buffer	InputBuffer;
};

struct permanent_state
{
	player_info Player[2];
};