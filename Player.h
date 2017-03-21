#pragma once

#include <stdint.h>
#include "Entity.h"

#define MAX_PLAYERS 2

typedef struct
{
	uint16_t gold;
	uint8_t baseX, baseY;
	uint8_t viewX, viewY;
	uint16_t cursorX, cursorY;
	EntityID selectedEntity;
	EntityID hoverEntity;
} Player;

extern Player AllPlayers[MAX_PLAYERS];
