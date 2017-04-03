#pragma once

#include <stdint.h>

enum GameState
{
	GameState_Menu,
	GameState_Playing
};

enum PlayerType
{
	Player_Human,
	Player_AI
};

typedef struct
{
	uint8_t state : 4;
	uint8_t isSplitScreen : 1;
	uint8_t player1 : 1;
	uint8_t player2 : 1;
} GameData;

extern GameData Game;