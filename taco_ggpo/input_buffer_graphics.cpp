#include "input_buffer_graphics.h"
#include "subsystems.h"
#include "render_system.h"

#define START_X -240.0f
#define ARROW_OFFSET 0.0f
#define BUTTON_A_OFFSET 16.0f
#define BUTTON_B_OFFSET 32.0f
#define BUTTON_C_OFFSET 48.0f
#define TEXT_OFFSET 70.0f
#define ROW_OFFSET 16.0f
#define INPUT_BUFFER_HISTORY_SIZE 16

void input_buffer_graphics::Initialize()
{
	m_ArrowTexture_1.loadFromFile("data/arrow 1.png");
	m_ArrowTexture_2.loadFromFile("data/arrow 2.png");
	m_ArrowTexture_3.loadFromFile("data/arrow 3.png");
	m_ArrowTexture_4.loadFromFile("data/arrow 4.png");
	m_ArrowTexture_6.loadFromFile("data/arrow 6.png");
	m_ArrowTexture_7.loadFromFile("data/arrow 7.png");
	m_ArrowTexture_8.loadFromFile("data/arrow 8.png");
	m_ArrowTexture_9.loadFromFile("data/arrow 9.png");
	m_ButtonTexture_A.loadFromFile("data/button a.png");
	m_ButtonTexture_B.loadFromFile("data/button b.png");
	m_ButtonTexture_C.loadFromFile("data/button c.png");
}

void input_buffer_graphics::Update(input_buffer* InputBuffer)
{
	uint32 CurrentEntry = InputBuffer->m_Cursor;
	float CurrentY = -135.0f;
	for (uint32 i = 0; i < INPUT_BUFFER_HISTORY_SIZE; i++)
	{
		bool DrewIcon = false;
		m_Sprite.setPosition(START_X + ARROW_OFFSET, CurrentY);
		m_Sprite.setColor(sf::Color(255, 255, 255, 255));
		if (InputBuffer->m_Buffer[CurrentEntry].m_InputMask & INPUT_DOWN)
		{
			if (InputBuffer->m_Buffer[CurrentEntry].m_InputMask & INPUT_LEFT)
			{
				// 1
				m_Sprite.setTexture(m_ArrowTexture_1);
				RenderSystem->DrawGUIElement(m_Sprite);
				DrewIcon = true;
			}
			else if (InputBuffer->m_Buffer[CurrentEntry].m_InputMask & INPUT_RIGHT)
			{
				// 3
				m_Sprite.setTexture(m_ArrowTexture_3);
				RenderSystem->DrawGUIElement(m_Sprite);
				DrewIcon = true;
			}
			else
			{
				// 2
				m_Sprite.setTexture(m_ArrowTexture_2);
				RenderSystem->DrawGUIElement(m_Sprite);
				DrewIcon = true;
			}
		}
		else if (InputBuffer->m_Buffer[CurrentEntry].m_InputMask & INPUT_UP)
		{
			if (InputBuffer->m_Buffer[CurrentEntry].m_InputMask & INPUT_LEFT)
			{
				// 7
				m_Sprite.setTexture(m_ArrowTexture_7);
				RenderSystem->DrawGUIElement(m_Sprite);
				DrewIcon = true;
			}
			else if (InputBuffer->m_Buffer[CurrentEntry].m_InputMask & INPUT_RIGHT)
			{
				// 9
				m_Sprite.setTexture(m_ArrowTexture_9);
				RenderSystem->DrawGUIElement(m_Sprite);
				DrewIcon = true;
			}
			else
			{
				// 8
				m_Sprite.setTexture(m_ArrowTexture_8);
				RenderSystem->DrawGUIElement(m_Sprite);
				DrewIcon = true;
			}
		}
		else if (InputBuffer->m_Buffer[CurrentEntry].m_InputMask & INPUT_LEFT)
		{
			// 4
			m_Sprite.setTexture(m_ArrowTexture_4);
			RenderSystem->DrawGUIElement(m_Sprite);
			DrewIcon = true;
		}
		else if (InputBuffer->m_Buffer[CurrentEntry].m_InputMask & INPUT_RIGHT)
		{
			// 6
			m_Sprite.setTexture(m_ArrowTexture_6);
			RenderSystem->DrawGUIElement(m_Sprite);
			DrewIcon = true;
		}
		if (InputBuffer->m_Buffer[CurrentEntry].m_InputMask & INPUT_A)
		{
			m_Sprite.setPosition(START_X + BUTTON_A_OFFSET, CurrentY);
			m_Sprite.setTexture(m_ButtonTexture_A);
			if (!((InputBuffer->m_Buffer[CurrentEntry].m_ConsumedMask >> 5) & INPUT_A))
			{
				m_Sprite.setColor(sf::Color(100, 100, 100, 255));
			}
			else
			{
				m_Sprite.setColor(sf::Color(255, 255, 255, 255));
			}
			RenderSystem->DrawGUIElement(m_Sprite);
			DrewIcon = true;
		}
		if (InputBuffer->m_Buffer[CurrentEntry].m_InputMask & INPUT_B)
		{
			m_Sprite.setPosition(-240.0f + BUTTON_B_OFFSET, CurrentY);
			m_Sprite.setTexture(m_ButtonTexture_B);
			if (!((InputBuffer->m_Buffer[CurrentEntry].m_ConsumedMask >> 5) & INPUT_B))
			{
				m_Sprite.setColor(sf::Color(100, 100, 100, 255));
			}
			else
			{
				m_Sprite.setColor(sf::Color(255, 255, 255, 255));
			}
			RenderSystem->DrawGUIElement(m_Sprite);
			DrewIcon = true;
		}
		if (InputBuffer->m_Buffer[CurrentEntry].m_InputMask & INPUT_C)
		{
			m_Sprite.setPosition(-240.0f + BUTTON_C_OFFSET, CurrentY);
			m_Sprite.setTexture(m_ButtonTexture_C);
			if (!((InputBuffer->m_Buffer[CurrentEntry].m_ConsumedMask >> 5) & INPUT_C))
			{
				m_Sprite.setColor(sf::Color(100, 100, 100, 255));
			}
			else
			{
				m_Sprite.setColor(sf::Color(255, 255, 255, 255));
			}
			RenderSystem->DrawGUIElement(m_Sprite);
			DrewIcon = true;
		}
		if (DrewIcon)
		{
			uint32 PreviousEntry = InputBuffer->m_Buffer[CurrentEntry].m_PrevEntry;
			RenderSystem->DrawInputText(-240.0f + TEXT_OFFSET, CurrentY, "%i", InputBuffer->m_Buffer[CurrentEntry].m_TimeStamp - InputBuffer->m_Buffer[PreviousEntry].m_TimeStamp);
		}
		CurrentY += ROW_OFFSET;
		CurrentEntry = InputBuffer->m_Buffer[CurrentEntry].m_PrevEntry;
	}
}

void input_buffer_graphics::Render()
{
}
