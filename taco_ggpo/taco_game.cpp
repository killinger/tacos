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
// TEST TINGS REMOVE AFTER IMPLEMENTATION
#include "script_handler.h"

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

namespace taco
{
	void AdvanceFrame(uint32* Inputs);

	// TEST TINGS REMOVE AFTER IMPLEMENTATION
	void UpdateTest(uint32* Inputs);
	void DrawCollisionBoxes();

	void Initialize(sf::RenderWindow* Window)
	{
		ConsoleSystem = new console_system();
		RenderSystem = new render_system(Window);
		InputHandler = new input_handler();

		GameState.Initialize();
		GameState.Player[0].PositionX = -PLAYER_STARTING_POSITIONS;
		GameState.Player[0].Facing = 1.0f;
		GameState.Player[1].PositionX = PLAYER_STARTING_POSITIONS;
		GameState.Player[1].Facing = -1.0f;

		PermanentState.Player[0].Type = PLAYER_TYPE_LOCAL;
		PermanentState.Player[0].InputBuffer.Initialize();
		
		PermanentState.Player[1].Type = PLAYER_TYPE_DUMMY;
		PermanentState.Player[1].InputBuffer.Initialize();


		PlayerGraphics[0].Initialize("data/sphere");
		PlayerGraphics[1].Initialize("data/sphere");
		//ScriptManager.initializeTestData();
		DebugOutput.OutputMode = DEBUG_DRAW_NONE;

		ConsoleSystem->RegisterCVar("debug", &DebugOutput.OutputMode, "0", "1", "Display debug string\n0: none\n1: script playback", CVAR_INT);
		ConsoleSystem->RegisterCVar("script", &GameState.Player[0].PlaybackState.Script, "0", "3", "Set script", CVAR_INT);

		// TEST TINGS REMOVE AFTER IMPLEMENTATION
		ScriptHandler.Initialize();
		GameState.Player[0].PlaybackState.Script = 0;
		GameState.Player[0].PlaybackState.PendingScript = 0;
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
		UpdateTest(Inputs);

		//RenderSystem->Clear();
		//// TODO: What's the draw order rules? Last character to land a hit on top?
		////RenderSystem->Draw(PlayerGraphics[0].m_CharacterSprite);
		////RenderSystem->Draw(PlayerGraphics[1].m_CharacterSprite);
		//for (uint32 i = 0; i < 2; i++)
		//{
		//	if (Hurtboxes[i] != NULL)
		//		DrawHurtboxes(i);
		//	if (Hitboxes[i] != NULL)
		//		DrawHitboxes(i);
		//}
		//if (ConsoleSystem->m_IsActive)
		//	ConsoleSystem->DrawConsole();
		//else if (DebugOutput.OutputMode != DEBUG_DRAW_NONE)
		//	RenderSystem->DrawDebugString(DebugOutput.String.c_str());
		//RenderSystem->DrawLine(	GameState.PlayerState[0].PositionX, -80.0f, 
		//						GameState.PlayerState[0].PositionX, 80.0f);
		//RenderSystem->DrawLine(	GameState.PlayerState[1].PositionX, -80.0f, 
		//						GameState.PlayerState[1].PositionX, 80.0f);
		//RenderSystem->Display();

		GameState.FrameCount++;
	}

	void UpdateTest(uint32* Inputs)
	{
		PermanentState.Player[0].InputBuffer.Update(Inputs[0], GameState.Player[0].Facing);
		PermanentState.Player[1].InputBuffer.Update(Inputs[1], GameState.Player[1].Facing);

		state_script* Script[2] =
		{
			ScriptHandler.GetScript(&GameState.Player[0].PlaybackState),
			ScriptHandler.GetScript(&GameState.Player[1].PlaybackState)
		};

		cancel_list* CancelList = NULL;
		int32 NextScript = -1;
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
				Move = ScriptHandler.GetMove(CancelList->Moves[i]);
				if ((PermanentState.Player[0].InputBuffer.m_InputStates[0].DirectionState.Direction & Move->InputMask) == Move->InputMask)
				{
					NextScript = Move->ScriptIndex;
					break;
				}
			}
		}

		if (NextScript == -1)
		{
			if ((Script[0]->Flags & SCRIPT_AIRBORNE))
			{
				NextScript = 1;

				if (GameState.Player[0].PositionY < 0.0f)
				{
					GameState.Player[0].VelocityX = 0.0f;
					GameState.Player[0].VelocityY = 0.0f;
					GameState.Player[0].AccelerationY = 0.0f;
					GameState.Player[0].AccelerationY = 0.0f;
					GameState.Player[0].PositionY = 0.0f;

					NextScript = 0;
				}
			}
			else
			{
				NextScript = 0;
			}
		}

		int32 OldScript = GameState.Player[0].PlaybackState.Script;
		GameState.Player[0].PlaybackState.Script = NextScript;
		Script[0] = ScriptHandler.GetScript(&GameState.Player[0].PlaybackState);

		if (NextScript == OldScript)
		{
			GameState.Player[0].PlaybackState.PlaybackCursor++;
			if (GameState.Player[0].PlaybackState.PlaybackCursor >= Script[0]->TotalFrames - 1)
				GameState.Player[0].PlaybackState.PlaybackCursor = 0;
		}
		else
		{
			GameState.Player[0].PlaybackState.PlaybackCursor = 0;
			GameState.Player[0].VelocityX *= Script[0]->ScalingXV;
			GameState.Player[0].VelocityY *= Script[0]->ScalingYV;
			GameState.Player[0].AccelerationX *= Script[0]->ScalingXA;
			GameState.Player[0].AccelerationY *= Script[0]->ScalingYA;
		}

		// TODO: This condition aint ok
		//if (GameState.PlayerState[0].PlaybackState.PendingScript != GameState.PlayerState[0].PlaybackState.Script)
		//{
		//	GameState.PlayerState[0].VelocityX *= Script[0]->ScalingXV;
		//	GameState.PlayerState[0].VelocityY *= Script[0]->ScalingYV;
		//	GameState.PlayerState[0].AccelerationX *= Script[0]->ScalingXA;
		//	GameState.PlayerState[0].AccelerationY *= Script[0]->ScalingYA;
		//}

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
		RenderSystem->Clear();
		DrawCollisionBoxes();
		RenderSystem->DrawDebugString(Script[0]->Name.c_str());
		if (ConsoleSystem->m_IsActive)
			ConsoleSystem->DrawConsole();

		RenderSystem->Display();
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
