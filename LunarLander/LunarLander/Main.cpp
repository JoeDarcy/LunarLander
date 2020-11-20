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

// GAME
const unsigned char characterToPrint = 219;


int main()
{
	// Set the console title
	SetConsoleTitle(L"Title of my Console Window");

	//Set screen buffer size
	SetConsoleScreenBufferSize(wHnd, bufferSize);

	// Set up the window
	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);

	// Bool for exiting the main game loop
	bool exitGame = false;

	// Display characters to the console window
	while (!exitGame)
	{
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			exitGame = true;
		}

		for (int y = 0; y < HEIGHT; y++)
		{
			for (int x = 0; x < WIDTH; x++)
			{
				consoleBuffer[x + WIDTH * y].Char.AsciiChar = characterToPrint; // Chooses character to print
				consoleBuffer[x + WIDTH * y].Attributes = rand() % 256;			// Choose random colour
			}
		}

		// Draw the console buffer array to the screen
		WriteConsoleOutputA(wHnd, consoleBuffer, characterBufferSize, characterPosition, &consoleWriteArea);

	}



	return 0;
}