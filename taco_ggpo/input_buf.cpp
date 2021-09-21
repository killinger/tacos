#include "input_buf.h"
#include <string>
#include "logging_system.h"
#include "subsystems.h"

void input_buf::Initialize()
{
	memset(m_Buffer, 0, sizeof(m_Buffer));
	for (uint32 i = 0; i < BUF_SIZE; i++)
	{
		m_Buffer[(i + 1) % BUF_SIZE].m_pPrevEntry = &m_Buffer[i];
	}
	m_Cursor = 0;
}

void input_buf::Update(uint32 InputMask, uint32 TimeStamp, float Facing)
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

bool input_buf::MatchInputs(move_description* MoveDescription, uint32 Buffer)
{
	buffer_entry* CurrentEntry = MatchInputs(&m_Buffer[m_Cursor], &MoveDescription->m_Input, (int32)Buffer);

	if (CurrentEntry == NULL)
		return false;
	
	for (int32 i = (MoveDescription->m_MotionCount - 1); i >= 0; i--)
	{
		CurrentEntry = MatchInputs(CurrentEntry, &MoveDescription->m_Motion[i].m_Input, (int32)MoveDescription->m_Motion[i].m_BufferFrames);
		if (CurrentEntry == NULL)
			return false;
		
		CurrentEntry = CurrentEntry->m_pPrevEntry;
	}

	return true;
}

input_buf::buffer_entry* input_buf::MatchInputs(buffer_entry* CurrentEntry, input_description* InputDescription, int32 Buffer)
{
	uint32 RestrictionMask = INPUT_BUTTONS;

	if (InputDescription->m_PropertyFlags & RESTRICT_SIMILAR)
		RestrictionMask = InputDescription->m_InputMask;
	else if (InputDescription->m_PropertyFlags & RESTRICT_EXACT)
		RestrictionMask = 0x1FF;
	do
	{
		if ((CurrentEntry->m_InputMask & RestrictionMask) == InputDescription->m_InputMask)
			return CurrentEntry;
		if (CurrentEntry->m_TimeStamp <= CurrentEntry->m_pPrevEntry->m_TimeStamp)
			return NULL;

		Buffer -= int32(CurrentEntry->m_TimeStamp - CurrentEntry->m_pPrevEntry->m_TimeStamp);
		CurrentEntry = CurrentEntry->m_pPrevEntry;
	} while (Buffer >= 0);
	
	return NULL;
}