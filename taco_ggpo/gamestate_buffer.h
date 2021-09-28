#pragma once
#include "gamestate.h"

#define GAMESTATE_BUFFER_SLOT_COUNT 3

class gamestate_buffer
{
public:
	gamestate_buffer(gamestate* GameState);

	void SetSaveSlot(uint8 Slot);
	void SaveGameState();
	void LoadGameState();
private:
	struct saved_gamestate
	{
		saved_gamestate() : m_HasSavedState(false) {}
		gamestate	m_GameState;
		bool		m_HasSavedState;
	};
	saved_gamestate	m_SavedGameStates[GAMESTATE_BUFFER_SLOT_COUNT];
	gamestate*		m_GameState;
	uint8			m_SaveSlot;
};

