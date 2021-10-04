#pragma once
#include "defs.h"
#include <SFML\Graphics.hpp>
#include <cstdarg>

#define VIEW_WIDTH 640.0f
#define VIEW_HEIGHT 360.0f

class render_system
{
public:
	render_system(sf::RenderWindow* Window);
	~render_system();

	void			DrawConsole(char* ConsoleInput, char* ConsoleHistory);
	void			SetDebugString(const char* DebugString...);
	void			DrawDebugString(const char* DebugString);
	void			DrawDebugString();
	void			DrawWorldText(float X, float Y, const char* WorldText...);
	void			Clear();
	void			Draw(sf::Sprite Sprite);
	void			Draw(sf::RectangleShape Rectangle);
	void			DrawLine(float P0X, float P0Y, float P1X, float P1Y);
	void			Display();
	sf::Vector2f	GetViewCenter();

private:
	sf::RenderWindow*	m_Window;
	sf::RenderTexture	m_MainFramebuffer;
	sf::Shader			m_Shader;
	sf::View			m_MainView;
	sf::View			m_DebugView;
	sf::Font			m_ConsoleFont;
	sf::Text			m_ConsoleInputText;
	sf::Text			m_DebugString;
	sf::Text			m_WorldText;
};