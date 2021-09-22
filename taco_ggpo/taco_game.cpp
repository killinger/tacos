#include "taco_game.h"
#include "render_system.h"
#include "console_system.h"
#include "input_handler.h"
#include "subsystems.h"
#include "gamestate.h"
#include "permanent_state.h"
#include "player_graphics.h"
#include "script_manager.h"
#include "debug_output.h"
#include "logging_system.h"
// TEST TINGS REMOVE AFTER IMPLEMENTATION
#include "script_handler.h"
#include <unordered_map>

// TASKS
// TODO: Input buffers can not be a part of game state. It would cause inputs to drop like a motherfucker.
//		 ^ i don't know how we'd deal with inputs possibly ending up in the wrong direction but it's much less of a problem than dropped inputs on every rollback
// TODO: Redo input
//		 ^ priority, will write up requirements
// TODO: Set up struct or whatever for more permanent (or doesn't change frame to frame) state that gets passed around (input mapping etc, ggpo stuff)
// TODO: Set up controls properly
//			- Read input mapping from file
//			- Some way to set up controls at runtime
// TODO: Set up everything to account for two players instead of this test shit
// TODO: Set up stage + camera rules + debug camera to visualize bounds
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
// TODO: Get a def for Facing
// TODO: Move some defs out of the common file, not everything is that general
// TODO: There has to be a better way to deal with transforms. All the shifting shit when doing collision detection and drawing is disgusting
// TODO: some kinda scene graph shit?

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
// TODO: Consider having an "apply walkspeed" flag in scripts instead of using x shift, so walkspeed can easily be changed at runtime.
//		 ^ same deal with run etc
//		 ^ x shift would be used for small shifts during moves 
// TODO: Consider changing the format of collision boxes to left/right/top/bottom as it's better for collision detection

// TODO: This should find a more suitable home
#define PLAYER_STARTING_POSITIONS 80.0f

// Subsystems
console_system* ConsoleSystem;
render_system* RenderSystem;
logging_system* LoggingSystem;
input_handler* InputHandler;

// State
gamestate		GameState = { 0 };
permanent_state PermanentState;

// Assets/other
player_graphics	PlayerGraphics[2];
script_manager	ScriptManager;
debug_output	DebugOutput;

// TEST TINGS REMOVE AFTER IMPLEMENTATION
script_handler	ScriptHandler;
input_buffer		InputBuffer;
move_description TestMove;

struct input_frame
{
	uint32 InputMask;
	uint32 FrameNumber;
};

struct input_sequence
{
	input_sequence()
	{
		memset(Inputs, 0, sizeof(Inputs));
		Cursor = 0;
		InputCount = 0;
	}

	void SetInputAtFrame(uint32 InputMask, uint32 Frame)
	{
		Inputs[InputCount].InputMask = InputMask;
		Inputs[InputCount].FrameNumber = Frame;

		InputCount++;
	}

	uint32 GetNextInput(uint32 Frame)
	{
		uint32 ReturnValue = 0;
		if (Inputs[Cursor].FrameNumber == Frame)
		{
			ReturnValue = Inputs[Cursor].InputMask;
			Cursor++;
		}
		return ReturnValue;
	}

	input_frame Inputs[30];
	uint32		Cursor;
	uint32		InputCount;
};

input_sequence ReplayData;

namespace taco
{
	void AdvanceFrame(uint32* Inputs);

	// TEST TINGS REMOVE AFTER IMPLEMENTATION
	void UpdateGameState(uint32* Inputs);
	void UpdateTest(uint32* Inputs);
	void DrawCollisionBoxes();

