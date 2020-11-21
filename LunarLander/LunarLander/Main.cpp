// INCLUDES
#include <Windows.h>
#include <chrono>
#include "GameObjects.h"
#include "Game.h"

// TYPEDEFS
typedef std::chrono::steady_clock::time_point Time;
typedef std::chrono::high_resolution_clock HiResClock;
typedef std::chrono::duration<float> TimeDiff;

/*
// FUNCTIONS
int ClampInt(int intToClamp, int lowerLimit, int upperLimit);
float ClampFloat(float floatToClamp, float lowerLimit, float upperLimit);
void WriteImageToBuffer(CHAR_INFO* consoleBuffer, const char* charsToPrint, const int coloursToPrint[], const int imageHeight, const int imageWidth, int imageXPos, int imageYPos);
void ClearScreen(CHAR_INFO* consoleBuffer);
void WriteTextToBuffer(CHAR_INFO* consoleBuffer, std::string stringToPrint, int textXPox, int textYPos);
*/
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
	bool exitGame = false;
	while (!exitGame)
	{
		// Calculate delta time (time since last frame)
		currentFrameTime = HiResClock::now();
		TimeDiff diff = currentFrameTime - previousFrameTime;
		deltaTime = diff.count();

		if (deltaTime >= (1.0f / FRAME_RATE))
		{
			// Calls the Update function for the application
			gameInstance.Update(deltaTime);

			// Cache the timestamp of this frame
			previousFrameTime = currentFrameTime;	
		}	

		// Calls the Draw function
		gameInstance.Draw();
	}
	return 0;
}
