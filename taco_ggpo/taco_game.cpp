#include "taco_game.h"
#include "render_system.h"
#include "console_system.h"
#include "subsystems.h"
#include "gamestate.h"
#include "permanent_state.h"
#include "player_graphics.h"
#include "script_manager.h"
#include "input_polling.h"
#include "debug_output.h"

// TASKS
// TODO: Set up struct or whatever for more permanent (or doesn't change frame to frame) state that gets passed around (input mapping etc, ggpo stuff)
// TODO: Set up controls properly + input_polling is a fucking mess
//			- Read input mapping from file
//			- Some way to set up controls at runtime
// TODO: Set up everything to account for two players instead of this test shit
// TODO: Set up stage + camera rules
// TODO: Figure out how to deal with coordinates 
//			- Origin of sprites (top-left, bottom-center etc.)
//				- rn using sprite height/width for offsetting doesn't look right because of variable sprite sizes + sfml positive y axis being down
//				- has to work nicely with horizontal flip 
// TODO: Script manager tings only halfway renamed to non-disgusting convention
// TODO: Same for input buffer
// TODO: Script manager is overall hella messy, streamline that update function
// TODO: Audio shit
// TODO: Is a custom memory allocator worth it? There isn't a lot of (if any) allocations during gameplay
// TODO: Basic menus
// TODO: Reloading scripts at runtime (either through menu or console)

// DESIGN
// TODO: The renderer interface is appaling
// TODO: Every other interface is also appaling
// TODO: Figure out where to house/how to work with assets
// TODO: Figure out where to house the camera (or let the renderer handle it)
// TODO: Is a renderer class even necessary? It's useful but doesn't really do renderer tings rn
// TODO: Name some of these enums perhaps? Undecided
// TODO: A lot of shit that probably shouldn't be is header only rn, figure out what to do with them

// Subsystems
console_system*	ConsoleSystem;
render_system*	RenderSystem;

// State
gamestate		GameState = { 0 };
permanent_state PermanentState;

// Assets/other
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
		PermanentState.Players[0].Type = PLAYER_TYPE_LOCAL;
		PermanentState.Players[1].Type = PLAYER_TYPE_DUMMY;
		
		PlayerGraphics.Initialize("data/sphere");
		ScriptManager.initializeTestData();
		DebugOutput.OutputMode = DEBUG_DRAW_NONE;

		ConsoleSystem->RegisterCVar("debug", &DebugOutput.OutputMode, "0", "1", "Display debug string\n0: none\n1: script playback", CVAR_INT);

		CreateDefaultInputMap();
	}

	/* TODO:
	RunFrame should pass inputs OK:d by GGPO to AdvanceFrame. AdvanceFrame (and whatever will handling updating gamestate) should only have to care about 
	what inputs belong to which player,	not the details of those players (local/remote/dummy).
	There should possibly be another layer of abstraction when getting a trigger from the command buffer to allow for directly setting a trigger,
	to allow for easier dummy behaviour.
	*/
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