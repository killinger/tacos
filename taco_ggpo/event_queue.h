#pragma once
#include "system_event_queue.h"

class event_queue
{
public:
	event_queue(system_event_queue* SystemEvents);
	~event_queue();
	void ProcessSystemQueue();
private:
	system_event_queue* m_SystemEvents;
};