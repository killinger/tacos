#include "event_queue.h"
#include "subsystems.h"
#include "console_system.h"
#include "gamestate_buffer.h"

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
		else if (Event.Type == EVENT_KEY)
		{
			if (Event.Parameters[0] >= 27 && Event.Parameters[0] <= 29)
			{
				GameStateBuffer->SetSaveSlot((uint8)(Event.Parameters[0] - 27));
			}
			else if (Event.Parameters[0] == 85)
			{
				GameStateBuffer->SaveGameState();
			}
			else if (Event.Parameters[0] == 86)
			{
				GameStateBuffer->LoadGameState();
			}
		}
	}
}
