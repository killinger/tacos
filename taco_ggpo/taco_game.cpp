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
// TODO: Is a custom memory allocator worth it? There isn't a lot of (if any) allocations during gameplay, but locality might be nice?
// TODO: Basic menus
// TODO: Reloading scripts at runtime (either through menu or console)
// TODO: When things are more comfortable, move all these non-detail TODOs outta here
// TODO: Streamline asset loading, and only load unique assets ie in case of a mirror match share the assets between both players
//		 ^ textures would be shared, not sprites

// DESIGN
// TODO: The renderer interface is appaling
// TODO: Every other interface is also appaling
// TODO: Figure out where to house/how to work with assets
// TODO: Figure out where to house the camera (or let the renderer handle it)
// TODO: Is a renderer class even necessary? It's useful but doesn't really do renderer tings rn
//		 ^ use RenderTexture to render off-screen, to set internal resolution
//		 ^ maybe some neat shader effects?
// TODO: Name some of these enums perhaps? Undecided
// TODO: A lot of shit that probably shouldn't be is header only rn, figure out what to do with them
// TODO: Should playerstate->script be index or pointer?
// TODO: Should animation be separated from scripts?

// TODO: This should find a more suitable home
#define PLAYER_STARTING_POSITIONS 80.0f

// Subsystems
console_system*	ConsoleSystem;
render_system*	RenderSystem;

// State
gamestate		GameState = { 0 };
permanent_state PermanentState;

// Assets/other
player_graphics	PlayerGraphics[2];
script_manager	ScriptManager;
debug_output	DebugOutput;

namespace taco
{
	void AdvanceFrame(uint32* Inputs);
	void UpdatePlayerState(uint32* Inputs);

	void Initialize(sf::RenderWindow* Window)
	{
		ConsoleSystem = new console_system();
		RenderSystem = new render_system(Window);

		GameState.Initialize();
		GameState.PlayerState[0].PositionX = -PLAYER_STARTING_POSITIONS;
		GameState.PlayerState[0].Facing = 1.0f;
		GameState.PlayerState[1].PositionX = PLAYER_STARTING_POSITIONS;
		GameState.PlayerState[1].Facing = -1.0f;

		PermanentState.Players[0].Type = PLAYER_TYPE_LOCAL;
		PermanentState.Players[1].Type = PLAYER_TYPE_DUMMY;

		PlayerGraphics[0].Initialize("data/sphere");
		PlayerGraphics[1].Initialize("data/sphere");
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

		uint32 Inputs[2] = { 0 };
		for (uint32 i = 0; i < 2; i++)
		{
			if (PermanentState.Players[i].Type == PLAYER_TYPE_LOCAL)
				Inputs[i] = PollInput();
		}

		// GGPO stuff here

		AdvanceFrame(Inputs);
	}

	void AdvanceFrame(uint32* Inputs)
	{
		UpdatePlayerState(Inputs);

		RenderSystem->Clear();
		RenderSystem->Draw(PlayerGraphics[0].m_CharacterSprite);
		RenderSystem->Draw(PlayerGraphics[1].m_CharacterSprite);
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
	void UpdatePlayerState(uint32* Inputs)
	{
		character_script* Scripts[2] =
		{
			&ScriptManager.m_Scripts[GameState.PlayerState[0].PlaybackState.Script],
			&ScriptManager.m_Scripts[GameState.PlayerState[1].PlaybackState.Script]
		};
		frame_info* Frames[2] =
		{
			&Scripts[0]->m_Frames[GameState.PlayerState[0].PlaybackState.PlaybackCursor],
			&Scripts[1]->m_Frames[GameState.PlayerState[1].PlaybackState.PlaybackCursor]
		};

		// TODO: Collision detection. It's ok for collision detection to be ineffective as there aren't going to be all that many checks per frame even in the worst case

		DebugOutput.Update(Scripts[0], &GameState);

		for (uint32 j = 0; j < 2; j++)
		{
			GameState.PlayerState[j].InputBuffer.Update(Inputs[j]);
			std::string Trigger = "";

			// TODO: Add another one of those script managers
			// TODO: There needs to be additional checks, such as meter requirement/availability 
			for (uint32 i = 0; i < ScriptManager.m_Commands.size(); i++)
			{
				if (GameState.PlayerState[j].InputBuffer.CheckForCommand(ScriptManager.m_Commands[i]))
				{
					Trigger = ScriptManager.m_Commands[i].Trigger;
					break;
				}
			}

			// TODO: -1 should be a define, not magic number. also should never happen
			if (Frames[j]->m_AnimationIndex != -1)
			{
				PlayerGraphics[j].SetAnimation(Frames[j]->m_AnimationIndex);
			}
			PlayerGraphics[j].SetPosition(
				GameState.PlayerState[j].PositionX, 
				GameState.PlayerState[j].PositionY, 
				GameState.PlayerState[j].Facing);

			// TODO: At this stage, is there a point to having a pending script if updating the playback state is the last operation? 
			// Is there ever anything you'd want to do between updating the playback state and committing to switching script?
			ScriptManager.Update(&GameState.PlayerState[j].PlaybackState, Trigger);
			GameState.PlayerState[j].PlaybackState.Script = GameState.PlayerState[j].PlaybackState.PendingScript;
		}
	}
}