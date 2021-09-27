#include "taco_game.h"
#include "render_system.h"
#include "console_system.h"
#include "input_handler.h"
#include "subsystems.h"
#include "gamestate.h"
#include "permanent_state.h"
#include "player_graphics.h"
#include "logging_system.h"
// TEST TINGS REMOVE AFTER IMPLEMENTATION
#include "state_manager.h"

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
// TODO: Is there a way to do BUTTON_RESTRICTION_ANY with fewer branches?

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

// TEST TINGS REMOVE AFTER IMPLEMENTATION
state_manager		StateManager;
input_buffer		InputBuffer;
move_description	TestMove;

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
	void UpdateTest(uint32* Inputs);
	void DrawCollisionBoxes();

	void Initialize(sf::RenderWindow* Window)
	{
		ConsoleSystem = new console_system();
		RenderSystem = new render_system(Window);
		LoggingSystem = new logging_system();
		InputHandler = new input_handler();

		GameState.m_Player[0].PositionX = -PLAYER_STARTING_POSITIONS;
		GameState.m_Player[0].Facing = 1.0f;
		GameState.m_Player[1].PositionX = PLAYER_STARTING_POSITIONS;
		GameState.m_Player[1].Facing = -1.0f;

		PermanentState.Player[0].Type = PLAYER_TYPE_LOCAL;
		//PermanentState.Player[0].WalkspeedForward = 1.34f;
		//PermanentState.Player[0].JumpGravity = -0.21f;
		//PermanentState.Player[0].JumpInitialVelocity = 8.2f;
		//PermanentState.Player[0].JumpForwardVelocity = 0.36f;

		PermanentState.Player[1].Type = PLAYER_TYPE_DUMMY;


		PlayerGraphics[0].Initialize("data/sphere");
		PlayerGraphics[1].Initialize("data/sphere");
		//ScriptManager.initializeTestData();

		// TEST TINGS REMOVE AFTER IMPLEMENTATION
		StateManager.Initialize();
		GameState.m_Player[0].PlaybackState.State = 0;
		GameState.m_Player[0].PlaybackState.New = true;
		GameState.m_Player[1].PlaybackState.State = 0;
		GameState.m_Player[1].PlaybackState.New = true;

		TestMove.m_Input.m_InputMask = INPUT_B | INPUT_DOWN;
		TestMove.m_Input.m_PropertyFlags = BUTTON_RESTRICTION_ANY | DIRECTION_RESTRICTION_SIMILAR;
		TestMove.m_MotionCount = 3;
		TestMove.m_Motion[0].m_BufferFrames = 8;
		TestMove.m_Motion[0].m_Input.m_InputMask = INPUT_FORWARD;
		TestMove.m_Motion[0].m_Input.m_PropertyFlags = DIRECTION_RESTRICTION_EXACT;
		TestMove.m_Motion[1].m_BufferFrames = 8;
		TestMove.m_Motion[1].m_Input.m_InputMask = INPUT_DOWN;
		TestMove.m_Motion[1].m_Input.m_PropertyFlags = DIRECTION_RESTRICTION_EXACT;
		TestMove.m_Motion[2].m_BufferFrames = 8;
		TestMove.m_Motion[2].m_Input.m_InputMask = INPUT_FORWARD;
		TestMove.m_Motion[2].m_Input.m_PropertyFlags = DIRECTION_RESTRICTION_EXACT;

		ReplayData.SetInputAtFrame(INPUT_A | INPUT_B | INPUT_BACK, 0);
	}

	/* TODO:
	RunFrame should pass inputs OK:d by GGPO to AdvanceFrame. AdvanceFrame (and whatever will handling updating gamestate) should only have to care about
	what inputs belong to which player,	not the details of those players (local/remote/dummy).
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
		GameState.Update(Inputs, &StateManager);

		RenderSystem->Clear();
		DrawCollisionBoxes();
		
		state_script* Script = StateManager.GetScript(GameState.m_Player[0].PlaybackState.State);
		uint32 Cursor = GameState.m_Player[0].PlaybackState.PlaybackCursor + 1;
		std::string DebugString("%d", GameState.m_FrameCount);
		char Buffer[256];
		sprintf_s(Buffer, 256, "%d\n%s\n%d / %d", GameState.m_FrameCount, Script->Name.c_str(), Cursor, Script->TotalFrames);
		RenderSystem->DrawDebugString(Buffer);

		//if (ConsoleSystem->m_IsActive)
		//	ConsoleSystem->DrawConsole();
		RenderSystem->Display();

		GameState.m_FrameCount++;
	}

	
	void UpdateTest(uint32* Inputs)
	{
		//LOG("[FRAME START %d]\n", GameState.FrameCount);

		GameState.m_Player[0].InputBuffer.Update(	Inputs[0], 
												GameState.m_FrameCount, 
												(GameState.m_Player[0].PositionX > GameState.m_Player[1].PositionX));
		GameState.m_Player[1].InputBuffer.Update(	Inputs[1],
												GameState.m_FrameCount, 
												(GameState.m_Player[1].PositionX > GameState.m_Player[0].PositionX));

		bool FlipDirections = (GameState.m_Player[0].PositionX > GameState.m_Player[1].PositionX);
		InputBuffer.Update(ReplayData.GetNextInput(0), 0, FlipDirections);
		InputBuffer.MatchInputs(&TestMove, 0, 3);

		state_script* Script[2];
		Script[1] = StateManager.GetScript(&GameState.m_Player[1].PlaybackState);
		
		GameState.m_Player[0].PositionX += GameState.m_Player[0].VelocityX;
		GameState.m_Player[0].PositionY += GameState.m_Player[0].VelocityY;
		GameState.m_Player[0].VelocityX += GameState.m_Player[0].AccelerationX;
		GameState.m_Player[0].VelocityY += GameState.m_Player[0].AccelerationY;

		if (GameState.m_Player[0].PositionY < 0.0f)
		{
			GameState.m_Player[0].VelocityX = 0.0f;
			GameState.m_Player[0].VelocityY = 0.0f;
			GameState.m_Player[0].AccelerationY = 0.0f;
			GameState.m_Player[0].AccelerationY = 0.0f;
			GameState.m_Player[0].PositionY = 0.0f;

			GameState.m_Player[0].PlaybackState.State = 0;
			GameState.m_Player[0].PlaybackState.PlaybackCursor = 0;
			Script[0] = StateManager.GetScript(&GameState.m_Player[0].PlaybackState);
		}

		//LOG("Final script: %s\n", Script[0]->Name.c_str());
		//LOG("Script duration: %d\n", Script[0]->TotalFrames);
		//LOG("Cursor position: %d\n", GameState.Player[0].PlaybackState.PlaybackCursor);

		collision_box Pushbox[2];
		for (uint32 i = 0; i < 2; i++)
		{
			for (uint32 j = 0; j < Script[i]->Elements.PushboxCount; j++)
			{
				if (Script[i]->Elements.PushboxElements[j].InRange(GameState.m_Player[i].PlaybackState.PlaybackCursor))
				{
					if (GameState.m_Player[i].Facing > 0.0f)
					{
						Pushbox[i].X = Script[i]->Elements.PushboxElements[j].Box.X + GameState.m_Player[i].PositionX;
						Pushbox[i].Y = Script[i]->Elements.PushboxElements[j].Box.Y + GameState.m_Player[i].PositionY;
						Pushbox[i].Width = Script[i]->Elements.PushboxElements[j].Box.Width;
						Pushbox[i].Height = Script[i]->Elements.PushboxElements[j].Box.Height;
					}
					else
					{


						Pushbox[i].X = GameState.m_Player[i].PositionX - (Script[i]->Elements.PushboxElements[j].Box.X + Script[i]->Elements.PushboxElements[j].Box.Width);
						Pushbox[i].Y = Script[i]->Elements.PushboxElements[j].Box.Y + GameState.m_Player[i].PositionY;
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
			GameState.m_Player[0].PositionX -= GameState.m_Player[0].Facing * (Result.Width / 2.0f);
			GameState.m_Player[1].PositionX -= GameState.m_Player[1].Facing * (Result.Width / 2.0f);

			//LOG("Pushbox collision, intersection amount: %f\n", Result.Width);
		}

		//LOG("[FRAME END %d]\n----------\n\n", GameState.FrameCount);
	}

	void DrawCollisionBoxes()
	{
		state_script* Script[2] =
		{
			StateManager.GetScript(&GameState.m_Player[0].PlaybackState),
			StateManager.GetScript(&GameState.m_Player[1].PlaybackState)
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
					if (Script[i]->Elements.HurtboxElements[j].InRange(GameState.m_Player[i].PlaybackState.PlaybackCursor))
					{
						if (GameState.m_Player[i].Facing > 0.0f)
						{
							HurtboxRect.setPosition(sf::Vector2f(
								GameState.m_Player[i].PositionX + Script[i]->Elements.HurtboxElements[j].Box.X,
								-(GameState.m_Player[i].PositionY +
									Script[i]->Elements.HurtboxElements[j].Box.Y +
									RenderSystem->GetViewCenter().y +
									Script[i]->Elements.HurtboxElements[j].Box.Height)));
						}
						else
						{
							HurtboxRect.setPosition(sf::Vector2f(
								GameState.m_Player[i].PositionX - (Script[i]->Elements.HurtboxElements[j].Box.X + Script[i]->Elements.HurtboxElements[j].Box.Width),
								-(GameState.m_Player[i].PositionY +
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
					if (Script[i]->Elements.HitboxElements[j].InRange(GameState.m_Player[i].PlaybackState.PlaybackCursor))
					{
						if (GameState.m_Player[i].Facing > 0.0f)
						{
							HitboxRect.setPosition(sf::Vector2f(
								GameState.m_Player[i].PositionX + Script[i]->Elements.HitboxElements[j].Box.X,
								-(GameState.m_Player[i].PositionY +
									Script[i]->Elements.HitboxElements[j].Box.Y +
									RenderSystem->GetViewCenter().y +
									Script[i]->Elements.HitboxElements[j].Box.Height)));
						}
						else
						{
							HitboxRect.setPosition(sf::Vector2f(
								GameState.m_Player[i].PositionX - (Script[i]->Elements.HitboxElements[j].Box.X + Script[i]->Elements.HitboxElements[j].Box.Width),
								-(GameState.m_Player[i].PositionY +
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
					if (Script[i]->Elements.PushboxElements[j].InRange(GameState.m_Player[i].PlaybackState.PlaybackCursor))
					{
						if (GameState.m_Player[i].Facing > 0.0f)
						{
							PushboxRect.setPosition(sf::Vector2f(
								GameState.m_Player[i].PositionX + Script[i]->Elements.PushboxElements[j].Box.X,
								-(GameState.m_Player[i].PositionY +
									Script[i]->Elements.PushboxElements[j].Box.Y +
									RenderSystem->GetViewCenter().y +
									Script[i]->Elements.PushboxElements[j].Box.Height)));
						}
						else
						{
							PushboxRect.setPosition(sf::Vector2f(
								GameState.m_Player[i].PositionX - (Script[i]->Elements.PushboxElements[j].Box.X + Script[i]->Elements.PushboxElements[j].Box.Width),
								-(GameState.m_Player[i].PositionY +
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
