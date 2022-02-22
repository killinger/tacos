#pragma once
#include "defs.h"
#include <SDL2/SDL.h>

class input_handler
{
public:
	input_handler();
	~input_handler();

	uint32 GetInputs();
private:
	struct input_map
	{
		struct button_binding
		{
			SDL_GameControllerButton	ButtonIndex;
			int32						InputBinding;
		};
		SDL_GameController* GameController;
		button_binding		Bindings[BUTTON_COUNT];
	};
	input_map m_InputMap;

	void CreateDefaultInputMap();
};