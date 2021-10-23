#include "taco_game.h"
#include "render_system.h"
#include "console_system.h"
#include "input_handler.h"
#include "gamestate_buffer.h"
#include "subsystems.h"
#include "gamestate.h"
#include "permanent_state.h"
#include "logging_system.h"
#include "state_manager.h"
#include "camera.h"

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
state_manager		StateManager;
camera*				Camera;

// TEST SHIT REMOVE

namespace taco
{
	void AdvanceFrame(uint32* Inputs);
	void Render();
	// TEST TINGS REMOVE AFTER IMPLEMENTATION
	void StepFrame();
	void ToggleFrameStepping();
	

	void Initialize(HINSTANCE Instance, HWND Window, system_event_queue* SystemEventQueue)
	{
		ConsoleSystem = new console_system();
		RenderSystem = new render_system(Window, Instance);
		LoggingSystem = new logging_system();
		InputHandler = new input_handler();
		EventQueue = new event_queue(SystemEventQueue, &StepFrame, &ToggleFrameStepping);
		GameStateBuffer = new gamestate_buffer(&GameState);
		Camera = new camera();

		FrameStepMode = false;
		DrawBoxes = false;
		DrawInputHistory = false;
		DrawStateInfo = false;

		GameState.Initialize();
		PermanentState.Player[0].Type = PLAYER_TYPE_LOCAL;
		PermanentState.Player[1].Type = PLAYER_TYPE_DUMMY;

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
		RenderSystem->Render();
		GameState.m_FrameCount++;
	}

	void Render()
	{
	
	}

	void StepFrame()
	{
		std::string DebugString = "Stepping frame: #" + std::to_string(GameState.m_FrameCount);


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

		}
		else
		{
			FrameStepMode = true;

		}
	}
}
