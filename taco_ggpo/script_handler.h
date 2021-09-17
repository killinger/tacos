#pragma once
#include "defs.h"
#include "state_script.h"
#include "gamestate.h"

#define FORCE_VELOCITY_X 0x01
#define FORCE_VELOCITY_Y 0x02
#define FORCE_ACCELERATION_X 0x04
#define FORCE_ACCELERATION_Y 0x08

#define POSITION_X_AXIS 0x01
#define POSITION_Y_AXIS 0x02

struct cancel_list
{
	uint8*	Moves;
	uint8	MoveCount;
};

struct move_list
{
	uint32 InputMask;
	uint32 ScriptIndex;
};

class script_handler
{
public:
	void			Initialize();
	void			Update(playbackstate* PlaybackState);
	state_script*	GetScript(playbackstate* PlaybackState);
	cancel_list*	GetCancelList(uint16 Index);
	cancel_list*	GetReservedCancelList(uint32 Index);
	move_list*		GetMove(uint32 Index);
private:
	state_script*	m_Scripts;
	cancel_list*	m_CancelLists;
	move_list*		m_Moves;
	uint32			m_ScriptCount;
	uint32			m_CancelCount;
	uint32			m_MoveCount;


	void ReadFromDirectory(const char* Path);
};
