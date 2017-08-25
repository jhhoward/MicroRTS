#pragma once

#include <stdint.h>
#include "Entity.h"

#define MAX_PLAYERS 2
#define NUM_STARTING_PEASANTS 1
#define STARTING_GOLD_AMOUNT 100
#define MAX_POPULATION_COUNT 16

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

void Player_Init(uint8_t team);
uint8_t Player_GetPopulationCapacity(uint8_t team);
uint8_t Player_GetPopulationCount(uint8_t team);