	void Initialize(sf::RenderWindow* Window)
	{
		ConsoleSystem = new console_system();
		RenderSystem = new render_system(Window);
		LoggingSystem = new logging_system();
		InputHandler = new input_handler();

		GameState.Initialize();
		GameState.Player[0].PositionX = -PLAYER_STARTING_POSITIONS;
		GameState.Player[0].Facing = 1.0f;
		GameState.Player[1].PositionX = PLAYER_STARTING_POSITIONS;
		GameState.Player[1].Facing = -1.0f;

		PermanentState.Player[0].Type = PLAYER_TYPE_LOCAL;
		PermanentState.Player[0].InputBuffer.Initialize();
		PermanentState.Player[0].WalkspeedForward = 1.34f;
		PermanentState.Player[0].JumpGravity = -0.21f;
		PermanentState.Player[0].JumpInitialVelocity = 8.2f;
		PermanentState.Player[0].JumpForwardVelocity = 0.36f;

		PermanentState.Player[1].Type = PLAYER_TYPE_DUMMY;
		PermanentState.Player[1].InputBuffer.Initialize();


		PlayerGraphics[0].Initialize("data/sphere");
		PlayerGraphics[1].Initialize("data/sphere");
		//ScriptManager.initializeTestData();
		DebugOutput.OutputMode = DEBUG_DRAW_NONE;

		ConsoleSystem->RegisterCVar("debug", &DebugOutput.OutputMode, "0", "1", "Display debug string\n0: none\n1: script playback", CVAR_INT);
		ConsoleSystem->RegisterCVar("script", &GameState.Player[0].PlaybackState.Script, "0", "3", "Set script", CVAR_INT);
		ConsoleSystem->RegisterCVar("fspeed", &PermanentState.Player[0].WalkspeedForward, "0.0", "10.0", "Forward walkspeed", CVAR_FLOAT);
		ConsoleSystem->RegisterCVar("jgravity", &PermanentState.Player[0].JumpGravity, "-10.0", "0.0", "Jump gravity", CVAR_FLOAT);
		ConsoleSystem->RegisterCVar("jvelocity", &PermanentState.Player[0].JumpInitialVelocity, "0.0", "30.0", "Jump velocity", CVAR_FLOAT);

		// TEST TINGS REMOVE AFTER IMPLEMENTATION
		ScriptHandler.Initialize();
		GameState.Player[0].PlaybackState.Script = 0;
		GameState.Player[0].PlaybackState.PendingScript = 0;
		GameState.Player[0].PlaybackState.New = 1;
		GameState.Player[1].PlaybackState.New = 1;
		
		InputBuffer.Initialize();
		TestMove.m_Input.m_InputMask = INPUT_B;
		TestMove.m_Input.m_PropertyFlags = BUTTON_RESTRICTION_ALL;
		TestMove.m_MotionCount = 3;
		TestMove.m_Motion[0].m_BufferFrames = 8;
		TestMove.m_Motion[0].m_Input.m_InputMask = INPUT_DOWN;
		TestMove.m_Motion[0].m_Input.m_PropertyFlags = DIRECTION_RESTRICTION_EXACT;
		TestMove.m_Motion[1].m_BufferFrames = 8;
		TestMove.m_Motion[1].m_Input.m_InputMask = INPUT_DOWN | INPUT_RIGHT;
		TestMove.m_Motion[1].m_Input.m_PropertyFlags = DIRECTION_RESTRICTION_EXACT;
		TestMove.m_Motion[2].m_BufferFrames = 8;
		TestMove.m_Motion[2].m_Input.m_InputMask = INPUT_RIGHT;
		TestMove.m_Motion[2].m_Input.m_PropertyFlags = DIRECTION_RESTRICTION_EXACT;

		ReplayData.SetInputAtFrame(INPUT_DOWN, 0);
		ReplayData.SetInputAtFrame(INPUT_DOWN | INPUT_RIGHT, 1);
		ReplayData.SetInputAtFrame(INPUT_RIGHT | INPUT_B, 2);
	}

	/* TODO:
	RunFrame should pass inputs OK:d by GGPO to AdvanceFrame. AdvanceFrame (and whatever will handling updating gamestate) should only have to care about
	what inputs belong to which player,	not the details of those players (local/remote/dummy).
	There should possibly be another layer of abstraction when getting a trigger from the command buffer to allow for directly setting a trigger,
	to allow for easier dummy behaviour.
	*/
	void RunFrame()
	{
		uint32 Key = InputHandler->GetKey();
		ConsoleSystem->Update(Key);

		uint32 Inputs[2] = { 0 };
		for (uint32 i = 0; i < 2; i++)
		{
			if (PermanentState.Player[i].Type == PLAYER_TYPE_LOCAL)
				Inputs[i] = InputHandler->GetInputs();
		}

		// GGPO stuff here

		AdvanceFrame(Inputs);
	}

	void AdvanceFrame(uint32* Inputs)
	{
		/*LOG("[ FRAME START: %d ]\n", GameState.FrameCount);
		UpdateGameState(Inputs);*/

		RenderSystem->Clear();
		UpdateTest(Inputs);

		DrawCollisionBoxes();
		LoggingSystem->DrawLog();
		//if (ConsoleSystem->m_IsActive)
		//	ConsoleSystem->DrawConsole();

		RenderSystem->Display();

		GameState.FrameCount++;
	}

	// 1. Get correct script and cursor position
	// 		if cursor at last frame then end
	//			current script -> resting state (temporary or not depending on inputs)
	//			new -> true
	//		else if !new then step
	// 2. Get cancels 

