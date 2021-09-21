#pragma once
#include "permanent_state.h"
#include "defs.h"

struct playbackstate
{
	int	PlaybackCursor;
	int Script;
	int PendingScript;
	int New;
};

struct playerstate
{
	playbackstate	PlaybackState;
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
	playerstate Player[2];
	int32		FrameCount;
	void Initialize()
	{
	}

	void Update(permanent_state* PermanentState, uint32* Inputs);
};