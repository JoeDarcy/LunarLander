// INCLUDES
#include <windows.h>
#include <chrono>

// DEFINES
#define WIDTH 150
#define HEIGHT 40
#define FRAME_RATE 30  // Lower values, (around 5) will give a more old school Atari feel to the gameplay

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

// TYPEDEFS
typedef std::chrono::steady_clock::time_point Time;
typedef std::chrono::high_resolution_clock HiResClock;
typedef std::chrono::duration<float> TimeDiff;

// GAME CONSTANTS (create player struct?)
const int PlayerWidth = 2;		// Define player character width
const int PlayerHeight = 3;		// Define player character height

// Define player character shape 
const char* PlayerCharacters =
{
	R"(__)"		// Values must match player width & height if sprite is changed
	R"(||)"
	R"(/\)"
};

// Define player character colours
const int PlayerColours[PlayerWidth * PlayerHeight] =
{
	0xF, 0xF,		
	0xF, 0xF,
	0xE, 0xE,
};

// Create the background for the window
const char* BackGroundCharacters =
{	// |1   Guide for background image spacing (line indicates position)																					|150
	R"(......................................................................................................................................................)"
	R"(......................................................................................................................................................)"
	R"(......................................................................................................................................................)"
	R"(......................................................................................................................................................)"
	R"(......................................................................................................................................................)"
	R"(......................................................................................................................................................)"
	R"(......................................................................................................................................................)"
	R"(......................................................................................................................................................)"
	R"(......................................................................................................................................................)"
	R"(......................................................................................................................................................)"
	R"(......................................................................................................................................................)"
	R"(......................................................................................................................................................)"
	R"(...............................................................______.................................................................................)"
	R"(............................................................../......\................................................................................)"
	R"(............................................................./........\...............................................................................)"
	R"(............................................................/..........\..............................................................................)"
	R"(.........................................................../............\.............................................................................)"
	R"(........................................................../..............\............................................................................)"
	R"(........................................................./................\...........................................................................)"
	R"(......................................................../..................\___.............__........................................................)"
	R"(......................................................./.......................\.........../..\.......................................................)"
	R"(....................................................../.........................\........./....\......................................................)"
	R"(...................................................../...........................\......./......\.....................................................)"
	R"(..................................................../.............................\_____/........\....................................................)"
	R"(.................................................../..............................................\...................................................)"
	R"(................................................../................................................\..................................................)"
	R"(................................................./..................................................\.................................................)"
	R"(....................................___........./....................................................\___.............................................)"
	R"(.................................../...\......./.........................................................\............................................)"
	R"(................................../.....\...../...........................................................\.......................................__..)"
	R"(................................./.......\.../.............................................................\.................._____............../..\.)"
	R"(................................/.........\_/...............................................................\................/.....\............/....\)"
	R"(....................____......./.............................................................................\............../.......\........../......)"
	R"(.................../....\...../...............................................................................\............/.........\......../.......)"
	R"(................../......\.../.................................................................................\........../...........\....../........)"
	R"(.....___........./........\_/...................................................................................\......../.............\____/.........)"
	R"(..../...\......./................................................................................................\....../.............................)"
	R"(.../.....\...../..................................................................................................\____/..............................)"
	R"(../.......\___/.......................................................................................................................................)"
	R"(_/....................................................................................................................................................)"
};

const int KeyEsc = VK_ESCAPE;
const int KeyW = 'W';
const int KeyA = 'A';
const int KeyS = 'S';
const int KeyD = 'D';


// GAME VARIABLES
int playerXPos = 0;
int playerYPos = 0;

// Bool for exiting the main game loop
bool exitGame = false;

// Time between each frame
float deltaTime = 0.0f;

// Total number of frames that have elapsed since starting the game
unsigned int frameCounter = 0;		// Unsigned can't be negative

// Sets "Time" to the current time on the HiResClock
Time previousFrameTime = HiResClock::now();


// FUNCTIONS
int ClampInt(int intToClamp, int lowerLimit, int upperLimit);



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
			// Cache the timestamp of this frame
			previousFrameTime = currentFrameTime;

			// Get player input
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

			// Clamp player input (stop them leaving the window and causing an error)
			playerXPos = ClampInt(playerXPos, 0, (WIDTH - PlayerWidth));
			playerYPos = ClampInt(playerYPos, 0, (HEIGHT - PlayerHeight));

			// Clear the previous "frame" before we start to build the next one
			for (int i = 0; i < (WIDTH * HEIGHT); i++)
			{
				consoleBuffer[i].Char.AsciiChar = 0;
				consoleBuffer[i].Attributes = 0;
			}

			// Draw player image, (sprite)
			for (int y = 0; y < PlayerHeight; y++)
			{
				for (int x = 0; x < PlayerWidth; x++)
				{
					consoleBuffer[(playerXPos + x) + WIDTH * (playerYPos + y)].Char.AsciiChar = PlayerCharacters[x + PlayerWidth * y];
					consoleBuffer[(playerXPos + x) + WIDTH * (playerYPos + y)].Attributes = PlayerColours[x + PlayerWidth * y];
				}
			}

			// Draw the console buffer array to the screen, (rendering to the screen / "Kick the draw")
			WriteConsoleOutputA(wHnd, consoleBuffer, characterBufferSize, characterPosition, &consoleWriteArea);
		}		
	}
	return 0;
}


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