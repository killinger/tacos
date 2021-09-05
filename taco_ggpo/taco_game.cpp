#include "taco_game.h"
#include "render_system.h"
#include "console_system.h"
#include "subsystems.h"
#include "gamestate.h"
#include "player_graphics.h"
#include "script_manager.h"
#include "input_polling.h"
#include "debug_output.h"

// TASKS
// TODO: Set up struct or whatever for more permanent (or doesn't change frame to frame) state that gets passed around (input mapping etc, ggpo stuff)
// TODO: Set up controls properly
// TODO: Set up everything to account for two players instead of this test shit

// DESIGN
// TODO: The renderer interface is appaling
// TODO: Every other interface is also appaling
// TODO: Figure out where to house assets
// TODO: Figure out where to house the camera (or let the renderer handle it)
// TODO: Figure out how to deal with coordinates 
//			- Origin of sprites (top-left, bottom-center etc.)
//				- rn using sprite height/width for offsetting doesn't look correctly because of variable sprite sizes
//				- has to work nicely with horizontal flip 

// CLEAN UP
// TODO: Script manager tings only halfway renamed to non-disgusting convention
// TODO: ^Also input buffer tings

console_system*	ConsoleSystem;
render_system*	RenderSystem;
gamestate		GameState = { 0 };
player_graphics	PlayerGraphics;
script_manager	ScriptManager;
debug_output	DebugOutput;

namespace taco
{
	void AdvanceFrame();
	void UpdatePlayerState();

	void Initialize(sf::RenderWindow* Window)
	{
		ConsoleSystem = new console_system();
		RenderSystem = new render_system(Window);
		GameState.Initialize();
		PlayerGraphics.Initialize("data/sphere");
		ScriptManager.initializeTestData();
		DebugOutput.OutputMode = DEBUG_DRAW_NONE;

		ConsoleSystem->RegisterCVar("debug", &DebugOutput.OutputMode, "0", "1", "Display debug string\n0: none\n1: script playback", CVAR_INT);
	}

	void RunFrame()
	{
		uint32 Key = PollKeyboard();
		ConsoleSystem->Update(Key);

		// GGPO stuff here
		AdvanceFrame();
	}

	void AdvanceFrame()
	{
		UpdatePlayerState();

		RenderSystem->Clear();
		RenderSystem->Draw(PlayerGraphics.m_CharacterSprite);
		if (ConsoleSystem->m_IsActive)
			ConsoleSystem->DrawConsole();
		else if (DebugOutput.OutputMode != DEBUG_DRAW_NONE)
			RenderSystem->DrawDebugString(DebugOutput.String.c_str());
		RenderSystem->Display();

		GameState.FrameCount++;
	}

	/* TODO:
	1. Get current frame for both players
	2. Check collisions for both players
	3. Rest can be sequential 
	*/
	void UpdatePlayerState()
	{
		uint32 Input = PollInput();
		GameState.PlayerState[0].InputBuffer.Update(Input);

		character_script* Script = &ScriptManager.m_Scripts[GameState.PlayerState[0].PlaybackState.Script];
		frame_info* Frame = &Script->m_Frames[GameState.PlayerState[0].PlaybackState.PlaybackCursor];

		std::string Trigger = "";
		
		for (uint32 i = 0; i < ScriptManager.m_Commands.size(); i++)
		{
			if (GameState.PlayerState[0].InputBuffer.CheckForCommand(ScriptManager.m_Commands[i]))
			{
				Trigger = ScriptManager.m_Commands[i].Trigger;
				break;
			}
		}

		DebugOutput.Update(Script, &GameState);

		if (Frame->m_AnimationIndex != -1)
		{
			PlayerGraphics.SetAnimation(Frame->m_AnimationIndex);
		}
		PlayerGraphics.SetPosition(GameState.PlayerState[0].PositionX, GameState.PlayerState[0].PositionY, 1);

		ScriptManager.Update(&GameState.PlayerState[0].PlaybackState, Trigger);
		GameState.PlayerState[0].PlaybackState.Script = GameState.PlayerState[0].PlaybackState.PendingScript;
	}
}