#pragma once
#include "defs.h"
#include "state_script.h"
#include "gamestate.h"

#define FORCE_VELOCITY_X 0x01
#define FORCE_VELOCITY_Y 0x02
#define FORCE_ACCELERATION_X 0x04
#define FORCE_ACCELERATION_Y 0x08

typedef struct flag_amount
{
	uint32	Flags;
	float	Amount;
} position_shift;

struct force_command
{
	uint32	Target;
	float	Amount;

	void Apply(playerstate* PlayerState)
	{
		switch (Target)
		{
		case FORCE_VELOCITY_X: PlayerState->VelocityX = Amount; break;
		case FORCE_VELOCITY_Y: PlayerState->VelocityY = Amount; break;
		case FORCE_ACCELERATION_X: PlayerState->AccelerationX = Amount; break;
		case FORCE_ACCELERATION_Y: PlayerState->AccelerationY = Amount; break;
		default:
			break;
		}
	}
};

struct cancel_list
{
	uint8*	Moves;
	uint8	MoveCount;
};

class script_handler
{
public:
	void			Initialize();
	void			Update(playbackstate* PlaybackState);
	state_script*	GetScript(playbackstate* PlaybackState);
	cancel_list*	GetCancelList(playbackstate* PlaybackState);
	cancel_list*	GetReservedCancelList(uint32 Index);
private:
	state_script*	m_Scripts;
	cancel_list*	m_CancelLists;
	uint32			m_ScriptCount;
	uint32			m_CancelCount;


	void ReadFromDirectory(const char* Path);
};