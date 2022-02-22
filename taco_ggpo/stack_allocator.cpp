#include "stack_allocator.h"
#include "pointer.h"

stack_allocator::stack_allocator(void* Start, uint32 Size) :
	memory_allocator(Start, Size),
	m_CurrentPosition(Start)
{
	assert(Size > 0);
}

stack_allocator::~stack_allocator()
{
	assert(m_UsedMemory == 0 && m_AllocationCount == 0);
	m_CurrentPosition = 0;
}

void* stack_allocator::Allocate(uint32 Size, uint8 Alignment)
{
	assert(Size > 1 && sizeof(allocation_header) == 1);
	uint8 Adjustment = Alignment - ((uintptr_t)PointerAdd(m_CurrentPosition, 1) & ((uintptr_t)Alignment - 1));
	if (Adjustment == Alignment)
		Adjustment = 0;
	Adjustment++;

	if (m_UsedMemory + Adjustment + Size > m_Size)
		return nullptr;

	void* AlignedAddress = PointerAdd(m_CurrentPosition, Adjustment);
	allocation_header* Header = (allocation_header*)PointerSubtract(AlignedAddress, 1);
	Header->Adjustment = Adjustment;

	m_CurrentPosition = PointerAdd(AlignedAddress, Size);
	m_UsedMemory += Size + Adjustment;
	m_AllocationCount++;

	return AlignedAddress;
}

void stack_allocator::Free(void* Memory)
{
	allocation_header* Header = (allocation_header*)((uintptr_t)Memory - 1);
	m_UsedMemory -= (uint32)(((uintptr_t)m_CurrentPosition - (uintptr_t)Memory + Header->Adjustment));
	m_CurrentPosition = (void*)((uintptr_t)Memory - (uintptr_t)Header->Adjustment);
	m_AllocationCount--;
}
