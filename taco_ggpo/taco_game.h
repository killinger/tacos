#pragma once
#include <SFML\Graphics.hpp>
#include "event_queue.h"

namespace taco
{
	void Initialize(sf::RenderWindow* Window, system_event_queue* SystemEventQueue);
	void RunFrame();
}