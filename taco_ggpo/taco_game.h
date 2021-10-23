#pragma once
#include "event_queue.h"
#include <Windows.h>

namespace taco
{
	void Initialize(HINSTANCE Instance, HWND Window, system_event_queue* SystemEventQueue);
	void RunFrame();
}