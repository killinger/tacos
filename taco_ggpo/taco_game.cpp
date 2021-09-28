#include "taco_game.h"
#include "render_system.h"
#include "console_system.h"
#include "input_handler.h"
#include "gamestate_buffer.h"
#include "subsystems.h"
#include "gamestate.h"
#include "permanent_state.h"
#include "player_graphics.h"
#include "logging_system.h"
#include "state_manager.h"

// TODO: This should find a more suitable home
#define PLAYER_STARTING_POSITIONS 80.0f

// Subsystems
console_system*		ConsoleSystem;
render_system*		RenderSystem;
logging_system*		LoggingSystem;
input_handler*		InputHandler;
event_queue*		EventQueue;
gamestate_buffer*	GameStateBuffer;

// State
gamestate			GameState = { 0 };
permanent_state		PermanentState;

// Assets/other
player_graphics		PlayerGraphics[2];
state_manager		StateManager;

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
	void DrawCollisionBoxes();

	void Initialize(sf::RenderWindow* Window, system_event_queue* SystemEventQueue)
	{
		ConsoleSystem = new console_system();
		RenderSystem = new render_system(Window);
		LoggingSystem = new logging_system();
		InputHandler = new input_handler();
		EventQueue = new event_queue(SystemEventQueue);
		GameStateBuffer = new gamestate_buffer(&GameState);

		GameState.m_Player[0].PositionX = -PLAYER_STARTING_POSITIONS;
		GameState.m_Player[0].Facing = 1.0f;
		GameState.m_Player[1].PositionX = PLAYER_STARTING_POSITIONS;
		GameState.m_Player[1].Facing = -1.0f;

		PermanentState.Player[0].Type = PLAYER_TYPE_LOCAL;
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
	}

	/* TODO:
	RunFrame should pass inputs OK:d by GGPO to AdvanceFrame. AdvanceFrame (and whatever will handling updating gamestate) should only have to care about
	what inputs belong to which player,	not the details of those players (local/remote/dummy).
	*/
	void RunFrame()
	{
		EventQueue->ProcessSystemQueue();

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
		GameState.Update(Inputs, &StateManager);

		RenderSystem->Clear();
		DrawCollisionBoxes();
		if (ConsoleSystem->m_IsActive)
			ConsoleSystem->DrawConsole();
		else
			RenderSystem->DrawDebugString();
		RenderSystem->Display();

		GameState.m_FrameCount++;
	}

	void DrawCollisionBoxes()
	{
		state_script* Script[2] =
		{
			StateManager.GetScript(GameState.m_Player[0].PlaybackState.State),
			StateManager.GetScript(GameState.m_Player[1].PlaybackState.State)
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