	void UpdateGameState(uint32* Inputs)
	{
		//PermanentState.Player[0].InputBuffer.Update(Inputs[0], GameState.Player[0].Facing);
		//PermanentState.Player[1].InputBuffer.Update(Inputs[1], GameState.Player[1].Facing);

		// TODO: change script to pendingscript before final decision

		state_script* Script[2] =
		{
			ScriptHandler.GetScript(&GameState.Player[0].PlaybackState),
			ScriptHandler.GetScript(&GameState.Player[1].PlaybackState)
		};

		uint32 InitialScript = GameState.Player[0].PlaybackState.Script;

		LOG("Initial script: %s\n", Script[0]->Name.c_str());
		LOG("Initial script total frames: %d\n", Script[0]->TotalFrames);
		LOG("Initial cursor position: %d\n", GameState.Player[0].PlaybackState.PlaybackCursor);
		LOG("New: %d\n", GameState.Player[0].PlaybackState.New);

		LOG("Script is stepping\n");
		if (!GameState.Player[0].PlaybackState.New)
		{
			GameState.Player[0].PlaybackState.PlaybackCursor++;
			if (GameState.Player[0].PlaybackState.PlaybackCursor >= Script[0]->TotalFrames)
			{
				LOG("Script ending due to cursor reaching end of timeline\n");
				GameState.Player[0].PlaybackState.New = 1;

				LOG("Script is preliminarily set to resting state: ");
				if ((Script[0]->Flags & SCRIPT_AIRBORNE))
				{
					GameState.Player[0].PlaybackState.Script = 1;
					LOG("airborne\n");

					if (GameState.Player[0].PositionY < 0.0f)
					{
						GameState.Player[0].VelocityX = 0.0f;
						GameState.Player[0].VelocityY = 0.0f;
						GameState.Player[0].AccelerationY = 0.0f;
						GameState.Player[0].AccelerationY = 0.0f;
						GameState.Player[0].PositionY = 0.0f;

						GameState.Player[0].PlaybackState.Script = 0;
						LOG("Script is airborne but has reached the ground\n");
					}
				}
				else
				{
					LOG("grounded\n");
					GameState.Player[0].PlaybackState.Script = 0;
				}
			}
		}

		Script[0] = ScriptHandler.GetScript(&GameState.Player[0].PlaybackState);

		cancel_list* CancelList = NULL;
		if ((Script[0]->Flags & 0x03) == SCRIPT_RESTING)
		{
			CancelList = ScriptHandler.GetCancelList(0);
		}
		else if ((Script[0]->Flags & 0x03) == 0x03)
		{
			CancelList = NULL;
		}
		else
		{
			for (uint32 i = 0; i < Script[0]->Elements.CancelCount; i++)
			{
				if (Script[0]->Elements.CancelElements[i].InRange(GameState.Player[0].PlaybackState.PlaybackCursor))
				{
					CancelList = ScriptHandler.GetCancelList(Script[0]->Elements.CancelElements[i].Index);
				}
			}
		}

		if (CancelList != NULL)
		{
			move_list* Move;
			for (int i = 0; i < CancelList->MoveCount; i++)
			{
				//Move = ScriptHandler.GetMove(CancelList->Moves[i]);
				//if ((PermanentState.Player[0].InputBuffer.m_InputStates[0].DirectionState.Direction & Move->InputMask) == Move->InputMask)
				//{
				//	if (GameState.Player[0].PlaybackState.Script == Move->ScriptIndex)
				//	{
				//		GameState.Player[0].PlaybackState.New = 0;
				//		LOG("Continue playback\n");
				//	}
				//	//else if (	InitialScript == Move->ScriptIndex && 
				//	//			GameState.Player[0].PlaybackState.New)
				//	//{
				//	//	GameState.Player[0].PlaybackState.New = 0;
				//	//	LOG("Script is looping\n");
				//	//}
				//	else // if can cancel
				//	{
				//		GameState.Player[0].PlaybackState.New = 1;
				//		LOG("Transition due to cancel\n");
				//	}
				//	GameState.Player[0].PlaybackState.Script = Move->ScriptIndex;
				//	Script[0] = ScriptHandler.GetScript(&GameState.Player[0].PlaybackState);

				//	break;
				//}
			}
		}

		LOG("Script decided: %s\n", Script[0]->Name.c_str());

		if (GameState.Player[0].PlaybackState.New)
		{
			GameState.Player[0].PlaybackState.PlaybackCursor = 0;
			GameState.Player[0].VelocityX *= Script[0]->ScalingXV;
			GameState.Player[0].VelocityY *= Script[0]->ScalingYV;
			GameState.Player[0].AccelerationX *= Script[0]->ScalingXA;
			GameState.Player[0].AccelerationY *= Script[0]->ScalingYA;
			GameState.Player[0].PlaybackState.New = 0;
			LOG("Script is initialized\n");
		}

		LOG("Cursor: %d, Duration %d\n", GameState.Player[0].PlaybackState.PlaybackCursor, Script[0]->TotalFrames);

		for (uint32 i = 0; i < Script[0]->Elements.ForceCount; i++)
		{
			if (Script[0]->Elements.ForceElements[i].InRange(GameState.Player[0].PlaybackState.PlaybackCursor))
			{
				switch (Script[0]->Elements.ForceElements[i].Target)
				{
				case FORCE_VELOCITY_X:
					GameState.Player[0].VelocityX = Script[0]->Elements.ForceElements[i].Amount; break;
				case FORCE_VELOCITY_Y:
					GameState.Player[0].VelocityY = Script[0]->Elements.ForceElements[i].Amount; break;
				case FORCE_ACCELERATION_X:
					GameState.Player[0].AccelerationX = Script[0]->Elements.ForceElements[i].Amount; break;
				case FORCE_ACCELERATION_Y:
					GameState.Player[0].AccelerationY = Script[0]->Elements.ForceElements[i].Amount; break;
				default:
					break;
				}
			}
		}

		GameState.Player[0].PositionX += GameState.Player[0].VelocityX;
		GameState.Player[0].PositionY += GameState.Player[0].VelocityY;
		GameState.Player[0].VelocityX += GameState.Player[0].AccelerationX;
		GameState.Player[0].VelocityY += GameState.Player[0].AccelerationY;


		// TODO: if the pushboxes never change during the course of a single script execution this can be simplified
		collision_box Pushbox[2];
		for (uint32 i = 0; i < 2; i++)
		{
			for (uint32 j = 0; j < Script[i]->Elements.PushboxCount; j++)
			{
				if (Script[i]->Elements.PushboxElements[j].InRange(GameState.Player[i].PlaybackState.PlaybackCursor))
				{
					if (GameState.Player[i].Facing > 0.0f)
					{
						Pushbox[i].X = Script[i]->Elements.PushboxElements[j].Box.X + GameState.Player[i].PositionX;
						Pushbox[i].Y = Script[i]->Elements.PushboxElements[j].Box.Y + GameState.Player[i].PositionY;
						Pushbox[i].Width = Script[i]->Elements.PushboxElements[j].Box.Width;
						Pushbox[i].Height = Script[i]->Elements.PushboxElements[j].Box.Height;
					}
					else
					{


						Pushbox[i].X = GameState.Player[i].PositionX - (Script[i]->Elements.PushboxElements[j].Box.X + Script[i]->Elements.PushboxElements[j].Box.Width);
						Pushbox[i].Y = Script[i]->Elements.PushboxElements[j].Box.Y + GameState.Player[i].PositionY;
						Pushbox[i].Width = Script[i]->Elements.PushboxElements[j].Box.Width;
						Pushbox[i].Height = Script[i]->Elements.PushboxElements[j].Box.Height;
					}
				}
			}
		}

		collision_box Result;
		if (BoxIntersection(&Pushbox[0], &Pushbox[1], &Result))
		{
			// Repulsion
			// TODO: Corner correction and edge cases
			GameState.Player[0].PositionX -= GameState.Player[0].Facing * (Result.Width / 2.0f);
			GameState.Player[1].PositionX -= GameState.Player[1].Facing * (Result.Width / 2.0f);
		}

		LOG("[END OF FRAME]\n\n");
	}

