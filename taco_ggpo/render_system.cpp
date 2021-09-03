#include "render_system.h"

//TODO: error checking
render_system::render_system(sf::RenderWindow* Window)
{
	m_Window = Window;

	if (!m_ConsoleFont.loadFromFile("consola.ttf"))
	{
		//
	}

	m_ConsoleInputText.setFont(m_ConsoleFont);
	m_ConsoleInputText.setCharacterSize(20);
	m_ConsoleInputText.setFillColor(sf::Color::White);
	m_ConsoleInputText.setPosition(sf::Vector2f(0.0f, 690.0f));
	m_DebugString.setFont(m_ConsoleFont);
	m_DebugString.setCharacterSize(24);
	m_DebugString.setFillColor(sf::Color::White);
}

render_system::~render_system()
{
}

void render_system::DrawConsole(char* ConsoleInput, char* ConsoleHistory)
{
	m_ConsoleInputText.setString(">" + std::string(ConsoleInput));
	m_DebugString.setString(ConsoleHistory);
	
	m_Window->draw(m_ConsoleInputText);
	m_Window->draw(m_DebugString);
}

void render_system::DrawDebugString(const char* DebugString)
{
	m_DebugString.setString(DebugString);
	m_Window->draw(m_DebugString);
}

void render_system::Clear()
{
	m_Window->clear();
}

void render_system::Draw(sf::Sprite Sprite)
{
	m_Window->draw(Sprite);
}

void render_system::Display()
{
	m_Window->display();
}