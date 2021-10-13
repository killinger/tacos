#pragma once
#include "defs.h"
#include "system_event.h"

#define CVAR_NAME_LENGTH		32
#define CVAR_VALUE_LENGTH		8
#define CVAR_DESCRIPTION_LENGTH 64
#define CVAR_MAX_COUNT			32
#define CONSOLE_FIELD_LENGTH	128
#define CONSOLE_HISTORY_LENGTH	256

enum
{
	CVAR_EMPTY = 0,
	CVAR_INT = 1 << 0,
	CVAR_FLOAT = 1 << 1,
	CVAR_STRING = 1 << 2,
	CVAR_UINT = 1 << 3,
	CVAR_BOOL = 1 << 4
};

// TODO: Proper repeat protection, use messages from OS message pump

class console_system
{
public:
	console_system();
	~console_system();

	bool m_IsActive;

	/* RegisterCVar - registers a CVar to be used in the dev console.
		* Name is how the CVar will be referenced in the console,
		* Variable is a pointer to the actual variable,
		* Min/MaxValue are string representations of the min/max values ie a max value of 1.0 is set as "1.0",
		* Description is the descriptive text displayed in the console,
		* Type is the basic type - int, float, string or uint */
	void RegisterCVar(	const char* Name,
						void*		Variable,
						const char* MinValue,
						const char* MaxValue,
						const char* Description,
						uint8		Type);
	bool ProcessEvent(system_event Event);
	void DrawConsole();
	// ** TEST/DEBUG/DEV FUNCTIONS, REMOVE AFTER IMPLEMENTATION IS COMPLETE **
private:
	struct cvar
	{
		void*	Variable;
		uint8	Type;
		char	Name[CVAR_NAME_LENGTH];
		char	MinValue[CVAR_VALUE_LENGTH];
		char	MaxValue[CVAR_VALUE_LENGTH];
		char	Description[CVAR_DESCRIPTION_LENGTH];
	};
	cvar	m_CVars[CVAR_MAX_COUNT];
	uint32	m_HistorySize;
	uint32	m_KeyLastFrame;
	char	m_InputBuffer[CONSOLE_FIELD_LENGTH];
	char	m_ConsoleHistory[CONSOLE_HISTORY_LENGTH];
	uint8	m_CVarCount;
	uint8	m_Cursor;

	void ProcessChar(uint32 Key);
	void ProcessInput();
};