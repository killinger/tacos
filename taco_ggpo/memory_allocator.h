#pragma once
#include "defs.h"
#include <assert.h>

class memory_allocator
{
public:
	memory_allocator(void* Start, uint32 Size) :
		m_Start(Start),
		m_Size(Size),
		m_UsedMemory(0),
		m_AllocationCount(0) {}
	virtual ~memory_allocator()
	{
		assert(m_UsedMemory == 0 && m_AllocationCount == 0);
		m_Start = nullptr;
		m_Size = 0;
	}
	virtual void*	Allocate(uint32 Size, uint8 Alignment = 4) = 0;
	virtual void	Free(void* Memory) = 0;

	void*			m_Start;
	uint32			m_Size;
	uint32			m_UsedMemory;
	uint32			m_AllocationCount;
};

#define ALLOCATE(Allocator, Type) Allocator->Allocate(sizeof(Type), alignof(Type))