#pragma once
#include "defs.h"
#include "system_event.h"

class system_event_queue
{
public:
	system_event_queue() :
		m_Capacity(256),
		m_Head(0),
		m_Tail(0),
		m_Mask(255) { }
	void Enqueue(event_type Type, int32 P0, int32 P1);
	system_event Dequeue();
	void Clear();
private:
	system_event	m_Events[256];
	int32			m_Mask;
	int32			m_Head;
	int32			m_Tail;
	int32			m_Capacity;
};
