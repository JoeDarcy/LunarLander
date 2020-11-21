// This classes header
#include "Game.h"

// INCLUDES
#include "Utility.h"
#include "Constants.h"

Game::Game()
{
	// Constructor
}

// Call Initialise function
void Game::Initialise()
{
	// Set the console title
	SetConsoleTitle(L"Title of my Console Window");

	// Set screen buffer size
	SetConsoleScreenBufferSize(wHnd, bufferSize);

	// Set up the window size
	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);
}

// Update function for the main game loop
void Game::Update(float deltaTime)
{
	static bool exitGame = false;

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
				WriteImageToBuffer(consoleBuffer, splash.CHARACTERS, nullptr, splash.HEIGHT, splash.WIDTH, 0, 0);
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
			WriteTextToBuffer(consoleBuffer, "1. PLAY", (SCREEN_WIDTH / 2) - 10, (SCREEN_HEIGHT / 2) - 10);
			WriteTextToBuffer(consoleBuffer, "2. OPTIONS", (SCREEN_WIDTH / 2) - 10, (SCREEN_HEIGHT / 2) - 9);
			WriteTextToBuffer(consoleBuffer, "3. SCOREBOARD", (SCREEN_WIDTH / 2) - 10, (SCREEN_HEIGHT / 2) - 8);
			WriteTextToBuffer(consoleBuffer, "4. QUIT", (SCREEN_WIDTH / 2) - 10, (SCREEN_HEIGHT / 2) - 7);

			if (GetAsyncKeyState(KEY_1))
			{
				currentGameState = PLAY;
			}
			else if (GetAsyncKeyState(KEY_2))
			{
				// TODO: Options menu
				currentGameState = OPTIONS;
			}
			else if (GetAsyncKeyState(KEY_3))
			{
				// TODO: Scoreboard
				currentGameState = SCOREBOARD;
			}
			else if (GetAsyncKeyState(KEY_4))
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
			if (GetAsyncKeyState(KEY_ESC))
			{
				exitGame = true;
			}

			// Get player input and quit game if enter is pressed
			if (player.hasCrashed && GetAsyncKeyState(KEY_ENTER))
			{
				player.Reset();
				currentGameState = MENU;
			}

			if (!player.hasLanded && !player.hasCrashed)
			{
				// Get player input and quit game if escape is pressed
				if (GetAsyncKeyState(KEY_ESC))
				{
					exitGame = true;
					//currentGameState = MENU;
				}

				// Get player inputs
				if (GetAsyncKeyState(KEY_W) && player.fuel > 0.0f)		// Checks if player presses "W", moves character up if pressed
				{
					player.isAccelerating = true;
					// Expend fuel
					player.fuel -= FUEL_CONSUMPTION_RATE;

					// Draw thrusters animation if accelerating
					if (player.isAccelerating)
					{
						static float thrusterFlashTimer = 0.0f;

						thrusterFlashTimer += deltaTime;

						if (thrusterFlashTimer >= 0.2f)
						{
							// Draw thrusters frame 1
							WriteImageToBuffer(consoleBuffer,  thrusters.CHARACTERS_1, thrusters.COLOURS, thrusters.HEIGHT, thrusters.WIDTH, player.xPos + 1, player.yPos - 3);		// Draw order bug, drawing behind player / background?
							thrusterFlashTimer = 0.0f;
						}
						else
						{
							// Draw thrusters frame 2
							WriteImageToBuffer(consoleBuffer, thrusters.CHARACTERS_2, thrusters.COLOURS, thrusters.HEIGHT, thrusters.WIDTH, player.xPos, player.yPos);
						}
					}
				}
				if (GetAsyncKeyState(KEY_A))	// Checks if player presses "A", moves character left if pressed
				{
					--player.xPos;
				}
				if (GetAsyncKeyState(KEY_D))	// Checks if player presses "D", moves character right if pressed
				{
					++player.xPos;
				}

				// Should lander accelerate?
				if (player.isAccelerating)
				{
					player.acceleration += (ACCELERATION_RATE * deltaTime);
				}
				else
				{
					player.acceleration -= (DECELERATION_RATE * deltaTime);
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

			// Draw explosion animation if crashed
			if (player.hasCrashed)
			{
				static float explosionFlashTimer = 0.0f;
				
				explosionFlashTimer += deltaTime;

				if (explosionFlashTimer >= 0.5f)
				{
					// Draw explosion frame 1
					WriteImageToBuffer(consoleBuffer, explosion.CHARACTERS_1, explosion.COLOURS, explosion.HEIGHT, explosion.WIDTH, player.xPos, player.yPos);
					explosionFlashTimer = 0.0f;
				}
				else
				{
					// Draw explosion frame 2
					WriteImageToBuffer(consoleBuffer, explosion.CHARACTERS_2, explosion.COLOURS, explosion.HEIGHT, explosion.WIDTH, player.xPos, player.yPos);
				}
				// Draw game over text
				WriteTextToBuffer(consoleBuffer, "HOUSTON, WE HAVE A PROBLEM!", (SCREEN_WIDTH / 2) - 18, SCREEN_HEIGHT / 2);
				WriteTextToBuffer(consoleBuffer, "Press 'Enter' to return to menu...", (SCREEN_WIDTH / 2) - 21, (SCREEN_HEIGHT / 2) + 1);
			}
			// Draw game over text
			else if (player.hasLanded)
			{
				WriteTextToBuffer(consoleBuffer, "THE EAGLE HAS LANDED!", (SCREEN_WIDTH / 2) - 15, SCREEN_HEIGHT / 2);
				WriteTextToBuffer(consoleBuffer, "Press 'Enter' to return to menu...", (SCREEN_WIDTH / 2) - 21, (SCREEN_HEIGHT / 2) + 1);
			}
			else
			{
				// Draw player image, (sprite)
				WriteImageToBuffer(consoleBuffer, player.CHARACTERS, player.COLOURS, player.HEIGHT, player.WIDTH, player.xPos, player.yPos);
			}
			
			// Draw UI text
			WriteTextToBuffer(consoleBuffer, "SCORE: ", 1, 0);
			WriteTextToBuffer(consoleBuffer, "TIME: ", 1, 1);
			WriteTextToBuffer(consoleBuffer, "FUEL: " + std::to_string(player.fuel), 1, 2);

			break;
		}
		case SCOREBOARD:
		{
			// TODO: Scoreboard

			break;
		}
		default:
		{
			break;
		}
	}
}

// Draw funtion for rendering the buffer to the screen
void Game::Draw()
{
	// Draw the console buffer array to the screen, (rendering to the screen / "Kick the draw")
	WriteConsoleOutputA(wHnd, consoleBuffer, characterBufferSize, characterPosition, &consoleWriteArea);
}