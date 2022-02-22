#pragma once
#include "defs.h"

inline void* PointerAdd(void* P, uint32 I)
{
	return (void*)((uintptr_t)P + (uintptr_t)I);
}

inline void* PointerSubtract(void* P, uint32 I)
{
	return (void*)((uintptr_t)P - (uintptr_t)I);
}