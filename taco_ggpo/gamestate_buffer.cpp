#include "gamestate_buffer.h"
#include "subsystems.h"
#include "render_system.h"

gamestate_buffer::gamestate_buffer(gamestate* GameState)
{
	m_SaveSlot = 1;
	m_GameState = GameState;
}

void gamestate_buffer::SetSaveSlot(uint8 Slot)
{
	m_SaveSlot = Slot;
	std::string OutputString("GameStateBuffer slot #" + std::to_string(m_SaveSlot) + " selected");
	RenderSystem->SetDebugString(OutputString.c_str());
}

void gamestate_buffer::SaveGameState()
{
	std::string FileName = "Captured data/Gamestate_" + std::to_string(m_SaveSlot) + ".gs";
	FILE* FilePtr;
	if (fopen_s(&FilePtr, FileName.c_str(), "wb") == 0)
	{
		fwrite(m_GameState, sizeof(gamestate), 1, FilePtr);
		fclose(FilePtr);
	}

	std::string OutputString("GameState saved in slot #" + std::to_string(m_SaveSlot));
	RenderSystem->SetDebugString(OutputString.c_str());
}

void gamestate_buffer::LoadGameState()
{
	std::string FileName = "Captured data/Gamestate_" + std::to_string(m_SaveSlot) + ".gs";
	FILE* FilePtr;
	if (fopen_s(&FilePtr, FileName.c_str(), "rb") == 0)
	{
		fread(m_GameState, sizeof(gamestate), 1, FilePtr);
		fclose(FilePtr);

		std::string OutputString("GameState loaded from slot #" + std::to_string(m_SaveSlot));
		RenderSystem->SetDebugString(OutputString.c_str());
	}
	else
	{
		std::string OutputString("GameStateBuffer slot #" + std::to_string(m_SaveSlot) + " has no saved GameState!");
		RenderSystem->SetDebugString(OutputString.c_str());
	}
}
