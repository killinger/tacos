#include "logging_system.h"
#include <string.h>
#include <cstdarg>
#include <stdio.h>
#include "subsystems.h"
#include "render_system.h"

logging_system::logging_system()
{
	memset(m_OutputBuffer, 0, LOG_BUFFER_SIZE);
	m_WriteCursor = 0;
	m_EmptyBufferToFile = 1;
}

void logging_system::Log(const char* String ...)
{
	char Buffer[512];

	va_list args;
	va_start(args, String);
	vsnprintf(Buffer, 512, String, args);
	va_end(args);
	int32 BufferLength = strlen(Buffer);
	
	if (m_WriteCursor + BufferLength >= LOG_BUFFER_SIZE)
	{
		if (m_EmptyBufferToFile)
		{
			FILE* FilePtr;
			errno_t error = fopen_s(&FilePtr, "log.txt", "a");
			fputs(m_OutputBuffer, FilePtr);
			fclose(FilePtr);
		}
		memset(m_OutputBuffer, 0, LOG_BUFFER_SIZE);
		m_WriteCursor = 0;
	}
	memcpy_s(m_OutputBuffer + m_WriteCursor, LOG_BUFFER_SIZE - m_WriteCursor, Buffer, BufferLength);
	m_WriteCursor += BufferLength;
}

void logging_system::DrawLog()
{
	int32 StartCursor;
	int32 EndCursor = m_WriteCursor;
	int32 MaxLines = 18;
	int32 Stop = m_WriteCursor - 511;
	if (Stop < 0)
		Stop = 0;
	int32 LineCount = 0;

	for (StartCursor = m_WriteCursor; StartCursor > Stop; --StartCursor)
	{
		if (m_OutputBuffer[StartCursor - 1] == '\n')
			LineCount++;
		if (LineCount > MaxLines)
			break;
	}

	char Buffer[512];
	memset(Buffer, 0, 512);
	memcpy_s(Buffer, 511, m_OutputBuffer + StartCursor, m_WriteCursor - StartCursor);

}