	enum jump_direction
	{
		None,
		NeutralJump,
		ForwardJump,
		BackJump
	};

	enum state
	{
		Standing,
		Forward,
		Back,
		Crouch,
		Prejump,
		Jump,
		Stand2Crouch,
		Crouch2Stand,
		Move,
		StateCount
	};

	state State = Standing;
	jump_direction BufferedJump = None;

	typedef state_script* (*script_selector)(playerstate* PlayerState, uint32 Inputs, player_info* PlayerInfo);

	state_script* StandingScriptSelector(playerstate* PlayerState, uint32 Inputs, player_info* PlayerInfo)
	{
		//LOG("%s\n", __FUNCTION__);
		
		state_script* Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
		bool ScriptFinished = false;
		if (!PlayerState->PlaybackState.New)
		{
			PlayerState->PlaybackState.PlaybackCursor++;
			if (PlayerState->PlaybackState.PlaybackCursor >= Script->TotalFrames)
			{
				PlayerState->PlaybackState.PlaybackCursor = 0;
				ScriptFinished = true;
			}
		}
		PlayerState->PlaybackState.New = 0;

		BufferedJump = None;
		if (Inputs & INPUT_UP)
		{
			//LOG("TRANSITION: Prejump, ");
			PlayerState->PlaybackState.PlaybackCursor = 0;
			PlayerState->PlaybackState.Script = 4;
			Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
			State = Prejump;
			if (Inputs & INPUT_RIGHT)
			{
				//LOG("buffered forward jump\n");
				BufferedJump = ForwardJump;
			}
			else if (Inputs & INPUT_LEFT)
			{
				//LOG("buffered back jump\n");
				BufferedJump = BackJump;
			}
			else
			{
				//LOG("buffered neutral jump\n");
				BufferedJump = NeutralJump;
			}
		}
		else if (Inputs & INPUT_RIGHT)
		{
			//LOG("TRANSITION: Forward\n");
			PlayerState->PlaybackState.PlaybackCursor = 0;
			PlayerState->PlaybackState.Script = 2;
			Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
			State = Forward;
		}
		else if (Inputs & INPUT_LEFT)
		{
			//LOG("TRANSITION: Back\n");
			PlayerState->PlaybackState.PlaybackCursor = 0;
			PlayerState->PlaybackState.Script = 3;
			Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
			State = Back;
		}
		else if (Inputs & INPUT_A)
		{
			PlayerState->PlaybackState.PlaybackCursor = 0;
			PlayerState->PlaybackState.Script = 6;
			Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
			//LOG("TRANSITION: Move, script name: %s\n", Script->Name.c_str());
			State = Move;
		}
		else
		{
			if (ScriptFinished)
			{
				//LOG("Script finished, looping\n");
				PlayerState->PlaybackState.PlaybackCursor = 0;
			}
			PlayerState->PlaybackState.Script = 0;
			Script = ScriptHandler.GetScript(&GameState.Player[0].PlaybackState);
		}
		return Script;
	}
	state_script* ForwardScriptSelector(playerstate* PlayerState, uint32 Inputs, player_info* PlayerInfo)
	{
		//LOG("%s\n", __FUNCTION__);

		state_script* Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
		bool ScriptFinished = false;
		if (!PlayerState->PlaybackState.New)
		{
			PlayerState->PlaybackState.PlaybackCursor++;
			if (PlayerState->PlaybackState.PlaybackCursor >= Script->TotalFrames)
			{
				PlayerState->PlaybackState.PlaybackCursor = 0;
				ScriptFinished = true;
			}
		}
		PlayerState->PlaybackState.New = 0;
		if (Inputs & INPUT_UP)
		{
			//LOG("TRANSITION: Prejump, ");
			PlayerState->PlaybackState.PlaybackCursor = 0;
			PlayerState->PlaybackState.Script = 4;
			Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
			State = Prejump;
			if (Inputs & INPUT_RIGHT)
			{
				//LOG("buffered forward jump\n");
				BufferedJump = ForwardJump;
			}
			else if (Inputs & INPUT_LEFT)
			{
				//LOG("buffered back jump\n");
				BufferedJump = BackJump;
			}
			else
			{
				//LOG("buffered neutral jump\n");
				BufferedJump = NeutralJump;
			}
		}
		else if (Inputs & INPUT_RIGHT)
		{
			if (ScriptFinished)
			{
				//LOG("Script finished, looping\n");
				PlayerState->PlaybackState.PlaybackCursor = 0;
			}
			PlayerState->PlaybackState.Script = 2;
			Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
			State = Forward;
		}
		else if (Inputs & INPUT_LEFT)
		{
			//LOG("TRANSITION: Back\n");
			PlayerState->PlaybackState.PlaybackCursor = 0;
			PlayerState->PlaybackState.Script = 3;
			Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
			State = Back;
		}
		else if (Inputs & INPUT_A)
		{
			PlayerState->PlaybackState.PlaybackCursor = 0;
			PlayerState->PlaybackState.Script = 6;
			Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
			//LOG("TRANSITION: Move, script name: %s\n", Script->Name.c_str());
			State = Move;
		}
		else
		{
			//LOG("TRANSITION: Standing\n");
			PlayerState->PlaybackState.PlaybackCursor = 0;
			PlayerState->PlaybackState.Script = 0;
			Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
			State = Standing;
		}

		return Script;
	}
	state_script* BackScriptSelector(playerstate* PlayerState, uint32 Inputs, player_info* PlayerInfo)
	{
		//LOG("%s\n", __FUNCTION__);

		state_script* Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
		bool ScriptFinished = false;
		if (!PlayerState->PlaybackState.New)
		{
			PlayerState->PlaybackState.PlaybackCursor++;
			if (PlayerState->PlaybackState.PlaybackCursor >= Script->TotalFrames)
			{
				PlayerState->PlaybackState.PlaybackCursor = 0;
				ScriptFinished = true;
			}
		}
		PlayerState->PlaybackState.New = 0;
		if (Inputs & INPUT_UP)
		{
			//LOG("TRANSITION: Prejump, ");
			PlayerState->PlaybackState.PlaybackCursor = 0;
			PlayerState->PlaybackState.Script = 4;
			Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
			State = Prejump;
			if (Inputs & INPUT_RIGHT)
			{
				//LOG("buffered forward jump\n");
				BufferedJump = ForwardJump;
			}
			else if (Inputs & INPUT_LEFT)
			{
				//LOG("buffered back jump\n");
				BufferedJump = BackJump;
			}
			else
			{
				//LOG("buffered neutral jump\n");
				BufferedJump = NeutralJump;
			}
		}
		else if (Inputs & INPUT_LEFT)
		{
			if (ScriptFinished)
			{
				//LOG("Script finished, looping\n");
				PlayerState->PlaybackState.PlaybackCursor = 0;
			}
			PlayerState->PlaybackState.Script = 3;
			Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
			State = Back;
		}
		else if (Inputs & INPUT_RIGHT)
		{
			//LOG("TRANSITION: Forward\n");
			PlayerState->PlaybackState.PlaybackCursor = 0;
			PlayerState->PlaybackState.Script = 2;
			Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
			State = Forward;
		}
		else if (Inputs & INPUT_A)
		{
			PlayerState->PlaybackState.PlaybackCursor = 0;
			PlayerState->PlaybackState.Script = 6;
			Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
			//LOG("TRANSITION: Move, script name: %s\n", Script->Name.c_str());
			State = Move;
		}
		else
		{
			//LOG("TRANSITION: Standing\n");
			PlayerState->PlaybackState.PlaybackCursor = 0;
			PlayerState->PlaybackState.Script = 0;
			Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
			State = Standing;
		}

		return Script;
	}
	state_script* CrouchScriptSelector(playerstate* PlayerState, uint32 Inputs, player_info* PlayerInfo)
	{
		//LOG("%s not implemented\n", __FUNCTION__);
		return NULL;
	}
	state_script* PrejumpScriptSelector(playerstate* PlayerState, uint32 Inputs, player_info* PlayerInfo)
	{
		//LOG("%s\n", __FUNCTION__);

		state_script* Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
		bool ScriptFinished = false;
		if (!PlayerState->PlaybackState.New)
		{
			PlayerState->PlaybackState.PlaybackCursor++;
			if (PlayerState->PlaybackState.PlaybackCursor >= Script->TotalFrames)
			{
				PlayerState->PlaybackState.PlaybackCursor = 0;
				ScriptFinished = true;
			}
		}
		PlayerState->PlaybackState.New = 0;

		if (ScriptFinished)
		{
			//LOG("TRANSITION: Jump\n");
			PlayerState->PlaybackState.Script = 1;
			Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
			State = Jump;
			PlayerState->PlaybackState.New = 1;
		}
		else
		{
			if (Inputs & INPUT_UP)
			{
				State = Prejump;
				if (Inputs & INPUT_RIGHT)
				{
					//LOG("buffered forward jump\n");
					BufferedJump = ForwardJump;
				}
				else if (Inputs & INPUT_LEFT)
				{
					//LOG("buffered back jump\n");
					BufferedJump = BackJump;
				}
				else
				{
					//LOG("buffered neutral jump\n");
					BufferedJump = NeutralJump;
				}
			}
		}

		return Script;
	}
	state_script* JumpScriptSelector(playerstate* PlayerState, uint32 Inputs, player_info* PlayerInfo)
	{
		//LOG("%s\n", __FUNCTION__);

		state_script* Script = ScriptHandler.GetScript(&PlayerState->PlaybackState);
		bool ScriptFinished = false;
		if (!PlayerState->PlaybackState.New)
		{
			PlayerState->PlaybackState.PlaybackCursor++;
			if (PlayerState->PlaybackState.PlaybackCursor >= Script->TotalFrames)
			{
				PlayerState->PlaybackState.PlaybackCursor = 0;
				ScriptFinished = true;
			}
		}
		PlayerState->PlaybackState.New = 0;

		return Script;
	}
	state_script* Stand2CrouchScriptSelector(playerstate* PlayerState, uint32 Inputs, player_info* PlayerInfo)
	{
		//LOG("%s not implemented\n", __FUNCTION__);
		return NULL;
	}
	state_script* Crouch2StandScriptSelector(playerstate* PlayerState, uint32 Inputs, player_info* PlayerInfo)
	{
		//LOG("%s not implemented\n", __FUNCTION__);
		return NULL;
	}

