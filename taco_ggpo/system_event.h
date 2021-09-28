#pragma once
#include "defs.h"

typedef enum
{
	NO_EVENT,
	EVENT_KEY,
	EVENT_CHAR
} event_type;

struct system_event
{
	system_event() :
		Type(NO_EVENT) {}
	event_type	Type;
	int32		Parameters[2];
	float		Timestamp;
};