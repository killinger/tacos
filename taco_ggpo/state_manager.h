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

struct gamestate;
struct playerstate;
struct playbackstate;

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

struct character_data
{
	float			WalkFSpeed;
	float			WalkBSpeed;
	float			JumpGravity;
	float			JumpVelocityY;
	float			JumpVelocityX;
};

class state_manager
{
public:
	void			Initialize();
	state_script*	GetScript(playbackstate* PlaybackState);
	state_script*	GetScript(uint32 Index);

	character_data	m_CharacterData;
private:
	state_script*	m_Scripts;
	cancel_list*	m_CancelLists;
	move_list*		m_Moves;
	uint32			m_ScriptCount;
	uint32			m_CancelCount;
	uint32			m_MoveCount;


	void ReadFromDirectory(const char* Path);
};
