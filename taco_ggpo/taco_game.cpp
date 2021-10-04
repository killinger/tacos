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
#include "linear_allocator.h"

// TODO:
// - - - - -
// BUGS
// - - - - -
// - Input buffer treats changes to directions while a button is held as though the button was pressed on that frame
// - - - - -
// OTHER
// - - - - - 
// - AtkLvl should be attached to hitboxes

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
bool				FrameStepMode;

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
	void Render();
	// TEST TINGS REMOVE AFTER IMPLEMENTATION
	void DrawCollisionBoxes();
	void DrawWorldText(uint32 Player);
	void StepFrame();
	void ToggleFrameStepping();
	

	void Initialize(sf::RenderWindow* Window, system_event_queue* SystemEventQueue)
	{
		ConsoleSystem = new console_system();
		RenderSystem = new render_system(Window);
		LoggingSystem = new logging_system();
		InputHandler = new input_handler();
		EventQueue = new event_queue(SystemEventQueue, &StepFrame, &ToggleFrameStepping);
		GameStateBuffer = new gamestate_buffer(&GameState);
		
		FrameStepMode = false;

		void* AllocatorStart = malloc(GAMESTATE_TRANSIENT_MEMORY_SIZE);
		memory_allocator* GameStateAllocator = new linear_allocator(AllocatorStart, GAMESTATE_TRANSIENT_MEMORY_SIZE);
		GameState.Initialize(GameStateAllocator);
		PermanentState.Player[0].Type = PLAYER_TYPE_LOCAL;
		PermanentState.Player[1].Type = PLAYER_TYPE_DUMMY;
		PlayerGraphics[0].Initialize("data/sphere");
		PlayerGraphics[1].Initialize("data/sphere");
		//ScriptManager.initializeTestData();

		// TEST TINGS REMOVE AFTER IMPLEMENTATION
		StateManager.Initialize();
	}

	void RunFrame()
	{
		EventQueue->ProcessSystemQueue();
		if (!FrameStepMode)
		{
			uint32 Inputs[2] = { 0 };
			for (uint32 i = 0; i < 2; i++)
			{
				if (PermanentState.Player[i].Type == PLAYER_TYPE_LOCAL)
					Inputs[i] = InputHandler->GetInputs();
			}

			// GGPO stuff here

			AdvanceFrame(Inputs);
		}
		else
			Render();
	}

	void AdvanceFrame(uint32* Inputs)
	{
		GameState.Update(Inputs, &StateManager);
		Render();
		GameState.m_FrameCount++;
	}

	void Render()
	{
		RenderSystem->Clear();
		DrawCollisionBoxes();
		DrawWorldText(0);
		DrawWorldText(1);
		if (ConsoleSystem->m_IsActive)
			ConsoleSystem->DrawConsole();
		else
			RenderSystem->DrawDebugString();
		RenderSystem->Display();
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

	void DrawWorldText(uint32 Player)
	{
		state_script* Script = StateManager.GetScript(GameState.m_Player[Player].PlaybackState.State);

		float Adjustment = 80.0f;
		if (GameState.m_Player[Player].Facing == -1.0f)
			Adjustment = 4.0f;

		std::string BufferedState = "None";
		if (GameState.m_Player[Player].PlaybackState.BufferedState != -1)
		{
			state_script* BufferedScript = StateManager.GetScript(GameState.m_Player[Player].PlaybackState.BufferedState);
			BufferedState = BufferedScript->Name;
		}

		RenderSystem->DrawWorldText(
			(-120.0f * GameState.m_Player[Player].Facing) - Adjustment,
			RenderSystem->GetViewCenter().y - 140.0f,
			"%s\n%d / %d\nPos %.2f, %.2f\nVel %.2f, %.2f\nAcc %.2f, %.2f\nHitstop %u\nBuffer: %s",
			Script->Name.c_str(),
			GameState.m_Player[Player].PlaybackState.PlaybackCursor + 1, Script->TotalFrames,
			GameState.m_Player[Player].PositionX, GameState.m_Player[Player].PositionY,
			GameState.m_Player[Player].VelocityX, GameState.m_Player[Player].VelocityY,
			GameState.m_Player[Player].AccelerationX, GameState.m_Player[Player].AccelerationY,
			GameState.m_Player[Player].Hitstop,
			BufferedState.c_str());
	}
	
	void StepFrame()
	{
		std::string DebugString = "Stepping frame: #" + std::to_string(GameState.m_FrameCount);
		RenderSystem->SetDebugString(DebugString.c_str());

		uint32 Inputs[2] = { 0 };
		for (uint32 i = 0; i < 2; i++)
		{
			if (PermanentState.Player[i].Type == PLAYER_TYPE_LOCAL)
				Inputs[i] = InputHandler->GetInputs();
		}

		AdvanceFrame(Inputs);
	}

	void ToggleFrameStepping()
	{
		if (FrameStepMode)
		{
			FrameStepMode = false;
			RenderSystem->SetDebugString("Frame step mode: off");
		}
		else
		{
			FrameStepMode = true;
			RenderSystem->SetDebugString("Frame step mode: on");
		}
	}
}
