#pragma once
#include "defs.h"
#include "gamestate.h"
#include <SFML\Graphics.hpp>

#define BOUND_LEFT_MAX -640.0f
#define BOUND_RIGHT_MAX 640.0f

class camera
{
public:
	camera();
	
	sf::View	m_View;
	float		m_LeftBound;
	float		m_RightBound;

	void Update(gamestate* Gamestate);
private:

};

