#include "console_system.h"
#include "render_system.h"
#include "subsystems.h"
#include <string>
#include <assert.h>

#define CONSOLE_KEYCODE 167

console_system::console_system()
{
	m_IsActive = 0;
	m_CVarCount = 0;
	m_Cursor = 0;
	m_HistorySize = 0;
	memset(m_InputBuffer, 0, CONSOLE_FIELD_LENGTH);
	memset(m_ConsoleHistory, 0, CONSOLE_HISTORY_LENGTH);
}

console_system::~console_system()
{
}

void console_system::RegisterCVar(const char* Name,
	void*		Variable,
	const char* MinValue,
	const char* MaxValue,
	const char* Description,
	uint8		Type)
{
	uint32 NameLength = (uint32)strlen(Name);
	uint32 MinValueLength = (uint32)strlen(MinValue);
	uint32 MaxValueLength = (uint32)strlen(MaxValue);
	uint32 DescriptionLength = (uint32)strlen(Description);

	assert(NameLength <= CVAR_NAME_LENGTH && NameLength > 0);
	assert(MinValueLength <= CVAR_VALUE_LENGTH && MinValueLength > 0);
	assert(MaxValueLength <= CVAR_VALUE_LENGTH && MaxValueLength > 0);
	assert(DescriptionLength <= CVAR_DESCRIPTION_LENGTH && DescriptionLength > 0);
	assert(m_CVarCount + 1 < CVAR_MAX_COUNT);

	memset((void*)m_CVars[m_CVarCount].Name, 0, CVAR_NAME_LENGTH);
	memcpy_s((void*)m_CVars[m_CVarCount].Name, CVAR_NAME_LENGTH, Name, NameLength);
	memset((void*)m_CVars[m_CVarCount].MinValue, 0, CVAR_VALUE_LENGTH);
	memcpy_s((void*)m_CVars[m_CVarCount].MinValue, CVAR_VALUE_LENGTH, MinValue, MinValueLength);
	memset((void*)m_CVars[m_CVarCount].MaxValue, 0, CVAR_VALUE_LENGTH);
	memcpy_s((void*)m_CVars[m_CVarCount].MaxValue, CVAR_VALUE_LENGTH, MaxValue, MaxValueLength);
	memset((void*)m_CVars[m_CVarCount].Description, 0, CVAR_DESCRIPTION_LENGTH);
	memcpy_s((void*)m_CVars[m_CVarCount].Description, CVAR_DESCRIPTION_LENGTH, Description, DescriptionLength);
	m_CVars[m_CVarCount].Variable = Variable;
	m_CVars[m_CVarCount].Type = Type;

	m_CVarCount++;
}

bool console_system::ProcessEvent(system_event Event)
{
	if (Event.Type == EVENT_CHAR)
	{
		if (Event.Parameters[0] == CONSOLE_KEYCODE)
		{
			m_IsActive = 1 - m_IsActive;
		}
		else if (m_IsActive)
		{
			ProcessChar(Event.Parameters[0]);
		}
		return true;
	}
	return false;
}

void console_system::DrawConsole()
{
	RenderSystem->DrawConsole(m_InputBuffer, m_ConsoleHistory);
}

void console_system::ProcessChar(uint32 Key)
{
	if (Key == 8)
	{
		if (m_Cursor > 0)
		{
			m_InputBuffer[m_Cursor - 1] = '\0';
			m_Cursor--;
		}
	}
	else if (Key == 13)
	{
		ProcessInput();
	}
	else
	{
		m_InputBuffer[m_Cursor] = Key;
		m_Cursor++;
	}
}

void console_system::ProcessInput()
{
	char Name[32];
	char Value[8];
	memset(Name, 0, 32);
	memset(Value, 0, 8);
	bool DescriptionOnly = false;
	uint32 Index;
	for (Index = 0; Index <= m_Cursor; Index++)
	{
		if (m_InputBuffer[Index] == ' ')
			break;
		else if (m_InputBuffer[Index] == '\0')
		{
			DescriptionOnly = true;
			break;
		}
		else
			Name[Index] = m_InputBuffer[Index];
	}
	Index++;
	for (uint32 i = 0; Index < m_Cursor; i++, Index++)
	{
		if (m_InputBuffer[Index] == ' ')
			break;
		Value[i] = m_InputBuffer[Index];
	}
	for (uint8 i = 0; i < m_CVarCount; i++)
	{
		bool Found = true;
		for (uint8 j = 0; j < 32; j++)
		{
			if (m_CVars[i].Name[j] != Name[j])
			{
				Found = false;
				break;
			}
		}
		if (Found)
		{
			if (!DescriptionOnly)
			{
				if (m_CVars[i].Type & CVAR_FLOAT)
				{
					float FloatValue = (float)atof(Value);
					float MinValue = (float)atof(m_CVars[i].MinValue);
					float MaxValue = (float)atof(m_CVars[i].MaxValue);
					if (FloatValue > MaxValue)
						FloatValue = MaxValue;
					else if (FloatValue < MinValue)
						FloatValue = MinValue;
					*(float*)m_CVars[i].Variable = FloatValue;
				}
				else if (m_CVars[i].Type & CVAR_INT)
				{
					int32 IntValue = (int32)atoi(Value);
					int32 MinValue = (int32)atoi(m_CVars[i].MinValue);
					int32 MaxValue = (int32)atoi(m_CVars[i].MaxValue);
					if (IntValue > MaxValue)
						IntValue = MaxValue;
					else if (IntValue < MinValue)
						IntValue = MinValue;
					*(int32*)m_CVars[i].Variable = IntValue;
				}
			}

			m_HistorySize = 0;
			for (uint8 j = 0; m_HistorySize < CONSOLE_HISTORY_LENGTH; m_HistorySize++, j++)
			{
				if (m_CVars[i].Name[j] == '\0')
				{
					break;
				}
				m_ConsoleHistory[m_HistorySize] = m_CVars[i].Name[j];
			}

			char ValueBuffer[32];
			if (m_CVars[i].Type & CVAR_FLOAT)
				sprintf_s(ValueBuffer, 32, " is %f", *(float*)(m_CVars[i].Variable));
			else if (m_CVars[i].Type & CVAR_INT)
				sprintf_s(ValueBuffer, 32, " is %i", *(int32*)(m_CVars[i].Variable));


			for (uint8 j = 0; m_HistorySize < CONSOLE_HISTORY_LENGTH; m_HistorySize++, j++)
			{
				if (ValueBuffer[j] == '\0')
				{
					m_ConsoleHistory[m_HistorySize] = '\n';
					m_HistorySize++;
					break;
				}
				m_ConsoleHistory[m_HistorySize] = ValueBuffer[j];
			}
			for (uint8 j = 0; m_HistorySize < CONSOLE_HISTORY_LENGTH; m_HistorySize++, j++)
			{
				if (m_CVars[i].Description[j] == '\0')
				{
					m_ConsoleHistory[m_HistorySize] = '\n';
					m_HistorySize++;
					break;
				}
				m_ConsoleHistory[m_HistorySize] = m_CVars[i].Description[j];
			}
			return;
		}
	}

	memset(m_ConsoleHistory, 0, CONSOLE_HISTORY_LENGTH);
	char ValueBuffer[64];
	sprintf_s(ValueBuffer, 64, "No variable \"%s\" registered!", Name);
	m_HistorySize = (uint32)strlen(ValueBuffer);
	memcpy_s(m_ConsoleHistory, CONSOLE_HISTORY_LENGTH, ValueBuffer, m_HistorySize);
}

