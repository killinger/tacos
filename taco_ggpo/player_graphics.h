#pragma once
#include "defs.h"
#include "SFML\Graphics.hpp"

class player_graphics
{
public:
	player_graphics();
	~player_graphics();

	sf::Sprite		m_CharacterSprite;

	uint32	Initialize(const char* Filename);
	void	SetAnimation(uint32 Index);
	// ** TEST/DEBUG/DEV FUNCTIONS, REMOVE AFTER IMPLEMENTATION IS COMPLETE **
	void	NextFrame();
private:
	sf::Texture		m_CharacterTexture;
	sf::IntRect*	m_AnimationFrames;
	uint32			m_FrameCount;
};