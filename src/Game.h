#pragma once

#include <stdint.h>
#include "Unit.h"
#include "Building.h"
#include "Player.h"
#include "Resource.h"
#include "Map.h"

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
	
	Player Players[MAX_PLAYERS];
	Unit Units[MAX_UNITS];
	Building Buildings[MAX_BUILDINGS];
	uint8_t Resources[MAX_RESOURCES >> 1];
	
	const Map* Map;
} GameData;

extern GameData Game;