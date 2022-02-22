#include "debug_draw_manager.h"
#include <string>

using namespace DirectX;

void debug_draw_manager::Initialize(uint32 StringQueueSize)
{
	void* StringQueueMemoryStart = (void*)malloc(StringQueueSize);
	m_StringQueueAllocator = new linear_allocator(StringQueueMemoryStart, StringQueueSize);
	m_Colors[COLOR_WHITE] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

void debug_draw_manager::AddString(const point& Point, const char* String, uint8 Color, bool AlignRight)
{
	string_queue_entry* NewEntry = (string_queue_entry*)m_StringQueueAllocator->Allocate(sizeof(string_queue_entry), alignof(string_queue_entry));
	uint32 StringSize = strlen(String) + 1;
	
	if (m_StringQueue.m_Head == NULL)
		m_StringQueue.m_Head = NewEntry;
	else
		m_StringQueue.m_Tail->m_NextEntry = NewEntry;
	m_StringQueue.m_Tail = NewEntry;
	
	NewEntry->m_Point = Point;
	NewEntry->m_Color = Color;
	NewEntry->m_AlignRight = AlignRight;
	NewEntry->m_String = (const char*)m_StringQueueAllocator->Allocate(StringSize, alignof(const char*));
	memcpy_s((void*)NewEntry->m_String, StringSize, String, StringSize);
}

void debug_draw_manager::ProcessAndRenderQueues()
{
	ClearStringQueue();
}

void debug_draw_manager::ClearStringQueue()
{
	m_StringQueueAllocator->Free();
	m_StringQueue.m_Head = NULL;
	m_StringQueue.m_Tail = NULL;
}