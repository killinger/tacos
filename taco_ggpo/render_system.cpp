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
	m_MainView.setCenter(sf::Vector2f(0.0f, 0.0f));
	m_MainView.setSize(sf::Vector2f(480, 270));

	m_MainFramebuffer.create(480, 270);
	//m_MainFramebuffer.setView(m_MainView);

	m_ConsoleInputText.setFont(m_ConsoleFont);
	m_ConsoleInputText.setCharacterSize(11);
	m_ConsoleInputText.setFillColor(sf::Color::White);
	m_ConsoleInputText.setPosition(sf::Vector2f(-240.0f, 115.0f));
	m_DebugString.setFont(m_ConsoleFont);
	m_DebugString.setCharacterSize(14);
	m_DebugString.setFillColor(sf::Color::White);
	m_DebugString.setPosition(-240.0f, -135.0f);
	m_WorldText.setFont(m_ConsoleFont);
	m_WorldText.setCharacterSize(10);
	m_WorldText.setFillColor(sf::Color::White);
	m_InputText.setFont(m_ConsoleFont);
	m_InputText.setCharacterSize(10);
	m_InputText.setFillColor(sf::Color::White);

	//m_Window->setView(m_MainView);

	const std::string FragmentShader = \
		"uniform sampler2D texture;" \
		"uniform vec2 Offset;"
		"void main()" \
		"{" \
		"vec2 Resolution = vec2(1280.0, 720.0);" \
		"vec2 UV = gl_FragCoord.xy / Resolution.xy;" \
		"vec4 Color;" \
		"Color.r = texture2D(texture, UV).r;" \
		"Color.g = texture2D(texture, UV - Offset).g;" \
		"Color.b = texture2D(texture, UV + Offset).b;" \
		"Color.a = 1.0;" \
		"gl_FragColor = gl_Color * Color;" \
		"}";
	if (!m_Shader.loadFromMemory(FragmentShader, sf::Shader::Fragment))
	{
		int g = 2;
	}
}

render_system::~render_system()
{
}

void render_system::DrawConsole(char* ConsoleInput, char* ConsoleHistory)
{
	m_MainFramebuffer.setView(m_MainView);
	m_ConsoleInputText.setString(">" + std::string(ConsoleInput));
	m_DebugString.setString(ConsoleHistory);	
	m_MainFramebuffer.draw(m_ConsoleInputText);
	m_MainFramebuffer.draw(m_DebugString);
}

void render_system::SetDebugString(const char* DebugString ...)
{
	char Buffer[512];
	va_list Args;
	va_start(Args, DebugString);
	vsnprintf(Buffer, 512, DebugString, Args);
	va_end(Args);
	m_DebugString.setString(Buffer);
}

void render_system::DrawDebugString(const char* DebugString)
{
	m_DebugString.setString(DebugString);
	m_MainFramebuffer.draw(m_DebugString);
}

void render_system::DrawDebugString()
{
	m_MainFramebuffer.setView(m_MainView);
	m_MainFramebuffer.draw(m_DebugString);
}

void render_system::DrawWorldText(float X, float Y, const char* WorldText ...)
{
	char Buffer[512];

	va_list Args;
	va_start(Args, WorldText);
	vsnprintf(Buffer, 512, WorldText, Args);
	va_end(Args);

	m_WorldText.setPosition(X, Y);
	m_WorldText.setString(Buffer);
	m_MainFramebuffer.setView(m_MainView);
	m_MainFramebuffer.draw(m_WorldText);
}

void render_system::Clear()
{
	m_Window->clear();
	m_MainFramebuffer.clear(sf::Color(25, 25, 25));
}

void render_system::Draw(sf::RectangleShape Rectangle, camera* Camera)
{
	m_MainFramebuffer.setView(Camera->m_View);
	m_MainFramebuffer.draw(Rectangle);
}

void render_system::Draw(sf::Sprite Sprite, camera* Camera)
{
	m_MainFramebuffer.setView(Camera->m_View);
	m_MainFramebuffer.draw(Sprite);
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

void render_system::DrawGUIElement(sf::Sprite& Sprite)
{
	m_MainFramebuffer.setView(m_MainView);
	m_MainFramebuffer.draw(Sprite);
}

void render_system::DrawInputText(float X, float Y, const char* InputText ...)
{
	m_MainFramebuffer.setView(m_MainView);

	char Buffer[512];

	va_list Args;
	va_start(Args, InputText);
	vsnprintf(Buffer, 512, InputText, Args);
	va_end(Args);

	m_InputText.setPosition(sf::Vector2f(X, Y));
	m_InputText.setString(Buffer);
	m_MainFramebuffer.draw(m_InputText);
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
	Framebuffer.setScale(2.666666667f, 2.666666667f);
	Framebuffer.setOrigin(sf::Vector2f(m_MainFramebuffer.getTexture().getSize() / 2u));
	Framebuffer.setPosition(m_Window->getView().getCenter());

	m_Window->draw(Framebuffer);
	m_Window->display();
}

sf::Vector2f render_system::GetViewCenter()
{
	return m_MainView.getCenter();
}
