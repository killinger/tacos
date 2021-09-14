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
// TODO: Commands need to be finished, a flag for any similar direction among other things (else walking forward to button won't work etc) 
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
console_system*	ConsoleSystem;
render_system*	RenderSystem;
input_handler*	InputHandler;

// State
gamestate		GameState = { 0 };
permanent_state PermanentState;

// Assets/other
player_graphics	PlayerGraphics[2];
script_manager	ScriptManager;
debug_output	DebugOutput;

// TEST TINGS REMOVE AFTER IMPLEMENTATION
hurtbox_info*	Hurtboxes[2];
hitbox_info*	Hitboxes[2];
bool			Collision = false;
script_handler	ScriptHandler;

namespace taco
{
	void AdvanceFrame(uint32* Inputs);
	void UpdatePlayerState(uint32* Inputs);

	// TEST TINGS REMOVE AFTER IMPLEMENTATION
	void UpdateTest(uint32* Inputs);
	void DrawCollisionBoxes();

	void Initialize(sf::RenderWindow* Window)
	{
		ConsoleSystem = new console_system();
		RenderSystem = new render_system(Window);
		InputHandler = new input_handler();

		GameState.Initialize();
		GameState.PlayerState[0].PositionX = -PLAYER_STARTING_POSITIONS;
		GameState.PlayerState[0].Facing = 1.0f;
		GameState.PlayerState[1].PositionX = PLAYER_STARTING_POSITIONS;
		GameState.PlayerState[1].Facing = -1.0f;

		PermanentState.Players[0].Type = PLAYER_TYPE_LOCAL;
		PermanentState.Players[1].Type = PLAYER_TYPE_DUMMY;

		PlayerGraphics[0].Initialize("data/sphere");
		PlayerGraphics[1].Initialize("data/sphere");
		//ScriptManager.initializeTestData();
		DebugOutput.OutputMode = DEBUG_DRAW_NONE;

		ConsoleSystem->RegisterCVar("debug", &DebugOutput.OutputMode, "0", "1", "Display debug string\n0: none\n1: script playback", CVAR_INT);

		// TEST TINGS REMOVE AFTER IMPLEMENTATION
		ScriptHandler.Initialize();
		GameState.PlayerState[0].PlaybackState.Script = 1;
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
			if (PermanentState.Players[i].Type == PLAYER_TYPE_LOCAL)
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

		if (GameState.PlayerState[0].PositionX < GameState.PlayerState[1].PositionX)
		{
			GameState.PlayerState[0].Facing = 1.0f;
			GameState.PlayerState[1].Facing = -1.0f;
		}
		else
		{
			GameState.PlayerState[0].Facing = -1.0f;
			GameState.PlayerState[1].Facing = 1.0f;
		}

		// TODO: Collision detection. It's ok for collision detection to be ineffective as there aren't going to be all that many checks per frame even in the worst case
		Collision = false;
		if (Hitboxes[0] != NULL &&
			Hurtboxes[1] != NULL)
		{
			sf::FloatRect Boxes[2] = 
			{ 
				{ Hitboxes[0]->x, Hitboxes[0]->x, Hitboxes[0]->width, Hitboxes[0]->height }, 
				{ Hurtboxes[1]->x, Hurtboxes[1]->y, Hurtboxes[1]->width, Hurtboxes[1]->height }
			};

			Boxes[0] = PlayerGraphics[0].m_CharacterSprite.getTransform().transformRect(Boxes[0]);
			Boxes[1] = PlayerGraphics[1].m_CharacterSprite.getTransform().transformRect(Boxes[1]);

			if (Boxes[0].intersects(Boxes[1]))
			{
				Collision = true;
			}
		}

		DebugOutput.Update(Scripts[0], &GameState);

		for (uint32 j = 0; j < 2; j++)
		{
			// TODO: There needs to be quite a bit of rules for changing position
			GameState.PlayerState[j].PositionX += (Frames[j]->m_xShift * GameState.PlayerState[j].Facing);
			if (Frames[j]->m_ApplyRun)
			{
				GameState.PlayerState[j].AccelerationX = 0.5f;
				GameState.PlayerState[j].VelocityX += GameState.PlayerState[j].AccelerationX;
				if (GameState.PlayerState[j].VelocityX >= 5.0f)
					GameState.PlayerState[j].VelocityX = 5.0f;
			}
			else if (GameState.PlayerState[j].VelocityX > 0.0f)
			{
				GameState.PlayerState[j].VelocityX -= 0.2f;
				if (GameState.PlayerState[j].VelocityX < 0.0f)
					GameState.PlayerState[j].VelocityX = 0.0f;
			}
			GameState.PlayerState[j].PositionX += (GameState.PlayerState[j].VelocityX * GameState.PlayerState[j].Facing);

			GameState.PlayerState[j].InputBuffer.Update(Inputs[j], GameState.PlayerState[j].Facing);
			std::string Trigger = "";

			// TODO: Add another one of those script managers
			// TODO: There needs to be additional checks, such as meter requirement/availability 
			for (uint32 i = 0; i < ScriptManager.m_Commands.size(); i++)
			{
				if (GameState.PlayerState[j].InputBuffer.CheckForCommand(&ScriptManager.m_Commands[i]))
				{
					Trigger = ScriptManager.m_Commands[i].Trigger;
					break;
				}
			}

			// TODO: -1 should be a define, not magic number. also should never happen
			if (Frames[j]->m_AnimationIndex != -1)
			{
				PlayerGraphics[j].SetAnimation(Frames[j]->m_AnimationIndex, GameState.PlayerState[j].Facing);
			}
			PlayerGraphics[j].SetPosition(
				GameState.PlayerState[j].PositionX, 
				GameState.PlayerState[j].PositionY, 
				GameState.PlayerState[j].Facing);

			// TODO: this be test shit
			if (Frames[j]->m_Hurtboxes != -1)
			{
				Hurtboxes[j] = &ScriptManager.m_hurtboxes[Frames[j]->m_Hurtboxes];
			}
			else
			{
				Hurtboxes[j] = NULL;
			}
			
			if (Frames[j]->m_Hitboxes != -1)
			{
				Hitboxes[j] = &ScriptManager.m_hitboxes[Frames[j]->m_Hitboxes];
			}
			else
			{
				Hitboxes[j] = NULL;
			}

			// TODO: At this stage, is there a point to having a pending script if updating the playback state is the last operation? 
			// Is there ever anything you'd want to do between updating the playback state and committing to switching script?
			ScriptManager.Update(&GameState.PlayerState[j].PlaybackState, Trigger);
			GameState.PlayerState[j].PlaybackState.Script = GameState.PlayerState[j].PlaybackState.PendingScript;
		}
	}
	
