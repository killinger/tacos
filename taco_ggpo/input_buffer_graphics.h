#pragma once
#include "defs.h"
#include "SFML\Graphics.hpp"
#include "input_buffer.h"

class input_buffer_graphics
{
public:
	void Initialize();
	void Update(input_buffer* InputBuffer);
	void Render();
private:
	sf::Texture m_ArrowTexture_1;
	sf::Texture m_ArrowTexture_2;
	sf::Texture m_ArrowTexture_3;
	sf::Texture m_ArrowTexture_4;
	sf::Texture m_ArrowTexture_6;
	sf::Texture m_ArrowTexture_7;
	sf::Texture m_ArrowTexture_8;
	sf::Texture m_ArrowTexture_9;
	sf::Texture m_ButtonTexture_A;
	sf::Texture m_ButtonTexture_B;
	sf::Texture m_ButtonTexture_C;

	sf::Sprite m_Sprite;
};

