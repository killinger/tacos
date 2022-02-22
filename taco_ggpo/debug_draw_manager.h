#pragma once
#include <DirectXMath.h>
#include "linear_allocator.h"
#include "defs.h"

enum
{
	COLOR_WHITE = 0,
	COLOR_COUNT
};

class debug_draw_manager
{
public:
	void Initialize(uint32 StringQueueSize);
	void AddString(const point& Point, const char* String, uint8 Color = 0, bool AlignRight = false);
	void AddRectangle(float X, float Y, float Width, float Height, uint8 Color = 0);
	void ProcessAndRenderQueues();
private:
	struct string_queue_entry
	{
		string_queue_entry() : m_NextEntry(NULL) { }
		string_queue_entry* m_NextEntry;
		point				m_Point;
		const char*			m_String;
		uint8				m_Color;
		bool				m_AlignRight;
		// TODO: Font.. something
	};
	struct string_queue
	{
		string_queue_entry* m_Head;
		string_queue_entry* m_Tail;
	};
	string_queue		m_StringQueue;
	linear_allocator*	m_StringQueueAllocator;
	DirectX::XMFLOAT4	m_Colors[COLOR_COUNT];
	void ClearStringQueue();
};