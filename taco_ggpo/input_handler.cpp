#include "input_handler.h"
#include <SFML/Window.hpp>

input_handler::input_handler()
{
	// TODO: Read input map from file 
	CreateDefaultInputMap();
}

input_handler::~input_handler()
{
}

uint32 input_handler::GetInputs()
{
	int32 InputState = 0;
	if (sf::Joystick::getAxisPosition(m_InputMap.Device, sf::Joystick::PovY) < -1.0)
		InputState |= INPUT_DOWN;
	if (sf::Joystick::getAxisPosition(m_InputMap.Device, sf::Joystick::PovY) > 1.0)
		InputState |= INPUT_UP;
	if (sf::Joystick::getAxisPosition(m_InputMap.Device, sf::Joystick::PovX) < -1.0)
		InputState |= INPUT_LEFT;
	if (sf::Joystick::getAxisPosition(m_InputMap.Device, sf::Joystick::PovX) > 1.0)
		InputState |= INPUT_RIGHT;
	for (uint32 i = 0; i < BUTTON_COUNT; i++)
	{
		if (sf::Joystick::isButtonPressed(m_InputMap.Device, m_InputMap.Bindings[i].ButtonIndex))
			InputState |= m_InputMap.Bindings[i].InputBinding;
	}

	return InputState;
}

void input_handler::CreateDefaultInputMap()
{
	m_InputMap.Device = 0;
	m_InputMap.Bindings[0] = { 0, INPUT_A };
	m_InputMap.Bindings[1] = { 3, INPUT_B };
	m_InputMap.Bindings[2] = { 5, INPUT_C };
	m_InputMap.Bindings[3] = { 7, INPUT_D };
	m_InputMap.Bindings[4] = { 2, INPUT_RUN };
}
