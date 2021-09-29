#pragma once
#include "defs.h"
#include "move_description.h"

#define INPUT_BUFFER_SIZE 40

class input_buffer
{
	friend class gamestate_buffer;
public:
	input_buffer();

	void Update(uint32 InputMask, uint32 TimeStamp, bool FlipDirections);
	bool MatchInputs(move_description* MoveDescription, uint32 TimeStamp, int32 Buffer);
	bool MatchLastEntry(input_description* InputDescription);
private:
	struct buffer_entry
	{
		uint32			m_InputMask;
		uint32			m_ConsumedMask;
		uint32			m_TimeStamp;
		int32			m_PrevEntry;
	};
	buffer_entry	m_Buffer[INPUT_BUFFER_SIZE];
	uint32			m_Cursor;

	int32 MatchInput(int32 CurrentIndex, input_description* InputDescription, int32 Buffer);
};