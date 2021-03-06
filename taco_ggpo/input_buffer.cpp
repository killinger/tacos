#include "input_buffer.h"
#include <string>

void input_buffer::Initialize()
{
	memset(m_Buffer, 0, sizeof(m_Buffer));
	for (int32 i = 0; i < INPUT_BUFFER_SIZE; i++)
	{
		m_Buffer[(i + 1) % INPUT_BUFFER_SIZE].m_PrevEntry = i;
	}
	m_Cursor = 0;
}

void input_buffer::Update(uint32 InputMask, uint32 TimeStamp, bool FlipDirections)
{
	if (FlipDirections)
	{
		uint32 AdjustedMask = ((InputMask & INPUT_LEFT) << 1);
		AdjustedMask |= ((InputMask & INPUT_RIGHT) >> 1);
		InputMask = (InputMask & ~(INPUT_LEFT | INPUT_RIGHT)) | AdjustedMask;
	}

	if (m_Buffer[m_Cursor].m_InputMask != InputMask)
	{
		++m_Cursor %= INPUT_BUFFER_SIZE;
		m_Buffer[m_Cursor].m_InputMask = InputMask;
		m_Buffer[m_Cursor].m_TimeStamp = TimeStamp;
		m_Buffer[m_Cursor].m_ConsumedMask = ((InputMask & INPUT_BUTTONS) & 
											(~m_Buffer[m_Buffer[m_Cursor].m_PrevEntry].m_InputMask & INPUT_BUTTONS)) << BUTTON_COUNT;
		// TODO: Send event: Input buffer updated. Should be sent from the calling function so player index can be included
	}
}

int32 input_buffer::MatchInputs(move_description* MoveDescription, uint32 TimeStamp, int32 Buffer, bool Consume)
{
	Buffer -= (int32)(TimeStamp - m_Buffer[m_Cursor].m_TimeStamp);
	int32 CurrentIndex = MatchInput(m_Cursor, &MoveDescription->m_Input, Buffer);
	int32 MatchedEntry = CurrentIndex;
	if (CurrentIndex < 0)
		return -1;
	int32 ConsumeIndex = CurrentIndex;
	for (int32 i = ((int32)MoveDescription->m_MotionCount - 1); i >= 0; i--)
	{
		Buffer = (int32)MoveDescription->m_Motion[i].m_BufferFrames - (int32)(TimeStamp - m_Buffer[CurrentIndex].m_TimeStamp);
		CurrentIndex = MatchInput(CurrentIndex, &MoveDescription->m_Motion[i].m_Input, Buffer);

		if (CurrentIndex < 0)
			return -1;

		TimeStamp = m_Buffer[CurrentIndex].m_TimeStamp;
		CurrentIndex = m_Buffer[CurrentIndex].m_PrevEntry;
	}
	if (Consume)
		m_Buffer[ConsumeIndex].m_ConsumedMask |= (MoveDescription->m_Input.m_InputMask & INPUT_BUTTONS);
	return MatchedEntry;
}

bool input_buffer::MatchLastEntry(input_description* InputDescription)
{
	uint32 BufferRestrictionMask = 0;
	uint32 MoveRestrictionMask = 0xFFFFFFFF;

	if (InputDescription->m_PropertyFlags & DIRECTION_RESTRICTION_SIMILAR)
		BufferRestrictionMask |= (InputDescription->m_InputMask & INPUT_DIRECTIONS);
	else if (InputDescription->m_PropertyFlags & DIRECTION_RESTRICTION_EXACT)
		BufferRestrictionMask |= INPUT_DIRECTIONS;
	if (InputDescription->m_PropertyFlags & BUTTON_RESTRICTION_ALL)
		BufferRestrictionMask |= (InputDescription->m_InputMask & INPUT_BUTTONS);
	else if (InputDescription->m_PropertyFlags & BUTTON_RESTRICTION_ANY)
		MoveRestrictionMask = INPUT_DIRECTIONS;

	buffer_entry* CurrentEntry = &m_Buffer[m_Cursor];

	if ((CurrentEntry->m_InputMask & BufferRestrictionMask) == (InputDescription->m_InputMask & MoveRestrictionMask))
		if (!(InputDescription->m_PropertyFlags & BUTTON_RESTRICTION_ANY))
			return true;
		else if ((CurrentEntry->m_InputMask & InputDescription->m_InputMask) & INPUT_BUTTONS)
			return true;

	return false;
}

void input_buffer::ConsumeInput(uint32 EntryIndex, uint32 InputMask)
{
	m_Buffer[EntryIndex].m_ConsumedMask |= (InputMask & INPUT_BUTTONS);
}

int32 input_buffer::MatchInput(int32 CurrentIndex, input_description* InputDescription, int32 Buffer)
{
	uint32 BufferRestrictionMask = 0;
	uint32 MoveRestrictionMask = 0xFFFFFFFF;

	if (InputDescription->m_PropertyFlags & DIRECTION_RESTRICTION_SIMILAR)
		BufferRestrictionMask |= (InputDescription->m_InputMask & INPUT_DIRECTIONS);
	else if (InputDescription->m_PropertyFlags & DIRECTION_RESTRICTION_EXACT)
		BufferRestrictionMask |= INPUT_DIRECTIONS;
	if (InputDescription->m_PropertyFlags & BUTTON_RESTRICTION_ALL)
		BufferRestrictionMask |= (InputDescription->m_InputMask & INPUT_BUTTONS);
	else if (InputDescription->m_PropertyFlags & BUTTON_RESTRICTION_ANY)
		MoveRestrictionMask = INPUT_DIRECTIONS;

	while (Buffer >= 0)
	{
		if ((((m_Buffer[CurrentIndex].m_InputMask & BufferRestrictionMask) & INPUT_DIRECTIONS) |
			((m_Buffer[CurrentIndex].m_ConsumedMask >> BUTTON_COUNT) & BufferRestrictionMask)) ==
			(InputDescription->m_InputMask & MoveRestrictionMask))
		{
			if ((m_Buffer[CurrentIndex].m_ConsumedMask & INPUT_BUTTONS) != (InputDescription->m_InputMask & INPUT_BUTTONS))
			{
				if (!(InputDescription->m_PropertyFlags & BUTTON_RESTRICTION_ANY))
					return CurrentIndex;
				else if ((m_Buffer[CurrentIndex].m_InputMask & InputDescription->m_InputMask) & INPUT_BUTTONS)
					return CurrentIndex;
			}
		}

		if (m_Buffer[CurrentIndex].m_TimeStamp <= m_Buffer[m_Buffer[CurrentIndex].m_PrevEntry].m_TimeStamp)
			return -1;

		Buffer -= int32(m_Buffer[CurrentIndex].m_TimeStamp - m_Buffer[m_Buffer[CurrentIndex].m_PrevEntry].m_TimeStamp);
		CurrentIndex = m_Buffer[CurrentIndex].m_PrevEntry;
	}

	return -1;
}