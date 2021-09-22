#include "input_buffer.h"
#include <string>
#include "logging_system.h"
#include "subsystems.h"

void input_buffer::Initialize()
{
	memset(m_Buffer, 0, sizeof(m_Buffer));
	for (uint32 i = 0; i < BUF_SIZE; i++)
	{
		m_Buffer[(i + 1) % BUF_SIZE].m_pPrevEntry = &m_Buffer[i];
	}
	m_Cursor = 0;
}

void input_buffer::Update(uint32 InputMask, uint32 TimeStamp, float Facing)
{
	if (Facing == -1.0f)
	{
		uint32 AdjustedMask = ((InputMask & INPUT_LEFT) << 1);
		AdjustedMask |= ((InputMask & INPUT_RIGHT) >> 1);
		InputMask = (InputMask & ~(INPUT_LEFT | INPUT_RIGHT)) | AdjustedMask;
	}

	if (m_Buffer[m_Cursor].m_InputMask != InputMask)
	{
		++m_Cursor %= BUF_SIZE;

		m_Buffer[m_Cursor].m_InputMask = InputMask;
		m_Buffer[m_Cursor].m_TimeStamp = TimeStamp;
	}
}

bool input_buffer::MatchInputs(move_description* MoveDescription, int32 Buffer, uint32 TimeStamp)
{
	Buffer -= (int32)(TimeStamp - m_Buffer[m_Cursor].m_TimeStamp);
	buffer_entry* CurrentEntry = MatchMotion(&m_Buffer[m_Cursor], &MoveDescription->m_Input, Buffer);

	if (CurrentEntry == NULL)
		return false;

	for (int32 i = (MoveDescription->m_MotionCount - 1); i >= 0; i--)
	{
		Buffer = (int32)MoveDescription->m_Motion[i].m_BufferFrames - (int32)(TimeStamp - CurrentEntry->m_TimeStamp);
		CurrentEntry = MatchMotion(CurrentEntry, &MoveDescription->m_Motion[i].m_Input, Buffer);

		if (CurrentEntry == NULL)
			return false;

		TimeStamp = CurrentEntry->m_TimeStamp;
		CurrentEntry = CurrentEntry->m_pPrevEntry;
	}

	return true;
}

input_buffer::buffer_entry* input_buffer::MatchMotion(buffer_entry* CurrentEntry, input_description* InputDescription, int32 Buffer)
{
	uint32 RestrictionMask = 0;

	if (InputDescription->m_PropertyFlags & DIRECTION_RESTRICTION_SIMILAR)
		RestrictionMask |= InputDescription->m_InputMask;
	else if (InputDescription->m_PropertyFlags & DIRECTION_RESTRICTION_EXACT)
		RestrictionMask |= 0xF;
	if (InputDescription->m_PropertyFlags & BUTTON_RESTRICTION_ALL)
		RestrictionMask |= INPUT_BUTTONS;

	while (Buffer >= 0)
	{
		if ((CurrentEntry->m_InputMask & RestrictionMask) == InputDescription->m_InputMask)
			return CurrentEntry;
		if (CurrentEntry->m_TimeStamp <= CurrentEntry->m_pPrevEntry->m_TimeStamp)
			return NULL;

		Buffer -= int32(CurrentEntry->m_TimeStamp - CurrentEntry->m_pPrevEntry->m_TimeStamp);
		CurrentEntry = CurrentEntry->m_pPrevEntry;
	}

	return NULL;
}