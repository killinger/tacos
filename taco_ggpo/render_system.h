#pragma once
#include "defs.h"
#include <SFML\Graphics.hpp>

class render_system
{
public:
	render_system(sf::RenderWindow* Window);
	~render_system();

	void DrawConsole(char* ConsoleInput, char* ConsoleHistory);
	void DrawDebugString(const char* DebugString);
	void Clear();
	void Draw(sf::Sprite Sprite);
	void Display();

private:
	sf::RenderWindow*	m_Window;
	sf::View			m_MainView;
	sf::Font			m_ConsoleFont;
	sf::Text			m_ConsoleInputText;
	sf::Text			m_DebugString;
};