#pragma once
#include "gamestate.h"

class gamestate_buffer
{
public:
	gamestate_buffer(gamestate* GameState);

	void SetSaveSlot(uint8 Slot);
	void SaveGameState();
	void LoadGameState();
private:
	gamestate*		m_GameState;
	uint8			m_SaveSlot;
};