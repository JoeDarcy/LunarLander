// INCLUDES
#include <windows.h>
#include <chrono>
#include <string>
#include "GameObjects.h"

// CONSOLE PARAMETERS
#pragma region Console Buffer Params
// Window size coordinates, be sure to start at zero
SMALL_RECT windowSize{ 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1 };

// A COORD struct for specifying the console's screen buffer dimensions
COORD bufferSize = { SCREEN_WIDTH, SCREEN_HEIGHT };
CONSOLE_SCREEN_BUFFER_INFO SBInfo;

// Setting up different variables for passing to WriteConsoleOutput
COORD characterBufferSize = { SCREEN_WIDTH,SCREEN_HEIGHT }; 
COORD characterPosition = { 0, 0 };
SMALL_RECT consoleWriteArea = { 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1 };

// A CHAR_INFO structure containing data about our frame
CHAR_INFO consoleBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

// Initialise handles
HANDLE wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE rHnd = GetStdHandle(STD_INPUT_HANDLE);
#pragma endregion

// TYPEDEFS
typedef std::chrono::steady_clock::time_point Time;
typedef std::chrono::high_resolution_clock HiResClock;
typedef std::chrono::duration<float> TimeDiff;

// GAME CONSTANTS (player struct GameObjects.h)
const int KeyEsc = VK_ESCAPE;

// Player movement keys
const int KeyW = 'W';
const int KeyA = 'A';
const int KeyS = 'S';
const int KeyD = 'D';

// Menu option keys
const int Key1 = '1';
const int Key2 = '2';
const int Key3 = '3';
const int Key4 = '4';

// ENUMS
enum GAME_STATE
{
	SPLASH,
	MENU,
	OPTIONS,	
	PLAY,
	SCOREBOARD,
	GAME_OVER,
};

// GAME VARIABLES

// Bool for exiting the main game loop
bool exitGame = false;

// Time between each frame
float deltaTime = 0.0f;

// Total number of frames that have elapsed since starting the game
unsigned int frameCounter = 0;		// Unsigned can't be negative

// Sets "Time" to the current time on the HiResClock
Time previousFrameTime = HiResClock::now();

// Current game state
GAME_STATE currentGameState = PLAY;		// SPLASH emun class instead?    Change to SPLASH for build
 

// Connect struct back into the main game 

// Background
Background background;

// Player
Player player;


// FUNCTIONS
void Update();
void Draw();
int ClampInt(int intToClamp, int lowerLimit, int upperLimit);
float ClampFloat(float floatToClamp, float lowerLimit, float upperLimit);
void WriteImageToBuffer(CHAR_INFO* consoleBuffer, const char* charsToPrint, const int coloursToPrint[], const int imageHeight, const int imageWidth, int imageXPos, int imageYPos);
void ClearScreen(CHAR_INFO* consoleBuffer);
void WriteTextToBuffer(CHAR_INFO* consoleBuffer, std::string stringToPrint, int textXPox, int textYPos);


int main()
{
	// Set the console title
	SetConsoleTitle(L"Title of my Console Window");

	// Set screen buffer size
	SetConsoleScreenBufferSize(wHnd, bufferSize);

	// Set up the window size
	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);

	// Display characters to the console window, (main game loop)
	while (!exitGame)
	{
		// Calculate delta time (time since last frame)
		Time  currentFrameTime = HiResClock::now();
		TimeDiff diff = currentFrameTime - previousFrameTime;
		deltaTime = diff.count();
		frameCounter++;

		if (deltaTime >= (1.0f / FRAME_RATE))
		{
			// Calls the Update function for the application
			Update();

			// Cache the timestamp of this frame
			previousFrameTime = currentFrameTime;	
		}	

		// Calls the Draw function
		Draw();
	}
	return 0;
}

