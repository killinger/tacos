#pragma once
#include "defs.h"
#include <SFML\Graphics.hpp>

#define VIEW_WIDTH 640.0f
#define VIEW_HEIGHT 360.0f

class render_system
{
public:
	render_system(sf::RenderWindow* Window);
	~render_system();

	void			DrawConsole(char* ConsoleInput, char* ConsoleHistory);
	void			DrawDebugString(const char* DebugString);
	void			Clear();
	void			Draw(sf::Sprite Sprite);
	void			Display();
	sf::Vector2f	GetViewCenter();

private:
	sf::RenderWindow*	m_Window;
	sf::View			m_MainView;
	sf::View			m_DebugView;
	sf::Font			m_ConsoleFont;
	sf::Text			m_ConsoleInputText;
	sf::Text			m_DebugString;
};