	void UpdateTest(uint32* Inputs)
	{
		// TODO: What is the actual order of operations supposed to be?
		// in sfv collisions aren't resolved until the next frame ie a character will start reeling on frame 4 after being hit by a 3f button
		// * collisions after movement/shifts seems obvious - else the movement would lag behind a frame + repulsion would be shit
		// * update script playback before movement? otherwise forces would bleed over for a frame on script transition i think?
		//		^ not if V/A scaling is applied immediately after transition
		//		^ when the heck does friction get applied then? probably with the rest of the movement.
		//			^ whenever friction is set accel must be set to 0?
		// * but script update depends on collisions.. ay caramba
		// * if updating script playback goes first the first frame would be skipped  

		state_script* Script[2] = 
		{	
			ScriptHandler.GetScript(&GameState.PlayerState[0].PlaybackState),
			ScriptHandler.GetScript(&GameState.PlayerState[1].PlaybackState)
		};

		GameState.PlayerState[0].PositionX += GameState.PlayerState[0].VelocityX;
		GameState.PlayerState[0].PositionY += GameState.PlayerState[0].VelocityY;
		GameState.PlayerState[0].VelocityX += GameState.PlayerState[0].AccelerationX;
		GameState.PlayerState[0].VelocityY += GameState.PlayerState[0].AccelerationY;

		// TODO: This condition aint ok
		//if (GameState.PlayerState[0].PlaybackState.PendingScript != GameState.PlayerState[0].PlaybackState.Script)
		//{
		//	GameState.PlayerState[0].VelocityX *= Script[0]->ScalingXV;
		//	GameState.PlayerState[0].VelocityY *= Script[0]->ScalingYV;
		//	GameState.PlayerState[0].AccelerationX *= Script[0]->ScalingXA;
		//	GameState.PlayerState[0].AccelerationY *= Script[0]->ScalingYA;
		//}

		GameState.PlayerState[0].InputBuffer.Update(Inputs[0], GameState.PlayerState[0].Facing);
		GameState.PlayerState[1].InputBuffer.Update(Inputs[1], GameState.PlayerState[1].Facing);

		ScriptHandler.Update(&GameState.PlayerState[0].PlaybackState);
		ScriptHandler.Update(&GameState.PlayerState[1].PlaybackState);

		DrawCollisionBoxes();

	}
	
