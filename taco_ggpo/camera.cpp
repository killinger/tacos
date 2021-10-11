#include "camera.h"
#include <stdlib.h>

#define MIN_POS_Y 25.0f
#define MIN_SIZE_X 480.0f
#define MIN_SIZE_Y 270.0f
#define MAX_SIZE_X 560.0f
#define MAX_SIZE_Y 315.0f

camera::camera()
{
	m_View.setSize(sf::Vector2f(480.0f, 270.0f));
	m_View.setCenter(sf::Vector2f(0.0f, -110.0f));
	m_LeftBound = m_View.getCenter().x - (m_View.getSize().x / 2.0f);
	m_RightBound = m_View.getCenter().x + (m_View.getSize().x / 2.0f);
}

void camera::Update(gamestate* Gamestate)
{
	// TODO: this needs research lol
	float PlayerMidpointX = (Gamestate->m_Player[0].PositionX + Gamestate->m_Player[1].PositionX) / 2.0f;
	float DistanceX = (Gamestate->m_Player[0].PositionX - Gamestate->m_Player[1].PositionX);
	DistanceX = abs(DistanceX);
	float CurrentSizeX = m_View.getSize().x;
	float CurrentPosX = m_View.getCenter().x;
	float CurrentPosY = m_View.getCenter().y;
	float AdjustmentY = 0.0f;
	if (DistanceX > (CurrentSizeX / 2.0f))
	{
		if (CurrentSizeX >= MAX_SIZE_X)
		{
			m_View.setSize(sf::Vector2f(MAX_SIZE_X, MAX_SIZE_Y));
			m_View.setCenter(PlayerMidpointX, CurrentPosY - AdjustmentY);
		}
		else
		{
			float NewSizeX = CurrentSizeX + (DistanceX - (CurrentSizeX / 2.0f));
			float NewSizeY = NewSizeX * 0.5625f;
			float LowestVisiblePoint = CurrentPosY + (NewSizeY / 2.0f);
			if (LowestVisiblePoint > MIN_POS_Y)
			{
				AdjustmentY = LowestVisiblePoint - MIN_POS_Y;
			}
			m_View.setSize(sf::Vector2f(NewSizeX, NewSizeY));
			m_View.setCenter(PlayerMidpointX, CurrentPosY - AdjustmentY);
		}
	}
	else if (DistanceX < 100.0f)
	{
		if (Gamestate->m_Player[0].PositionX > CurrentPosX &&
			Gamestate->m_Player[1].PositionX > CurrentPosX)
		{
			if (Gamestate->m_Player[0].PositionX < Gamestate->m_Player[1].PositionX)
			{
				m_View.setCenter(Gamestate->m_Player[0].PositionX, CurrentPosY - AdjustmentY);
			}
			else
			{
				m_View.setCenter(Gamestate->m_Player[1].PositionX, CurrentPosY - AdjustmentY);
			}
		}
		else if (	Gamestate->m_Player[0].PositionX < CurrentPosX &&
					Gamestate->m_Player[1].PositionX < CurrentPosX)
		{
			if (Gamestate->m_Player[0].PositionX > Gamestate->m_Player[1].PositionX)
			{
				m_View.setCenter(Gamestate->m_Player[0].PositionX, CurrentPosY - AdjustmentY);
			}
			else
			{
				m_View.setCenter(Gamestate->m_Player[1].PositionX, CurrentPosY - AdjustmentY);
			}
		}
	}
	else if (DistanceX < (CurrentSizeX / 2.5f))
	{
		if (CurrentSizeX <= MIN_SIZE_X)
		{
			m_View.setSize(sf::Vector2f(MIN_SIZE_X, MIN_SIZE_Y));
			m_View.setCenter(PlayerMidpointX, CurrentPosY - AdjustmentY);
		}
		else
		{
			float NewSizeX = CurrentSizeX + (DistanceX - (CurrentSizeX / 2.5f));
			float NewSizeY = NewSizeX * 0.5625f;
			float LowestVisiblePoint = CurrentPosY + (NewSizeY / 2.0f);
			if (LowestVisiblePoint < MIN_POS_Y)
			{
				AdjustmentY = LowestVisiblePoint - MIN_POS_Y;
			}
			m_View.setSize(sf::Vector2f(NewSizeX, NewSizeY));
			m_View.setCenter(PlayerMidpointX, CurrentPosY - AdjustmentY);
		}
	}
	else
	{
		m_View.setCenter(PlayerMidpointX, CurrentPosY - AdjustmentY);
	}
	
	m_LeftBound = m_View.getCenter().x - (m_View.getSize().x / 2.0f);
	m_RightBound = m_View.getCenter().x + (m_View.getSize().x / 2.0f);
}
