#include "render_system.h"

// TODO: error checking
// TODO: view tings.. camera class?

render_system::render_system(sf::RenderWindow* Window)
{
	m_Window = Window;

	if (!m_ConsoleFont.loadFromFile("consola.ttf"))
	{
		//
	}

	m_DebugView = m_Window->getDefaultView();
	m_MainView = m_Window->getDefaultView();
	m_MainView.setCenter(sf::Vector2f(0.0f, -80.0f));
	m_MainView.setSize(sf::Vector2f(640, 360));

	m_MainFramebuffer.create(640, 360);
	m_MainFramebuffer.setView(m_MainView);

	m_ConsoleInputText.setFont(m_ConsoleFont);
	m_ConsoleInputText.setCharacterSize(11);
	m_ConsoleInputText.setFillColor(sf::Color::White);
	m_ConsoleInputText.setPosition(sf::Vector2f(-320.0f, 75.0f));
	m_DebugString.setFont(m_ConsoleFont);
	m_DebugString.setCharacterSize(14);
	m_DebugString.setFillColor(sf::Color::White);
	m_DebugString.setPosition(-320.0f, -260.0f);

	//m_Window->setView(m_MainView);
}

render_system::~render_system()
{
}

void render_system::DrawConsole(char* ConsoleInput, char* ConsoleHistory)
{
	m_ConsoleInputText.setString(">" + std::string(ConsoleInput));
	m_DebugString.setString(ConsoleHistory);	
	
	m_MainFramebuffer.draw(m_ConsoleInputText);
	m_MainFramebuffer.draw(m_DebugString);
}

void render_system::DrawDebugString(const char* DebugString)
{
	m_DebugString.setString(DebugString);
	m_MainFramebuffer.draw(m_DebugString);
}

void render_system::Clear()
{
	m_Window->clear();
	m_MainFramebuffer.clear(sf::Color(45, 45, 45));
}

void render_system::Draw(sf::Sprite Sprite)
{
	//m_Window->setView(m_MainView);
	m_MainFramebuffer.draw(Sprite);
}

void render_system::Draw(sf::RectangleShape Rectangle)
{
	//m_Window->setView(m_MainView);
	m_MainFramebuffer.draw(Rectangle);
}

void render_system::DrawLine(float P0X, float P0Y, float P1X, float P1Y)
{
	sf::Vertex Points[2] =
	{
		sf::Vector2f(P0X, P0Y),
		sf::Vector2f(P1X, P1Y)
	};
	m_Window->draw(Points, 2, sf::Lines);
}

void render_system::Display()
{
	m_MainFramebuffer.display();
	sf::Sprite Framebuffer(m_MainFramebuffer.getTexture());
	Framebuffer.setScale({ 2.0f, 2.0f });
	Framebuffer.setOrigin(sf::Vector2f(m_MainFramebuffer.getTexture().getSize() / 2u));
	Framebuffer.setPosition(m_Window->getView().getCenter());
	m_Window->draw(Framebuffer);
	m_Window->display();
}

sf::Vector2f render_system::GetViewCenter()
{
	return m_MainView.getCenter();
}
