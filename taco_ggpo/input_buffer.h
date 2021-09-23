#pragma once
#include "defs.h"
#define INPUT_BUFFER_SIZE 40
#define DIRECTION_RESTRICTION_SIMILAR 0x01
#define DIRECTION_RESTRICTION_EXACT 0x02
#define BUTTON_RESTRICTION_ANY 0x10
#define BUTTON_RESTRICTION_ALL 0x20

struct input_description
{
	uint32 m_InputMask;
	uint32 m_PropertyFlags;
};

struct mo_entry
{
	input_description	m_Input;
	uint32				m_BufferFrames;
};

struct move_description
{
	input_description	m_Input;
	mo_entry			m_Motion[3];
	uint32				m_MotionCount;
};

class input_buffer
{
public:
	void Initialize();
	void Update(uint32 InputMask, uint32 TimeStamp, bool FlipDirections);
	bool MatchInputs(move_description* MoveDescription, uint32 TimeStamp, int32 Buffer);
private:
	struct buffer_entry
	{
		uint32			m_InputMask;
		uint32			m_ConsumedMask;
		uint32			m_TimeStamp;
		buffer_entry*	m_pPrevEntry;
	};
	buffer_entry	m_Buffer[INPUT_BUFFER_SIZE];
	uint32			m_Cursor;

	buffer_entry* MatchInput(buffer_entry* CurrentEntry, input_description* InputDescription, int32 Buffer);
};