#pragma once
#include "system_event_queue.h"

typedef void (*event_callback)();

struct event_queue_callbacks
{
	event_callback StepFrameCallback;
	event_callback ToggleFrameSteppingCallback;
};

class event_queue
{
public:
	event_queue(system_event_queue* SystemEvents, event_queue_callbacks* Callbacks);
	event_queue(system_event_queue* SystemEvents, 
				event_callback		StepFrameCallback,
				event_callback		ToggleFrameSteppingCallback);
	~event_queue();
	void ProcessSystemQueue();
private:
	system_event_queue*		m_SystemEvents;
	event_queue_callbacks	m_Callbacks;
};