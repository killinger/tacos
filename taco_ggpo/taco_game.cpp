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
#include "debug_overlay.h"
#include "debug_draw_manager.h"
#include "scene_graph.h"
#include "auto_profiler.h"


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
overlay_manager*	OverlayManager;
debug_draw_manager* DebugDrawManager;
scene_graph*		SceneGraph;
#ifdef _PROFILE
profile_system*		ProfileSystem;
#endif // _PROFILE


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
		SceneGraph = new scene_graph();
		RenderSystem = new render_system(Window, Instance);
		LoggingSystem = new logging_system();
		InputHandler = new input_handler();
		EventQueue = new event_queue(SystemEventQueue, &StepFrame, &ToggleFrameStepping);
		GameStateBuffer = new gamestate_buffer(&GameState);
		Camera = new camera();
		DebugDrawManager = new debug_draw_manager();
#ifdef _PROFILE
		ProfileSystem = new profile_system();
#endif // _PROFILE

		DebugDrawManager->Initialize(Kilobytes(4));
		SceneGraph->m_Camera = Camera;

		char Buffer[128];
		sprintf_s(Buffer, 128, "Big test");
		DebugDrawManager->AddString(point(0.0f, 0.0f, 1.0f), Buffer);
		sprintf_s(Buffer, 128, "Big test2");
		DebugDrawManager->AddString(point(0.0f, 0.0f, 1.0f), Buffer);
		sprintf_s(Buffer, 128, "Big test3");
		DebugDrawManager->AddString(point(0.0f, 0.0f, 1.0f), Buffer);

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

		OverlayManager = new overlay_manager();
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
		PROFILE_ROOT();
		GameState.Update(Inputs, &StateManager);
		Camera->Update(&GameState, Inputs[0]);
		Render();
		GameState.m_FrameCount++;
	}

	void Render()
	{
		RenderSystem->Clear();
		RenderSystem->Render(SceneGraph);
		OverlayManager->Render(&GameState, &StateManager);
		RenderSystem->Present();
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
		FrameStepMode = 1 - FrameStepMode;
	}
}
