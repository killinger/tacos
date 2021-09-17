#include <SFML\Graphics.hpp>
#include <Windows.h>
#include "taco_game.h"

// TODO: different frame limiting scheme, maybe

int main()
{
	sf::RenderWindow window(sf::VideoMode(1280, 720), "Tacos");

	taco::Initialize(&window);

	LARGE_INTEGER startCounter, endCounter;
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&startCounter);
	float targetFrameTime = 1.0f / 60.0f;
	float Next = 0.0f;
	float Now = 0.0f;

	while (window.isOpen())
	{	
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		QueryPerformanceCounter(&endCounter);
		Now = (float)(endCounter.QuadPart - startCounter.QuadPart) / (float)frequency.QuadPart;
		if (Now >= Next)
		{
			taco::RunFrame();
			Next = Now + targetFrameTime;
		}
	}
	return 0;
}