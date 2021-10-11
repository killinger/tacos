#pragma once
#include "defs.h"
#include "input_buffer.h"
#include "state_manager.h"

#define PLAYER_ALLOW_CANCEL 0x01
#define PLAYER_USED_AIR_ACTION 0x02
#define BUFFERED_STATE_FROM_OPPONENT 0x01
#define BUFFERED_STATE_HIT_CANCEL 0x02
#define BUFFERED_STATE_WHIFF_CANCEL 0x04
#define BUFFERED_STATE_ANY_CANCEL 0x06

class state_manager;

struct playbackstate
{
	uint32	PlaybackCursor;
	uint32	State;
	bool	New;
};

struct buffered_state
{
	uint32 StateIndex;
	uint32 Flags;
	uint32 InputMask;
	uint32 InputBufferIndex;
};

struct playerstate
{
	playbackstate		PlaybackState;
	buffered_state		BufferedState;
	input_buffer		InputBuffer;
	uint32				Flags;
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
	uint8				Hitstop;
};

// TODO: Move playerstate to a proper class? possibly the entirety of gamestate?
struct gamestate
{
	playerstate m_Player[2];
	int32		m_FrameCount;
	
	void			Initialize();
	void			Update(uint32* Inputs, state_manager* StateManager);
private:
	state_script*	AdvancePlayerState(state_manager* StateManager, playerstate* PlayerState, playerstate* OtherPlayer);
	void			UpdateMovement(playerstate* PlayerState);
	void			CorrectAndFinalizePositions(state_script* Script[2]);
	void			HitDetection(state_manager* StateManager, uint32 PlayerIndex, uint32 OtherIndex, state_script* PlayerScript, state_script* OtherScript);
	void			ResolveHitAndApplyEffects(state_manager* StateManager, uint32 PlayerIndex, uint32 OtherIndex, uint8 EffectIndex, state_script* OtherScript);
};