	script_selector ScriptSelector[StateCount] =
	{
		&StandingScriptSelector,
		&ForwardScriptSelector,
		&BackScriptSelector,
		&CrouchScriptSelector,
		&PrejumpScriptSelector,
		&JumpScriptSelector,
		&Stand2CrouchScriptSelector,
		&Crouch2StandScriptSelector
	};

	void UpdateTest(uint32* Inputs)
	{
		//LOG("[FRAME START %d]\n", GameState.FrameCount);

	/*	PermanentState.Player[0].InputBuffer.Update(Inputs[0], GameState.Player[0].Facing);
		PermanentState.Player[1].InputBuffer.Update(Inputs[1], GameState.Player[1].Facing);*/

		InputBuffer.Update(ReplayData.GetNextInput(0), 0, 1.0f);
		InputBuffer.Update(ReplayData.GetNextInput(1), 1, 1.0f);
		InputBuffer.Update(ReplayData.GetNextInput(2), 2, 1.0f);
		InputBuffer.MatchInputs(&TestMove, 3, 3);

		state_script* Script[2];
		Script[1] = ScriptHandler.GetScript(&GameState.Player[1].PlaybackState);
		
		Script[0] = ScriptSelector[State](&GameState.Player[0], Inputs[0], &PermanentState.Player[0]);
		if (Script[0] == NULL)
		{
			GameState.Player[0].PlaybackState.Script = 0;
			Script[0] = ScriptHandler.GetScript(&GameState.Player[0].PlaybackState);
			State = Standing;
		}

		if (GameState.Player[0].PlaybackState.PlaybackCursor == 0)
		{
			switch (State)
			{
			case taco::Forward:
				GameState.Player[0].VelocityX = PermanentState.Player[0].WalkspeedForward;
				break;
			case taco::Back:
				GameState.Player[0].VelocityX = -PermanentState.Player[0].WalkspeedForward;
				break;
			case taco::Jump:
				if (GameState.Player[0].PlaybackState.New)
				{
					GameState.Player[0].VelocityY = PermanentState.Player[0].JumpInitialVelocity;
					GameState.Player[0].AccelerationY = PermanentState.Player[0].JumpGravity;

					if (BufferedJump == ForwardJump)
					{
						GameState.Player[0].VelocityX = PermanentState.Player[0].JumpForwardVelocity;
					}
					else if (BufferedJump == BackJump)
					{
						GameState.Player[0].VelocityX = -PermanentState.Player[0].JumpForwardVelocity;
					}

					GameState.Player[0].PlaybackState.New = 0;
				}
				break;
			
			default:
				GameState.Player[0].VelocityX = 0.0f;
				GameState.Player[0].VelocityY = 0.0f;
				GameState.Player[0].AccelerationX = 0.0f;
				GameState.Player[0].AccelerationY = 0.0f;
				break;
			}
		}

		GameState.Player[0].PositionX += GameState.Player[0].VelocityX;
		GameState.Player[0].PositionY += GameState.Player[0].VelocityY;
		GameState.Player[0].VelocityX += GameState.Player[0].AccelerationX;
		GameState.Player[0].VelocityY += GameState.Player[0].AccelerationY;

		if (GameState.Player[0].PositionY < 0.0f)
		{
			GameState.Player[0].VelocityX = 0.0f;
			GameState.Player[0].VelocityY = 0.0f;
			GameState.Player[0].AccelerationY = 0.0f;
			GameState.Player[0].AccelerationY = 0.0f;
			GameState.Player[0].PositionY = 0.0f;

			GameState.Player[0].PlaybackState.Script = 0;
			GameState.Player[0].PlaybackState.PlaybackCursor = 0;
			Script[0] = ScriptHandler.GetScript(&GameState.Player[0].PlaybackState);
			State = Standing;
		}

		//LOG("Final script: %s\n", Script[0]->Name.c_str());
		//LOG("Script duration: %d\n", Script[0]->TotalFrames);
		//LOG("Cursor position: %d\n", GameState.Player[0].PlaybackState.PlaybackCursor);

		collision_box Pushbox[2];
		for (uint32 i = 0; i < 2; i++)
		{
			for (uint32 j = 0; j < Script[i]->Elements.PushboxCount; j++)
			{
				if (Script[i]->Elements.PushboxElements[j].InRange(GameState.Player[i].PlaybackState.PlaybackCursor))
				{
					if (GameState.Player[i].Facing > 0.0f)
					{
						Pushbox[i].X = Script[i]->Elements.PushboxElements[j].Box.X + GameState.Player[i].PositionX;
						Pushbox[i].Y = Script[i]->Elements.PushboxElements[j].Box.Y + GameState.Player[i].PositionY;
						Pushbox[i].Width = Script[i]->Elements.PushboxElements[j].Box.Width;
						Pushbox[i].Height = Script[i]->Elements.PushboxElements[j].Box.Height;
					}
					else
					{


						Pushbox[i].X = GameState.Player[i].PositionX - (Script[i]->Elements.PushboxElements[j].Box.X + Script[i]->Elements.PushboxElements[j].Box.Width);
						Pushbox[i].Y = Script[i]->Elements.PushboxElements[j].Box.Y + GameState.Player[i].PositionY;
						Pushbox[i].Width = Script[i]->Elements.PushboxElements[j].Box.Width;
						Pushbox[i].Height = Script[i]->Elements.PushboxElements[j].Box.Height;
					}
				}
			}
		}

		collision_box Result;
		if (BoxIntersection(&Pushbox[0], &Pushbox[1], &Result))
		{
			// Repulsion
			// TODO: Corner correction and edge cases
			GameState.Player[0].PositionX -= GameState.Player[0].Facing * (Result.Width / 2.0f);
			GameState.Player[1].PositionX -= GameState.Player[1].Facing * (Result.Width / 2.0f);

			//LOG("Pushbox collision, intersection amount: %f\n", Result.Width);
		}

		//LOG("[FRAME END %d]\n----------\n\n", GameState.FrameCount);
	}

