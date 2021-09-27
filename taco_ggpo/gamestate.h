#pragma once
#include "defs.h"
#include "input_buffer.h"
#include "state_manager.h"

class state_manager;

struct playbackstate
{
	uint32	PlaybackCursor;
	uint32	State;
	bool	New;
};

struct playerstate
{
	playbackstate		PlaybackState;
	input_buffer		InputBuffer;
	float				PositionX;
	float				PositionY;
	float				VelocityX;
	float				VelocityY;
	float				AccelerationX;
	float				AccelerationY;
	float				RunVelocity;
	float				RunAcceleration;
	float				Facing;
	uint32				BufferedJump;
};

// TODO: Move playerstate to a proper class? possibly the entirety of gamestate?
struct gamestate
{
	playerstate m_Player[2];
	int32		m_FrameCount;
	
	void Update(uint32* Inputs, state_manager* StateManager);
private:
	void AdvancePlayerState(state_manager* StateManager, playerstate* PlayerState, playerstate* OtherPlayer);
	void UpdateMovement(playerstate* PlayerState);
	void PerformPositionCorrection(state_manager* StateManager);
};