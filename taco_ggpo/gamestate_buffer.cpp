#include "gamestate_buffer.h"
#include "subsystems.h"
#include "render_system.h"

gamestate_buffer::gamestate_buffer(gamestate* GameState)
{
	m_GameState = GameState;
	m_SaveSlot = 1;
	m_LastInputBufferUpdate = 0;
	m_ReplayBufferEntryCount = 0;
	m_ReplayBufferCursor = 0;
	m_ReplayingSlot = -1;
	m_IsRecordingInput = false;
	m_IsReplayingInput = false;
}

void gamestate_buffer::SetCaptureSlot(uint8 Slot)
{
	m_SaveSlot = Slot;
	std::string OutputString("GameStateBuffer slot #" + std::to_string(m_SaveSlot) + " selected");
	RenderSystem->SetDebugString(OutputString.c_str());
}

void gamestate_buffer::SaveGameState()
{
	std::string FileName = "Recorded state/Gamestate_" + std::to_string(m_SaveSlot) + ".gs";
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
	std::string FileName = "Recorded state/Gamestate_" + std::to_string(m_SaveSlot) + ".gs";
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

void gamestate_buffer::ToggleInputRecording()
{
	if (!m_IsRecordingInput)
	{
		std::string FileName = "Recorded state/InputReplay_" + std::to_string(m_SaveSlot) + ".rep";
		FILE* FilePtr;
		if (fopen_s(&FilePtr, FileName.c_str(), "rb") == 0)
		{
			fread(m_GameState, sizeof(gamestate), 1, FilePtr);
			fclose(FilePtr);

			std::string OutputString("Replay data already exists on slot #" + std::to_string(m_SaveSlot) + "!");
			RenderSystem->SetDebugString(OutputString.c_str());
		}
		else
		{
			m_IsRecordingInput = true;

			input_buffer* InputBuffer = &m_GameState->m_Player[0].InputBuffer;
			m_LastInputBufferUpdate = InputBuffer->m_Buffer[InputBuffer->m_Cursor].m_TimeStamp;

			SaveGameState();

			std::string OutputString("Input capture started on slot #" + std::to_string(m_SaveSlot));
			RenderSystem->SetDebugString(OutputString.c_str());
		}
	}
	else
	{
		m_IsRecordingInput = false;

		std::string OutputString("Input capture ended on slot #" + std::to_string(m_SaveSlot));
		RenderSystem->SetDebugString(OutputString.c_str());
	}
}

void gamestate_buffer::ToggleInputPlayback()
{
	if (!m_IsReplayingInput)
	{
		std::string FileName = "Recorded state/InputReplay_" + std::to_string(m_SaveSlot) + ".rep";
		FILE* FilePtr;
		if (fopen_s(&FilePtr, FileName.c_str(), "rb") == 0)
		{
			fseek(FilePtr, 0, SEEK_END);
			long Size = ftell(FilePtr);
			fclose(FilePtr);

			m_ReplayBufferEntryCount = Size / sizeof(input_buffer::buffer_entry);
			m_ReplayingSlot = (int8)m_SaveSlot;
			m_ReplayBufferCursor = 0;
			m_IsReplayingInput = true;

			LoadGameState();

			std::string OutputString("Starting replay playback from slot #" + std::to_string(m_ReplayingSlot));
			RenderSystem->SetDebugString(OutputString.c_str());
		}
	}
}

void gamestate_buffer::Update()
{
	if (m_IsRecordingInput)
	{
		input_buffer* InputBuffer = &m_GameState->m_Player[0].InputBuffer;
		if (InputBuffer->m_Buffer[InputBuffer->m_Cursor].m_TimeStamp > m_LastInputBufferUpdate)
		{
			m_LastInputBufferUpdate = InputBuffer->m_Buffer[InputBuffer->m_Cursor].m_TimeStamp;
			
			std::string FileName = "Recorded state/InputReplay_" + std::to_string(m_SaveSlot) + ".rep";
			FILE* FilePtr;
			if (fopen_s(&FilePtr, FileName.c_str(), "ab") == 0)
			{
				fwrite(&InputBuffer->m_Buffer[InputBuffer->m_Cursor], sizeof(input_buffer::buffer_entry), 1, FilePtr);
				fclose(FilePtr);

				std::string OutputString("Input captured on slot #" + std::to_string(m_SaveSlot));
				RenderSystem->SetDebugString(OutputString.c_str());
			}
		}
	}
	else if (m_IsReplayingInput)
	{
		std::string FileName = "Recorded state/InputReplay_" + std::to_string(m_ReplayingSlot) + ".rep";
		FILE* FilePtr;
		if (fopen_s(&FilePtr, FileName.c_str(), "rb") == 0)
		{
			input_buffer::buffer_entry BufferEntry;
			input_buffer* InputBuffer = &m_GameState->m_Player[0].InputBuffer;
			fseek(FilePtr, sizeof(input_buffer::buffer_entry) * m_ReplayBufferCursor, SEEK_SET);
			fread(&BufferEntry, sizeof(input_buffer::buffer_entry), 1, FilePtr);

			if (m_GameState->m_FrameCount == BufferEntry.m_TimeStamp)
			{
				++InputBuffer->m_Cursor %= INPUT_BUFFER_SIZE;

				InputBuffer->m_Buffer[InputBuffer->m_Cursor].m_InputMask = BufferEntry.m_InputMask;
				InputBuffer->m_Buffer[InputBuffer->m_Cursor].m_TimeStamp = BufferEntry.m_TimeStamp;
				m_ReplayBufferCursor++;
			}

			if (m_ReplayBufferCursor >= m_ReplayBufferEntryCount)
			{
				m_IsReplayingInput = 0;
				std::string OutputString("Replay finished");
				RenderSystem->SetDebugString(OutputString.c_str());
			}

			fclose(FilePtr);
		}
	}
}

bool gamestate_buffer::IsReplaying()
{
	return m_IsReplayingInput;
}
