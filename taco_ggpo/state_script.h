#pragma once
#include "defs.h"
#include <string>

struct collision_box
{
	float X;
	float Y;
	float Width;
	float Height;
};

struct frame_element
{
	uint16	Index;
	uint8	FrameStart;
	uint8	FrameEnd;
	bool InRange(uint32 Cursor)
	{
		if (Cursor >= FrameStart && Cursor < FrameEnd)
			return true;
		return false;
	}
};

struct pushbox_element
{
	collision_box	Box;
	uint8			FrameStart;
	uint8			FrameEnd;

	bool InRange(uint32 Cursor)
	{
		if (Cursor >= FrameStart && Cursor < FrameEnd)
			return true;
		return false;
	}
};

struct hurtbox_element
{
	collision_box	Box;
	uint8			FrameStart;
	uint8			FrameEnd;

	bool InRange(uint32 Cursor)
	{
		if (Cursor >= FrameStart && Cursor < FrameEnd)
			return true;
		return false;
	}
};

struct hitbox_element
{
	collision_box	Box;
	uint16			EffectIndex;
	uint8			FrameStart;
	uint8			FrameEnd;

	bool InRange(uint32 Cursor)
	{
		if (Cursor >= FrameStart && Cursor < FrameEnd)
			return true;
		return false;
	}
};

struct frame_elements
{
	frame_element*		StatusElements;
	hitbox_element*		HitboxElements;
	hurtbox_element*	HurtboxElements;
	pushbox_element*	PushboxElements;
	frame_element*		CancelElements;
	frame_element*		ForceElements;
	frame_element*		PositionElements;
	frame_element*		EngineElements;
	uint8				StatusCount;
	uint8				HitboxCount;
	uint8				HurtboxCount;
	uint8				PushboxCount;
	uint8				CancelCount;
	uint8				ForceCount;
	uint8				PositionCount;
	uint8				EngineCount;
};

struct state_script
{
	std::string		Name;
	float			ScalingXV;
	float			ScalingYV;
	float			ScalingXA;
	float			ScalingYA;
	int8			FirstHitboxFrame;
	int8			LastHitboxFrame;
	uint8			TotalFrames;
	frame_elements	Elements;
};