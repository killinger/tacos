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
#include "camera.h"
#include "input_buffer_graphics.h"

// TODO:
// - - - - -
// BUGS
// - - - - -
// - Input buffer treats changes to directions while a button is held as though the button was pressed on that frame
// - - Fixed it but the any restriction aint working too hot
// - Buffering moves consumes the input even if the move isn't performed, which means input leniency doesn't apply 

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
bool				DrawBoxes;
bool				DrawInputHistory;
bool				DrawStateInfo;

// Assets/other
player_graphics		PlayerGraphics[2];
state_manager		StateManager;
camera*				Camera;

// TEST SHIT REMOVE
sf::Texture BgTexture;
sf::Sprite	BgSprite;
input_buffer_graphics InputBufferGraphics;

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
		Camera = new camera();

		InputBufferGraphics.Initialize();
		BgTexture.loadFromFile("data/bg.png");
		BgSprite.setTexture(BgTexture, true);
		BgSprite.setPosition(-480.0f, -380.0f);

		FrameStepMode = false;
		DrawBoxes = false;
		DrawInputHistory = false;
		DrawStateInfo = false;

		GameState.Initialize();
		PermanentState.Player[0].Type = PLAYER_TYPE_LOCAL;
		PermanentState.Player[1].Type = PLAYER_TYPE_DUMMY;
		PlayerGraphics[0].Initialize("data/mad 3");
		PlayerGraphics[1].Initialize("data/sphere");
		//ScriptManager.initializeTestData();

		// TEST TINGS REMOVE AFTER IMPLEMENTATION
		StateManager.Initialize();

		ConsoleSystem->RegisterCVar("r_drawboxes", &DrawBoxes, "0", "1", "Draw collision boxes.", CVAR_BOOL);
		ConsoleSystem->RegisterCVar("r_drawinputs", &DrawInputHistory, "0", "1", "Draw input history.", CVAR_BOOL);
		ConsoleSystem->RegisterCVar("dbg_stateinfo", &DrawStateInfo, "0", "1", "Show player state info.", CVAR_BOOL);
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
		Camera->Update(&GameState);
		Render();
		GameState.m_FrameCount++;
	}

	void Render()
	{
		RenderSystem->Clear();

		state_script* Script = StateManager.GetScript(GameState.m_Player[0].PlaybackState.State);
		float OffsetX = 0.0f;
		float OffsetY = 0.0f;
		for (uint32 i = 0; i < Script->Elements.AnimationCount; i++)
		{
			if (Script->Elements.AnimationElements[i].InRange(GameState.m_Player[0].PlaybackState.PlaybackCursor))
			{
				PlayerGraphics->SetAnimation(Script->Elements.AnimationElements[i].Index);
				OffsetX = Script->Elements.AnimationElements[i].OffsetX;
				OffsetY = Script->Elements.AnimationElements[i].OffsetY;
			}
		}
		PlayerGraphics->SetPosition(GameState.m_Player[0].PositionX + OffsetX, GameState.m_Player[0].PositionY + OffsetY, GameState.m_Player[0].Facing);
		
		RenderSystem->Draw(PlayerGraphics->m_CharacterSprite, Camera);
		if (DrawBoxes)
			DrawCollisionBoxes();
		if (DrawInputHistory)
			InputBufferGraphics.Update(&GameState.m_Player[0].InputBuffer);
		if (DrawStateInfo)
		{
			DrawWorldText(0);
			DrawWorldText(1);
		}
		if (ConsoleSystem->m_IsActive)
			ConsoleSystem->DrawConsole();
	
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
						RenderSystem->Draw(HurtboxRect, Camera);
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
						RenderSystem->Draw(HitboxRect, Camera);
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
						RenderSystem->Draw(PushboxRect, Camera);
					}
				}
			}
		}
	}

	void DrawWorldText(uint32 Player)
	{
		state_script* Script = StateManager.GetScript(GameState.m_Player[Player].PlaybackState.State);

		float Offset = -200.0f;
		if (Player == 1)
		{
			Offset = 100.0f;
		}

		std::string BufferedState = "none";
		if (GameState.m_Player[Player].BufferedState.Flags & BUFFERED_STATE_ANY_CANCEL)
		{
			state_script* BufferedScript = StateManager.GetScript(GameState.m_Player[Player].BufferedState.StateIndex);
			BufferedState = BufferedScript->Name;
		}

		RenderSystem->DrawWorldText(
			Offset,
			-20.0f,
			"%s\n%d / %d\nPos %.2f, %.2f\nVel %.2f, %.2f\nAcc %.2f, %.2f\nHitstop %u\nBuffer %s",
			Script->Name.c_str(),
			GameState.m_Player[Player].PlaybackState.PlaybackCursor + 1, Script->TotalFrames,
			GameState.m_Player[Player].PositionX, GameState.m_Player[Player].PositionY,
			GameState.m_Player[Player].VelocityX, GameState.m_Player[Player].VelocityY,
			GameState.m_Player[Player].AccelerationX, GameState.m_Player[Player].AccelerationY,
			GameState.m_Player[Player].Hitstop,
			BufferedState.c_str(),
			(GameState.m_Player[Player].Flags & PLAYER_ALLOW_CANCEL), (GameState.m_Player[Player].Flags & PLAYER_USED_AIR_ACTION));
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
