#pragma once
#include <string>

// TODO: these are half assed

struct command
{
	int type;
	int directionCount;
	int directions[3];
	int button;
	int bufferTime;
	std::string Trigger;
};