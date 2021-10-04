#pragma once
#include "defs.h"
#include "state_script.h"
#include "gamestate.h"
#include "move_description.h"

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

struct character_data
{
	float			WalkFSpeed;
	float			WalkBSpeed;
	float			JumpGravity;
	float			JumpVelocityY;
	float			JumpVelocityX;
};

struct hitbox_effects
{
	uint8 AtkLvl;
	uint8 Hitstop;
	float Knockback;
};

class state_manager
{
public:
	character_data		m_CharacterData;

	void				Initialize();
	state_script*		GetScript(uint32 Index);
	cancel_list*		GetCancelList(uint32 Index);
	move_description*	GetMoveDescription(uint32 Index);
	hitbox_effects*		GetHitboxEffects(uint32 Index);
private:
	state_script*		m_Scripts;
	cancel_list*		m_CancelLists;
	move_description*	m_Moves;
	hitbox_effects*		m_HitboxEffects;
	uint32				m_ScriptCount;
	uint32				m_CancelCount;
	uint32				m_MoveCount;
	uint32				m_HitboxEffectCount;

	void ReadFromDirectory(const char* Path);
};
