#include "input_buffer.h"
#include "defs.h"


input_buffer::input_buffer()
{
}


input_buffer::~input_buffer()
{
}

void input_buffer::Initialize()
{
	for (int i = 0; i < INPUT_BUFFER_SIZE; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m_InputStates[i].ButtonStates[j].Consumed = 0;
			m_InputStates[i].ButtonStates[j].Held = -1;
			m_InputStates[i].ButtonStates[j].Release = 0;
		}
		m_InputStates[i].DirectionState.Consumed = 0;
		m_InputStates[i].DirectionState.Direction = 5;
		m_InputStates[i].DirectionState.FromNeutral = 0;
	}
}

int input_buffer::Update(int rawInputs)
{
	//TODO: ring buffer
	for (int i = INPUT_BUFFER_SIZE - 2; i >= 0; i--)
	{
		m_InputStates[i + 1] = m_InputStates[i];
	}

	m_InputStates[0].ButtonStates[0].Update(rawInputs & INPUT_A);
	m_InputStates[0].ButtonStates[1].Update(rawInputs & INPUT_B);
	m_InputStates[0].ButtonStates[2].Update(rawInputs & INPUT_C);
	m_InputStates[0].DirectionState.Update(rawInputs & (INPUT_DIRECTIONS));
	
	return 0;
}

// TODO: change to something more general, like input being an array with stuff
int input_buffer::qcfDetection(int maxFrames)
{
	int foundInputIndex = 2;
	int directions[3] = { 2, 3, 6 };

	for (int i = 0; i < maxFrames; i++)
	{
		if (directions[foundInputIndex] == m_InputStates[i].DirectionState.Direction)
			foundInputIndex--;
		if (foundInputIndex < 0)
			return 1;
	}
	return 0;
}

int input_buffer::CheckForCommand(command Command)
{
	if (Command.type == 0)
	{
		if (m_InputStates[0].DirectionState.Direction == Command.directions[0] &&
			m_InputStates[0].ButtonStates[Command.button].Held == 0)
		{
			return 1;
		}
	}
	else if (Command.type == 1)
	{
		if (m_InputStates[0].ButtonStates[Command.button].Held == 0)
		{
			int foundInputIndex = Command.directionCount - 1;

			for (int i = 0; i < Command.bufferTime; i++)
			{
				if (Command.directions[foundInputIndex] == m_InputStates[i].DirectionState.Direction)
					foundInputIndex--;
				if (foundInputIndex < 0)
					return 1;
			}
		}
	}
	else if (Command.type == 2)
	{
		if (Command.directions[0] == m_InputStates[0].DirectionState.Direction)
		{
			return 1;
		}
	}
	return 0;
}
