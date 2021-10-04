#pragma once
#include "defs.h"
#include "input_buffer.h"
#include "state_manager.h"
#include "memory_allocator.h"

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
	bool				DisableHitbox;
	uint8				BufferedJump;
	uint32				Hitstop;
};

// TODO: Move playerstate to a proper class? possibly the entirety of gamestate?
struct gamestate
{
	playerstate m_Player[2];
	int32		m_FrameCount;
	
	void			Initialize(memory_allocator* MemoryAllocator);
	void			Update(uint32* Inputs, state_manager* StateManager);
private:
	state_script*	AdvancePlayerState(state_manager* StateManager, playerstate* PlayerState, playerstate* OtherPlayer);
	void			UpdateMovement(playerstate* PlayerState);
	void			CorrectAndFinalizePositions(state_script* Script[2]);
	void			HitDetection(state_manager* StateManager, uint32 PlayerIndex, uint32 OtherIndex, state_script* PlayerScript, state_script* OtherScript);
	void			ResolveHitAndApplyEffects(state_manager* StateManager, uint32 PlayerIndex, uint32 OtherIndex, uint8 EffectIndex, state_script* OtherScript);
};