	void DrawCollisionBoxes()
	{
		state_script* Script[2] =
		{
			ScriptHandler.GetScript(&GameState.PlayerState[0].PlaybackState),
			ScriptHandler.GetScript(&GameState.PlayerState[1].PlaybackState)
		};

		RenderSystem->Clear();

		for (uint32 i = 0; i < 2; i++)
		{
			if (Script[i]->Elements.HurtboxCount)
			{
				sf::RectangleShape HurtboxRect;
				HurtboxRect.setFillColor(sf::Color(25, 220, 0, 180));
				HurtboxRect.setOutlineColor(sf::Color(10, 80, 0, 160));
				HurtboxRect.setOutlineThickness(-2.0f);

				for (uint32 j = 0; j < Script[i]->Elements.HurtboxCount; j++)
				{
					if (Script[i]->Elements.HurtboxElements[j].InRange(GameState.PlayerState[i].PlaybackState.PlaybackCursor))
					{
						HurtboxRect.setPosition(sf::Vector2f(
							GameState.PlayerState[i].PositionX + Script[i]->Elements.HurtboxElements[j].Box.X,
							-(GameState.PlayerState[i].PositionY +
								Script[i]->Elements.HurtboxElements[j].Box.Y +
								RenderSystem->GetViewCenter().y +
								Script[i]->Elements.HurtboxElements[j].Box.Height)));
						HurtboxRect.setSize(sf::Vector2f(Script[i]->Elements.HurtboxElements[j].Box.Width, Script[i]->Elements.HurtboxElements[j].Box.Height));
						RenderSystem->Draw(HurtboxRect);
					}
				}
			}

			if (Script[i]->Elements.HitboxCount)
			{
				sf::RectangleShape HitboxRect;
				HitboxRect.setFillColor(sf::Color(220, 0, 0, 180));
				HitboxRect.setOutlineColor(sf::Color(140, 0, 0, 160));
				HitboxRect.setOutlineThickness(-2.0f);

				for (uint32 j = 0; j < Script[i]->Elements.HitboxCount; j++)
				{
					if (Script[i]->Elements.HitboxElements[j].InRange(GameState.PlayerState[i].PlaybackState.PlaybackCursor))
					{
						HitboxRect.setPosition(sf::Vector2f(
							GameState.PlayerState[i].PositionX + Script[i]->Elements.HitboxElements[j].Box.X,
							-(GameState.PlayerState[i].PositionY +
								Script[i]->Elements.HitboxElements[j].Box.Y +
								RenderSystem->GetViewCenter().y +
								Script[i]->Elements.HitboxElements[j].Box.Height)));
						HitboxRect.setSize(sf::Vector2f(Script[i]->Elements.HitboxElements[j].Box.Width, Script[i]->Elements.HitboxElements[j].Box.Height));
						RenderSystem->Draw(HitboxRect);
					}
				}
			}

			if (Script[i]->Elements.PushboxCount)
			{
				sf::RectangleShape PushboxRect;
				PushboxRect.setFillColor(sf::Color(200, 200, 0, 180));
				PushboxRect.setOutlineColor(sf::Color(120, 120, 0, 160));
				PushboxRect.setOutlineThickness(-2.0f);

				for (uint32 j = 0; j < Script[i]->Elements.PushboxCount; j++)
				{
					if (Script[i]->Elements.PushboxElements[j].InRange(GameState.PlayerState[i].PlaybackState.PlaybackCursor))
					{
						PushboxRect.setPosition(sf::Vector2f(
							GameState.PlayerState[i].PositionX + Script[i]->Elements.PushboxElements[j].Box.X,
							-(GameState.PlayerState[i].PositionY +
								Script[i]->Elements.PushboxElements[j].Box.Y +
								RenderSystem->GetViewCenter().y +
								Script[i]->Elements.PushboxElements[j].Box.Height)));
						PushboxRect.setSize(sf::Vector2f(Script[i]->Elements.PushboxElements[j].Box.Width, Script[i]->Elements.PushboxElements[j].Box.Height));
						RenderSystem->Draw(PushboxRect);
					}
				}
			}
		}

		RenderSystem->Display();
	}
}