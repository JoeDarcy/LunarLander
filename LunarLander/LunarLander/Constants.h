#ifndef CONSTANTS_H
#define CONSTANTS_H

// INCLUDES
#include <windows.h>

// DEFINES
#define SCREEN_WIDTH 150
#define SCREEN_HEIGHT 40
#define FRAME_RATE 5  // Lower values, (around 5) will give a more old school Atari feel to the gameplay

// Keys
const int KEY_ESC = VK_ESCAPE;
const int KEY_ENTER = VK_RETURN;
// Player movement keys
const int KEY_W = 'W';
const int KEY_A = 'A';
const int KEY_S = 'S';
const int KEY_D = 'D';
// Menu option keys
const int KEY_1 = '1';
const int KEY_2 = '2';
const int KEY_3 = '3';
const int KEY_4 = '4';

// Player parameters
// Lander acceleration rate
const float ACCELERATION_RATE = 0.5f;
// Lander deceleration rate
const float DECELERATION_RATE = 0.2f;

#endif // !CONSTANTS_H