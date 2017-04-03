#pragma once

#include <stdint.h>
#include "Entity.h"

#define MAX_PLAYERS 2

enum
{
	Player1_Team = 0,
	Player2_Team = 1
};

typedef struct
{
	uint16_t gold;
	uint8_t viewX, viewY;
	uint16_t cursorX, cursorY;
	EntityID selectedEntity;
	EntityID hoverEntity;
} Player;

extern Player AllPlayers[MAX_PLAYERS];

