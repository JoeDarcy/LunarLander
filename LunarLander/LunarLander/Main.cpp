#include <windows.h>

// DEFINES
#define WIDTH 150
#define HEIGHT 40

// CONSOLE PARAMETERS
#pragma region Console Buffer Params
// Window size coordinates, be sure to start at zero
SMALL_RECT windowSize{ 0, 0, WIDTH - 1, HEIGHT - 1 };

// A COORD struct for specifying the console's screen buffer dimensions
COORD bufferSize = { WIDTH, HEIGHT };
CONSOLE_SCREEN_BUFFER_INFO SBInfo;

// Setting up different variables for passing to WriteConsoleOutput
COORD characterBufferSize = { WIDTH,HEIGHT }; 
COORD characterPosition = { 0, 0 };
SMALL_RECT consoleWriteArea = { 0, 0, WIDTH - 1, HEIGHT - 1 };

// A CHAR_INFO structure containing data about our frame
CHAR_INFO consoleBuffer[WIDTH * HEIGHT];

// Initialise handles
HANDLE wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE rHnd = GetStdHandle(STD_INPUT_HANDLE);
#pragma endregion

// GAME CONSTANTS
const int playerCharacter = '@';
const int playerColour = 7;

const int KeyEsc = VK_ESCAPE;
const int KeyW = 'W';
const int KeyA = 'A';
const int KeyS = 'S';
const int KeyD = 'D';


// GAME VARIABLES
int playerXPos = 0;
int playerYPos = 0;

int main()
{
	// Set the console title
	SetConsoleTitle(L"Title of my Console Window");

	// Set screen buffer size
	SetConsoleScreenBufferSize(wHnd, bufferSize);

	// Set up the window size
	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);

	// Bool for exiting the main game loop
	bool exitGame = false;

	// Display characters to the console window, (main game loop)
	while (!exitGame)
	{
		if (GetAsyncKeyState(KeyEsc))	// Checks if player presses "Escape", quits game if pressed
		{
			exitGame = true;
		}
		if (GetAsyncKeyState(KeyW))		// Checks if player presses "W", moves character up if pressed
		{
			--playerYPos;
		}
		if (GetAsyncKeyState(KeyA))	// Checks if player presses "A", moves character left if pressed
		{
			--playerXPos;
		}
		if (GetAsyncKeyState(KeyS))	// Checks if player presses "S", moves character down if pressed
		{
			++playerYPos;
		}
		if (GetAsyncKeyState(KeyD))	// Checks if player presses "D", moves character right if pressed
		{
			++playerXPos;
		}

		// Clear the previous "frame" before we start to build the next one
		for (int i = 0; i < (WIDTH * HEIGHT); i++)
		{
			consoleBuffer[i].Char.AsciiChar = 0;
			consoleBuffer[i].Attributes = 0;
		}

		// Get character struct
		CHAR_INFO thisChar = consoleBuffer[playerXPos + WIDTH * playerYPos];

		//Set the ASCII character and colour for the player
		thisChar.Char.AsciiChar = playerCharacter;
		thisChar.Attributes = playerColour; 

		// Override that character in our buffer with new data
		consoleBuffer[playerXPos + WIDTH * playerYPos] = thisChar;

		// Draw the console buffer array to the screen, (rendering to the screen)
		WriteConsoleOutputA(wHnd, consoleBuffer, characterBufferSize, characterPosition, &consoleWriteArea);

	}



	return 0;
}