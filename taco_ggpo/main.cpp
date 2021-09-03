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

	float targetFrameTime = 1.0f / 60.0f;

	while (window.isOpen())
	{	
		QueryPerformanceCounter(&startCounter);

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		taco::RunFrame();

		QueryPerformanceCounter(&endCounter);
		float t = (float)(endCounter.QuadPart - startCounter.QuadPart) / (float)frequency.QuadPart;
		sf::Time sleepTime = sf::seconds(targetFrameTime - t - 0.01f);
		if (sleepTime.asSeconds() > 0.0f)
			sf::sleep(sleepTime);
		while (targetFrameTime >= t)
		{
			QueryPerformanceCounter(&endCounter);
			t = (float)(endCounter.QuadPart - startCounter.QuadPart) / (float)frequency.QuadPart;
		}
	}
	return 0;
}