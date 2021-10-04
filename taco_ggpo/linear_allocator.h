#pragma once
#include "memory_allocator.h"

class linear_allocator : public memory_allocator
{
public:
	linear_allocator(void* Start, uint32 Size);
	~linear_allocator();

	void*	Allocate(uint32 Size, uint8 Alignment) override;
	void	Free(void* Memory = nullptr) override;
private:
	void* m_CurrentPosition;
};