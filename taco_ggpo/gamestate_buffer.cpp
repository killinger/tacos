#include "gamestate_buffer.h"
#include "subsystems.h"
#include "render_system.h"

gamestate_buffer::gamestate_buffer(gamestate* GameState)
{
	m_SaveSlot = 0;
	m_GameState = GameState;
}

void gamestate_buffer::SetSaveSlot(uint8 Slot)
{
	if (Slot < GAMESTATE_BUFFER_SLOT_COUNT)
	{
		m_SaveSlot = Slot;
		std::string OutputString("GameStateBuffer slot #" + std::to_string(m_SaveSlot) + " selected");
		RenderSystem->SetDebugString(OutputString.c_str());
	}
}

void gamestate_buffer::SaveGameState()
{
	memcpy_s(&m_SavedGameStates[m_SaveSlot].m_GameState, sizeof(gamestate), m_GameState, sizeof(gamestate));
	m_SavedGameStates[m_SaveSlot].m_HasSavedState = true;

	std::string OutputString("GameState saved in buffer slot #" + std::to_string(m_SaveSlot));
	RenderSystem->SetDebugString(OutputString.c_str());
}

void gamestate_buffer::LoadGameState()
{
	if (m_SavedGameStates[m_SaveSlot].m_HasSavedState)
	{
		memcpy_s(m_GameState, sizeof(gamestate), &m_SavedGameStates[m_SaveSlot].m_GameState, sizeof(gamestate));
		std::string OutputString("GameState loaded from buffer slot #" + std::to_string(m_SaveSlot));
		RenderSystem->SetDebugString(OutputString.c_str());
	}
	else
	{
		std::string OutputString("GameStateBuffer slot #" + std::to_string(m_SaveSlot) + " has no saved GameState!");
		RenderSystem->SetDebugString(OutputString.c_str());
	}
}
