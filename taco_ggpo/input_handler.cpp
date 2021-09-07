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
	for (uint32 i = 0; i < BUTTON_BIND_COUNT; i++)
	{
		if (sf::Joystick::isButtonPressed(m_InputMap.Device, m_InputMap.Bindings[i].ButtonIndex))
			InputState |= m_InputMap.Bindings[i].InputBinding;
	}

	return InputState;
}

uint32 input_handler::GetKey()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Backspace))
		return 8;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
		return 13;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		return 32;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Period))
		return 46;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0))
		return 48;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
		return 49;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
		return 50;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
		return 51;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
		return 52;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
		return 53;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6))
		return 54;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num7))
		return 55;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num8))
		return 56;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num9))
		return 57;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
		return 92;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		return 97;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
		return 98;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
		return 99;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		return 100;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
		return 101;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
		return 102;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
		return 103;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::H))
		return 104;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
		return 105;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::J))
		return 106;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
		return 107;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
		return 108;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
		return 109;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
		return 110;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
		return 111;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
		return 112;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		return 113;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
		return 114;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		return 115;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
		return 116;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::U))
		return 117;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
		return 118;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		return 119;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
		return 120;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y))
		return 121;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
		return 122;

	return 0;
}

void input_handler::CreateDefaultInputMap()
{
	m_InputMap.Device = 0;
	m_InputMap.Bindings[0] = { 0, INPUT_A };
	m_InputMap.Bindings[1] = { 3, INPUT_B };
	m_InputMap.Bindings[2] = { 5, INPUT_C };
	m_InputMap.Bindings[3] = { 7, INPUT_D };
}
