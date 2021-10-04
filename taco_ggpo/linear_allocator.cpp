#include "linear_allocator.h"

linear_allocator::linear_allocator(void* Start, uint32 Size) :
	memory_allocator(Start, Size),
	m_CurrentPosition(Start)
{
	assert(Size > 0);
}

linear_allocator::~linear_allocator()
{
}

void* linear_allocator::Allocate(uint32 Size, uint8 Alignment)
{
	uint8 Adjustment = Alignment - ((uintptr_t)m_CurrentPosition & (uintptr_t)Alignment - 1);
	if (Adjustment == Alignment)
		Adjustment = 0;

	if (Size + Adjustment + m_UsedMemory > m_Size)
		return nullptr;
	
	void* AlignedAddress = (void*)((uintptr_t)m_CurrentPosition + (uintptr_t)Adjustment);
	m_CurrentPosition = (void*)((uintptr_t)m_CurrentPosition + (uintptr_t)Size);
	m_UsedMemory += Size + Adjustment;
	m_AllocationCount++;

	return AlignedAddress;
}

void linear_allocator::Free(void* Memory)
{
	m_AllocationCount = 0;
	m_UsedMemory = 0;
	m_CurrentPosition = m_Start;
}