#pragma once
#include "input_buffer.h"
#include "defs.h"

struct playbackstate
{
	int	PlaybackCursor;
	int Script;
	int PendingScript;
};

struct playerstate
{
	playbackstate	PlaybackState;
	input_buffer	InputBuffer;
	float			PositionX;
	float			PositionY;
	float			VelocityX;
	float			VelocityY;
	float			AccelerationX;
	float			AccelerationY;
	float			Facing;
};

// TODO: Move playerstate to a proper class? possibly the entirety of gamestate?
struct gamestate
{
	playerstate PlayerState[2];
	int32		FrameCount;
	void Initialize()
	{
		PlayerState[0].InputBuffer.Initialize();
		PlayerState[1].InputBuffer.Initialize();
	}
};