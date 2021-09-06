#pragma once

#include "command.h"

#define INPUT_BUFFER_SIZE 60

struct button_state
{
	int Held;
	int Release;
	int Consumed;

	void Update(bool Input)
	{
		Release = 0;
		if (Input)
		{
			Held++;
		}
		else if (!Input && Held >= 0)
		{
			Release = 1;
			Held = -1;
		}
	}
};

struct direction_state
{
	int Direction;
	int FromNeutral;
	int Consumed;

	void Update(int input)
	{
		int newDirection = 5;
		switch (input)
		{
		case 1:	newDirection = 2; break;
		case 2:	newDirection = 4; break;
		case 3: newDirection = 1; break;
		case 4: newDirection = 6; break;
		case 5: newDirection = 3; break;
		case 8: newDirection = 8; break;
		case 10: newDirection = 7; break;
		case 12: newDirection = 9; break;
		default:
			break;
		}

		if (Direction == 5 && newDirection != 5)
		{
			FromNeutral = 1;
		}
		else
		{
			FromNeutral = 0;
		}

		Direction = newDirection;
	}
};

struct input_states
{
	button_state ButtonStates[4];
	direction_state DirectionState;
};

class input_buffer
{
public:
	input_states m_InputStates[INPUT_BUFFER_SIZE];

	input_buffer();
	~input_buffer();

	void Initialize();
	int Update(int rawInputs);
	int Update(int rawInputs, float Facing);
	int qcfDetection(int maxFrames);
	int CheckForCommand(command Command);
};