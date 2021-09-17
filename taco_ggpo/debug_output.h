#pragma once
// TODO: Do something with the debug stuff
enum
{
	DEBUG_DRAW_NONE = 0,
	DEBUG_DRAW_SCRIPT_INFO
};

struct debug_output
{
	std::string String;
	uint32		OutputMode;
	void Update(character_script* Script, gamestate* GameState)
	{
		switch (OutputMode)
		{
		case DEBUG_DRAW_SCRIPT_INFO:
			String =	
				Script->m_ScriptName + "\n" +
				std::to_string(GameState->Player[0].PlaybackState.PlaybackCursor - 1) + " / " + 
				std::to_string(Script->m_FrameCount);
			break;
		case DEBUG_DRAW_NONE:
		default:
			String = "";
			break;
		}

	}
};