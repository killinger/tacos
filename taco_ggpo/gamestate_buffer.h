#pragma once
#include "gamestate.h"

class gamestate_buffer
{
public:
	gamestate_buffer(gamestate* GameState);
	
	void SetCaptureSlot(uint8 Slot);
	void SaveGameState();
	void LoadGameState();
	void ToggleInputRecording();
	void ToggleInputPlayback();
	void Update();
	bool IsReplaying();
private:
	gamestate*		m_GameState;
	uint32			m_LastInputBufferUpdate;
	uint32			m_ReplayBufferEntryCount;
	uint32			m_ReplayBufferCursor;
	bool			m_IsRecordingInput;
	bool			m_IsReplayingInput;
	uint8			m_SaveSlot;
	int8			m_ReplayingSlot;
};