#pragma once
#include "defs.h"

#define LOG_BUFFER_SIZE 5120
#define LOG LoggingSystem->Log
//#define LOG(...)

class logging_system
{
public:
	logging_system();

	void Log(const char* String...);
	void DrawLog();
	
	bool m_EmptyBufferToFile;
private:
	char	m_OutputBuffer[LOG_BUFFER_SIZE];
	uint32	m_WriteCursor;
};