#pragma once
#include <string>

struct Command
{
	int type;
	int directionCount;
	int directions[3];
	int button;
	int bufferTime;
	std::string Trigger;
};