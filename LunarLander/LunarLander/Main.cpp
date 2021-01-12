// INCLUDES
#include <Windows.h>
#include <chrono>
#include "GameObjects.h"
#include "Game.h"

// TYPEDEFS
typedef std::chrono::steady_clock::time_point Time;
typedef std::chrono::high_resolution_clock HiResClock;
typedef std::chrono::duration<float> TimeDiff;

int main()
{
	Game gameInstance;

	// Initialise the console window
	gameInstance.Initialise();

	// Initialise variables
	float deltaTime = 0.0f;
	Time currentFrameTime = HiResClock::now();
	Time previousFrameTime = HiResClock::now();

	// Display characters to the console window, (main game loop)
	// Bool for exiting the main game loop
	bool exit = false;
	while (!exit)
	{
		// Calculate delta time (time since last frame)
		currentFrameTime = HiResClock::now();
		TimeDiff diff = currentFrameTime - previousFrameTime;
		deltaTime = diff.count();

		if (deltaTime >= (1.0f / FRAME_RATE))
		{
			// Calls the Update function for the application
			exit = gameInstance.Update(deltaTime);

			// Cache the timestamp of this frame
			previousFrameTime = currentFrameTime;	
		}	

		// Calls the Draw function
		gameInstance.Draw();
	}
	return 0;
}
