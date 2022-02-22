#pragma once
#include "memory_allocator.h"

class stack_allocator : public memory_allocator
{
public:
	stack_allocator(void* Start, uint32 Size);
	~stack_allocator();

	void* Allocate(uint32 Size, uint8 Alignment) override;
	void	Free(void* Memory) override;

private:
	void* m_CurrentPosition;

	struct allocation_header
	{
		uint8_t Adjustment;
	};
};