// Update function for the main game loop
void Update()
{
	// Switch depending on the current game state
	switch (currentGameState)
	{
		case SPLASH:
		{
			// Create a new splash screen
			static Splash splash;

			// Calculate current splash duration
			splash.Duration += deltaTime;

			if (splash.Duration < 3.0f)
			{
				// Draw splash image to screen					// No colours defined yet
				WriteImageToBuffer(consoleBuffer, splash.CHARACTERS , nullptr, splash.HEIGHT, splash.WIDTH, 0, 0);
			}
			else
			{
				//Change to menu state
				splash.Duration = 0.0f;		// Reset splash duration timer, (incase of reuse)
				currentGameState = MENU;
			}

			break;
		}
		case MENU:
		{
			// Clear any previous images from the screen
			ClearScreen(consoleBuffer);

			// Print out menu options
			WriteTextToBuffer(consoleBuffer, "1. PLAY", (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2));
			WriteTextToBuffer(consoleBuffer, "2. OPTIONS", (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2) + 1);
			WriteTextToBuffer(consoleBuffer, "3. SCOREBOARD", (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2) + 2);
			WriteTextToBuffer(consoleBuffer, "4. QUIT", (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2) + 3);

			if (GetAsyncKeyState(Key1))
			{
				currentGameState = PLAY;
			}
			else if (GetAsyncKeyState(Key2))
			{
				// TODO: Options menu
				currentGameState = OPTIONS;
			}
			else if (GetAsyncKeyState(Key3))
			{
				// TODO: Scoreboard
				currentGameState = SCOREBOARD;
			}
			else if (GetAsyncKeyState(Key4))
			{
				exitGame = true;
			}

			break;
		}
		case OPTIONS:
		{
			// TODO: Options menu

			break;
		}
		case PLAY:
		{
			// Get player input and quit game if escape is pressed
			if (GetAsyncKeyState(KeyEsc))	
			{
				exitGame = true;
				//currentGameState = MENU;
			}

			if (!player.hasLanded && !player.hasCrashed)
			{
				// Get player inputs
				if (GetAsyncKeyState(KeyW))		// Checks if player presses "W", moves character up if pressed
				{
					player.isAccelerating = true;
				}
				if (GetAsyncKeyState(KeyA))	// Checks if player presses "A", moves character left if pressed
				{
					--player.xPos;
				}
				if (GetAsyncKeyState(KeyD))	// Checks if player presses "D", moves character right if pressed
				{
					++player.xPos;
				}

				// Should lander accelerate?
				if (player.isAccelerating)
				{
					player.acceleration += (player.ACCELERATION_RATE * deltaTime);
				}
				else
				{
					player.acceleration -= (player.DECELERATION_RATE * deltaTime);
				}

				// Reset acceleration flag, (bool)
				player.isAccelerating = false;

				// Clamp lander acceleration
				player.acceleration = ClampFloat(player.acceleration, 0.0f, 1.5f);

				// Apply acceleration to the lander
				if (player.acceleration >= 1.0f)			// TODO: Remove magic number, (replace with a const)
				{
					player.yPos--;
				}
				else if (player.acceleration < 0.5f)		// TODO: Remove magic number, (replace with a const)
				{
					player.yPos++;
				}

				// Clamp player input (stop them leaving the window and causing an error)
				player.xPos = ClampInt(player.xPos, 0, (SCREEN_WIDTH - player.WIDTH));
				player.yPos = ClampInt(player.yPos, 0, (SCREEN_HEIGHT - player.HEIGHT));

				// Get the two character under the landing gear of the lander
				char bottomLeftChar = background.CHARACTERS[player.xPos + SCREEN_WIDTH * (player.yPos + (player.HEIGHT - 1))];
				char bottomRightChar = background.CHARACTERS[(player.xPos + (player.WIDTH - 1)) + SCREEN_WIDTH * (player.yPos + (player.HEIGHT - 1))];

				// Did we land or crash?

				// Landed
				if (bottomLeftChar == '_' && bottomRightChar == '_')
				{
					// Landed succeeded!
					player.hasLanded = true;
				}
				else if (bottomLeftChar != ' ' || bottomRightChar != ' ')
				{
					// Crashed!
					player.hasCrashed = true;
				}
			}
			
			
			// Clear the previous "frame" before we start to build the next one
			ClearScreen(consoleBuffer);

			// Draw background image
			WriteImageToBuffer(consoleBuffer, background.CHARACTERS, nullptr, SCREEN_HEIGHT, SCREEN_WIDTH, 0, 0);

			// Draw player image, (sprite)
			WriteImageToBuffer(consoleBuffer, player.CHARACTERS, player.COLOURS, player.HEIGHT, player.WIDTH, player.xPos, player.yPos);

			// Draw game over text
			if (player.hasLanded)
			{
				WriteTextToBuffer(consoleBuffer, "THE EAGLE HAS LANDED!", (SCREEN_WIDTH / 2) - 15, SCREEN_HEIGHT / 2);
			}
			else if (player.hasCrashed)
			{
				WriteTextToBuffer(consoleBuffer, "HOUSTON, WE HAVE A PROBLEM!", (SCREEN_WIDTH / 2) - 18, SCREEN_HEIGHT / 2);
			}

			// Draw UI text
			WriteTextToBuffer(consoleBuffer, "SCORE: ", 1, 0);
			WriteTextToBuffer(consoleBuffer, "TIME: ", 1, 1);
			WriteTextToBuffer(consoleBuffer, "FUEL: ", 1, 2);

			break;
		}
		case SCOREBOARD:
		{
			// TODO: Scoreboard

			break;
		}
		case GAME_OVER:
		{
			// TODO: Game over screen

			break;
		}
		default:
		{
			break;
		}
	}
}

