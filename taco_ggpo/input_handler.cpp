#include "input_handler.h"

input_handler::input_handler()
{
	// TODO: Read input map from file 
	SDL_Init(SDL_INIT_GAMECONTROLLER);
	CreateDefaultInputMap();
}

input_handler::~input_handler()
{
}

uint32 input_handler::GetInputs()
{
	SDL_GameControllerUpdate();
	int32 InputState = 0;
	if (SDL_GameControllerGetButton(m_InputMap.GameController, SDL_CONTROLLER_BUTTON_DPAD_DOWN))
		InputState |= INPUT_DOWN;
	if (SDL_GameControllerGetButton(m_InputMap.GameController, SDL_CONTROLLER_BUTTON_DPAD_UP))
		InputState |= INPUT_UP;
	if (SDL_GameControllerGetButton(m_InputMap.GameController, SDL_CONTROLLER_BUTTON_DPAD_LEFT))
		InputState |= INPUT_LEFT;
	if (SDL_GameControllerGetButton(m_InputMap.GameController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
		InputState |= INPUT_RIGHT;
	for (uint32 i = 0; i < BUTTON_COUNT; i++)
	{
		if (SDL_GameControllerGetButton(m_InputMap.GameController, m_InputMap.Bindings[i].ButtonIndex))
			InputState |= m_InputMap.Bindings[i].InputBinding;
	}

	return InputState;
}

void input_handler::CreateDefaultInputMap()
{
	m_InputMap.GameController = SDL_GameControllerOpen(0);
	m_InputMap.Bindings[0] = { SDL_CONTROLLER_BUTTON_X, INPUT_A };
	m_InputMap.Bindings[1] = { SDL_CONTROLLER_BUTTON_Y, INPUT_B };
	m_InputMap.Bindings[2] = { SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, INPUT_C };
	m_InputMap.Bindings[3] = { SDL_CONTROLLER_BUTTON_A, INPUT_D };
	m_InputMap.Bindings[4] = { SDL_CONTROLLER_BUTTON_B, INPUT_RUN };
}
