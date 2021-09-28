#include "event_queue.h"
#include "subsystems.h"
#include "console_system.h"

event_queue::event_queue(system_event_queue* SystemEvents)
{
	m_SystemEvents = SystemEvents;
}

void event_queue::ProcessSystemQueue()
{
	system_event Event;
	while (true)
	{
		Event = m_SystemEvents->Dequeue();
		if (Event.Type == NO_EVENT)
			break;
		else if (ConsoleSystem->ProcessEvent(Event))
			continue;
	}
}
