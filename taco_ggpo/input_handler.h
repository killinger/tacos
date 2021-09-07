#pragma once
#include "defs.h"

class input_handler
{
public:
	input_handler();
	~input_handler();

	uint32 GetInputs();
	uint32 GetKey();
private:
	struct input_map
	{
		struct button_binding
		{
			int32 ButtonIndex;
			int32 InputBinding;
		};
		int32			Device;
		button_binding	Bindings[BUTTON_BIND_COUNT];
	};

	input_map m_InputMap;

	void CreateDefaultInputMap();
};