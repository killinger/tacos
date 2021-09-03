#pragma once

// TODO: everything about this a mess pls get rid off

#define IN_2 0x01
#define IN_4 0x02
#define IN_6 0x04
#define IN_8 0x08
#define IN_A 0x10
#define IN_B 0x20
#define IN_C 0x40
#define IN_D 0x80

uint32 PollInput()
{
	int inputState = 0;
	if (sf::Joystick::getAxisPosition(0, sf::Joystick::PovY) < -1.0)
		inputState |= IN_2;
	if (sf::Joystick::getAxisPosition(0, sf::Joystick::PovY) > 1.0)
		inputState |= IN_8;
	if (sf::Joystick::getAxisPosition(0, sf::Joystick::PovX) < -1.0)
		inputState |= IN_4;
	if (sf::Joystick::getAxisPosition(0, sf::Joystick::PovX) > 1.0)
		inputState |= IN_6;
	if (sf::Joystick::isButtonPressed(0, 0))
		inputState |= IN_A;
	if (sf::Joystick::isButtonPressed(0, 3))
		inputState |= IN_B;
	if (sf::Joystick::isButtonPressed(0, 5))
		inputState |= IN_C;

	return inputState;
}

unsigned int PollKeyboard()
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