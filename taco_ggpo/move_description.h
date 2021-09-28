#pragma once
#include "defs.h"

#define MOTION_MAX_ENTRIES 5

#define DIRECTION_RESTRICTION_SIMILAR 0x01
#define DIRECTION_RESTRICTION_EXACT 0x02
#define BUTTON_RESTRICTION_ANY 0x10
#define BUTTON_RESTRICTION_ALL 0x20

struct input_description
{
	uint32 m_InputMask;
	uint32 m_PropertyFlags;
};

struct motion_entry
{
	input_description	m_Input;
	uint32				m_BufferFrames;
};

struct move_description
{
	input_description	m_Input;
	motion_entry		m_Motion[MOTION_MAX_ENTRIES];
	uint32				m_MotionCount;
	uint32				m_ScriptIndex;
};