// Draw funtion for rendering the buffer to the screen
void Draw()
{
	// Draw the console buffer array to the screen, (rendering to the screen / "Kick the draw")
	WriteConsoleOutputA(wHnd, consoleBuffer, characterBufferSize, characterPosition, &consoleWriteArea);
}


#pragma region UTILITY FUNCTIONS
// Function to limit player movement to prevent overfolw
int ClampInt(int intToClamp, int lowerLimit, int upperLimit)
{
	if (intToClamp <= lowerLimit)
	{
		return lowerLimit;
	}
	else if (intToClamp >= upperLimit)
	{
		return upperLimit;
	}
	else
	{
		return intToClamp;
	}
}

// Clamp float for keeping lander acceleration reasonable
float ClampFloat(float floatToClamp, float lowerLimit, float upperLimit)
{
	return floatToClamp <= lowerLimit ? lowerLimit : floatToClamp >= upperLimit ? upperLimit : floatToClamp;
}

// Function to write image to buffer ready to be drawn
void WriteImageToBuffer(CHAR_INFO* consoleBuffer, const char* charsToPrint, const int coloursToPrint[], const int imageHeight, const int imageWidth, int imageXPos, int imageYPos)
{
	for (int y = 0; y < imageHeight; y++)
	{
		for (int x = 0; x < imageWidth; x++)
		{
			consoleBuffer[(imageXPos + x) + SCREEN_WIDTH * (imageYPos + y)].Char.AsciiChar = charsToPrint[x + imageWidth * y];
			if (coloursToPrint)
			{
				consoleBuffer[(imageXPos + x) + SCREEN_WIDTH * (imageYPos + y)].Attributes = coloursToPrint[x + imageWidth * y];
			}
			else
			{
				consoleBuffer[(imageXPos + x) + SCREEN_WIDTH * (imageYPos + y)].Attributes = 7;
			}			
		}
	}
}

// Function to clear the screen before redrawing it
void ClearScreen(CHAR_INFO* consoleBuffer)
{
	for (int i = 0; i < (SCREEN_WIDTH * SCREEN_HEIGHT); i++)
	{
		consoleBuffer[i].Char.AsciiChar = 0;
		consoleBuffer[i].Attributes = 0;
	}
}

// Function to write text to the screen
void WriteTextToBuffer(CHAR_INFO* consoleBuffer, std::string stringToPrint, int textXPox, int textYPos)
{
	for (int x = 0; x < stringToPrint.length(); x++)
	{
		consoleBuffer[(textXPox + x) + SCREEN_WIDTH * textYPos].Char.AsciiChar = stringToPrint[x];
		consoleBuffer[(textXPox + x) + SCREEN_WIDTH * textYPos].Attributes = 0xF;
	}
}
#pragma endregion