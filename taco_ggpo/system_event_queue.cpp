#include "system_event_queue.h"
#include <assert.h>

void system_event_queue::Enqueue(event_type Type, int32 P0, int32 P1)
{
	system_event* Event = &m_Events[m_Head & m_Mask];
	// full, assert for now
	assert(m_Head - m_Tail < m_Capacity);
	//if (Head - Tail >= Capacity)
	//{
	//	
	//	Tail++;
	//}
	m_Head++;
	Event->Type = Type;
	Event->Parameters[0] = P0;
	Event->Parameters[1] = P1;
	//Event->Timestamp = GetSysTimeSeconds();
}

system_event system_event_queue::Dequeue()
{
	system_event Result;
	if (m_Head > m_Tail)
	{
		Result = m_Events[m_Tail & m_Mask];
		m_Tail++;
	}
	return Result;
}

void system_event_queue::Clear()
{
	m_Head = 0;
	m_Tail = 0;
}
