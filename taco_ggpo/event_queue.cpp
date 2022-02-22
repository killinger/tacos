#include "event_queue.h"
#include "subsystems.h"
#include "console_system.h"
#include "gamestate_buffer.h"
#include "debug_overlay.h"

event_queue::event_queue(system_event_queue* SystemEvents, event_queue_callbacks* Callbacks)
{
	m_SystemEvents = SystemEvents;
}

event_queue::event_queue(system_event_queue* SystemEvents, event_callback StepFrameCallback, event_callback ToggleFrameSteppingCallback)
{
	m_SystemEvents = SystemEvents; 
	m_Callbacks.StepFrameCallback = StepFrameCallback;
	m_Callbacks.ToggleFrameSteppingCallback = ToggleFrameSteppingCallback;
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
#if defined(_DEBUG) || defined(_PROFILE)
		else if (Event.Type == EVENT_KEY)
		{
			if (Event.Parameters[0] >= 48 && Event.Parameters[0] <= 59)
			{
				GameStateBuffer->SetCaptureSlot((uint8)(Event.Parameters[0] - 26));
			}
			else if (Event.Parameters[0] == 32)
			{
				m_Callbacks.ToggleFrameSteppingCallback();
			}
			else if (Event.Parameters[0] == 13)
			{
				m_Callbacks.StepFrameCallback();
			}
			else if (Event.Parameters[0] == 112)
			{
				GameStateBuffer->SaveGameState();
			}
			else if (Event.Parameters[0] == 113)
			{
				GameStateBuffer->LoadGameState();
			}
			else if (Event.Parameters[0] == 114)
			{
				GameStateBuffer->ToggleInputRecording();
			}
			else if (Event.Parameters[0] == 115)
			{
				GameStateBuffer->ToggleInputPlayback();
			}
		}
		else if (Event.Type == EVENT_MOUSEDOWN)
		{
			OverlayManager->MouseEvent(Event);
		}
		else if (Event.Type == EVENT_MOUSEUP)
		{
			OverlayManager->MouseEvent(Event);
		}
		else if (Event.Type == EVENT_MOUSEMOVE)
		{
			OverlayManager->MouseEvent(Event);
		}
		else if (Event.Type == EVENT_MOUSE2DOWN)
		{
			OverlayManager->MouseEvent(Event);
		}
#endif
	}
}