	void DrawCollisionBoxes()
	{
		state_script* Script[2] =
		{
			ScriptHandler.GetScript(&GameState.Player[0].PlaybackState),
			ScriptHandler.GetScript(&GameState.Player[1].PlaybackState)
		};



		for (uint32 i = 0; i < 2; i++)
		{
			if (Script[i]->Elements.HurtboxCount)
			{
				sf::RectangleShape HurtboxRect;
				HurtboxRect.setFillColor(sf::Color(25, 220, 0, 120));
				HurtboxRect.setOutlineColor(sf::Color(10, 80, 0, 100));
				HurtboxRect.setOutlineThickness(-2.0f);

				for (uint32 j = 0; j < Script[i]->Elements.HurtboxCount; j++)
				{
					if (Script[i]->Elements.HurtboxElements[j].InRange(GameState.Player[i].PlaybackState.PlaybackCursor))
					{
						if (GameState.Player[i].Facing > 0.0f)
						{
							HurtboxRect.setPosition(sf::Vector2f(
								GameState.Player[i].PositionX + Script[i]->Elements.HurtboxElements[j].Box.X,
								-(GameState.Player[i].PositionY +
									Script[i]->Elements.HurtboxElements[j].Box.Y +
									RenderSystem->GetViewCenter().y +
									Script[i]->Elements.HurtboxElements[j].Box.Height)));
						}
						else
						{
							HurtboxRect.setPosition(sf::Vector2f(
								GameState.Player[i].PositionX - (Script[i]->Elements.HurtboxElements[j].Box.X + Script[i]->Elements.HurtboxElements[j].Box.Width),
								-(GameState.Player[i].PositionY +
									Script[i]->Elements.HurtboxElements[j].Box.Y +
									RenderSystem->GetViewCenter().y +
									Script[i]->Elements.HurtboxElements[j].Box.Height)));
						}

						HurtboxRect.setSize(sf::Vector2f(Script[i]->Elements.HurtboxElements[j].Box.Width, Script[i]->Elements.HurtboxElements[j].Box.Height));
						RenderSystem->Draw(HurtboxRect);
					}
				}
			}

			if (Script[i]->Elements.HitboxCount)
			{
				sf::RectangleShape HitboxRect;
				HitboxRect.setFillColor(sf::Color(220, 0, 0, 120));
				HitboxRect.setOutlineColor(sf::Color(140, 0, 0, 100));
				HitboxRect.setOutlineThickness(-2.0f);

				for (uint32 j = 0; j < Script[i]->Elements.HitboxCount; j++)
				{
					if (Script[i]->Elements.HitboxElements[j].InRange(GameState.Player[i].PlaybackState.PlaybackCursor))
					{
						if (GameState.Player[i].Facing > 0.0f)
						{
							HitboxRect.setPosition(sf::Vector2f(
								GameState.Player[i].PositionX + Script[i]->Elements.HitboxElements[j].Box.X,
								-(GameState.Player[i].PositionY +
									Script[i]->Elements.HitboxElements[j].Box.Y +
									RenderSystem->GetViewCenter().y +
									Script[i]->Elements.HitboxElements[j].Box.Height)));
						}
						else
						{
							HitboxRect.setPosition(sf::Vector2f(
								GameState.Player[i].PositionX - (Script[i]->Elements.HitboxElements[j].Box.X + Script[i]->Elements.HitboxElements[j].Box.Width),
								-(GameState.Player[i].PositionY +
									Script[i]->Elements.HitboxElements[j].Box.Y +
									RenderSystem->GetViewCenter().y +
									Script[i]->Elements.HitboxElements[j].Box.Height)));
						}
						HitboxRect.setSize(sf::Vector2f(Script[i]->Elements.HitboxElements[j].Box.Width, Script[i]->Elements.HitboxElements[j].Box.Height));
						RenderSystem->Draw(HitboxRect);
					}
				}
			}

			if (Script[i]->Elements.PushboxCount)
			{
				sf::RectangleShape PushboxRect;
				PushboxRect.setFillColor(sf::Color(200, 200, 0, 120));
				PushboxRect.setOutlineColor(sf::Color(120, 120, 0, 100));
				PushboxRect.setOutlineThickness(-2.0f);

				for (uint32 j = 0; j < Script[i]->Elements.PushboxCount; j++)
				{
					if (Script[i]->Elements.PushboxElements[j].InRange(GameState.Player[i].PlaybackState.PlaybackCursor))
					{
						if (GameState.Player[i].Facing > 0.0f)
						{
							PushboxRect.setPosition(sf::Vector2f(
								GameState.Player[i].PositionX + Script[i]->Elements.PushboxElements[j].Box.X,
								-(GameState.Player[i].PositionY +
									Script[i]->Elements.PushboxElements[j].Box.Y +
									RenderSystem->GetViewCenter().y +
									Script[i]->Elements.PushboxElements[j].Box.Height)));
						}
						else
						{
							PushboxRect.setPosition(sf::Vector2f(
								GameState.Player[i].PositionX - (Script[i]->Elements.PushboxElements[j].Box.X + Script[i]->Elements.PushboxElements[j].Box.Width),
								-(GameState.Player[i].PositionY +
									Script[i]->Elements.PushboxElements[j].Box.Y +
									RenderSystem->GetViewCenter().y +
									Script[i]->Elements.PushboxElements[j].Box.Height)));
						}
						PushboxRect.setSize(sf::Vector2f(Script[i]->Elements.PushboxElements[j].Box.Width, Script[i]->Elements.PushboxElements[j].Box.Height));
						RenderSystem->Draw(PushboxRect);
					}
				}
			}
		}
	}
}
