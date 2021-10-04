#pragma once
#include "collisions.h"

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

struct status_element
{
	uint32	StatusFlags;
	uint8	FrameStart;
	uint8	FrameEnd;
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
	uint8			Effects;
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

typedef struct movement_element
{
	uint32			Target;
	float			Amount;
	uint8			FrameStart;
	uint8			FrameEnd;

	bool InRange(uint32 Cursor)
	{
		if (Cursor >= FrameStart && Cursor < FrameEnd)
			return true;
		return false;
	}
} force_element, position_element;

struct frame_elements
{
	status_element*		StatusElements;
	hitbox_element*		HitboxElements;
	hurtbox_element*	HurtboxElements;
	pushbox_element*	PushboxElements;
	frame_element*		CancelElements;
	force_element*		ForceElements;
	position_element*	PositionElements;
	frame_element*		AnimationElements;
	frame_element*		EngineElements;
	uint8				StatusCount;
	uint8				HitboxCount;
	uint8				HurtboxCount;
	uint8				PushboxCount;
	uint8				CancelCount;
	uint8				ForceCount;
	uint8				PositionCount;
	uint8				AnimationCount;
	uint8				EngineCount;
};