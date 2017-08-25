#include "Player.h"
#include "Game.h"
#include "Map.h"

void Player_Init(uint8_t team)
{
	Player* player = &Game.Players[team];
	player->gold = STARTING_GOLD_AMOUNT;
	player->selectedEntity.value = INVALID_ENTITY_VALUE;
	player->hoverEntity.value = INVALID_ENTITY_VALUE;

	// Spawn base
	uint8_t buildX = pgm_read_byte(&Game.Map->playerStart[team].x);
	uint8_t buildY = pgm_read_byte(&Game.Map->playerStart[team].y);
	
	const BuildingTypeInfo* typeInfo = &AllBuildingTypeInfo[BuildingType_TownCenter];	
	Map_FindClearSpace(&buildX, &buildY, pgm_read_byte(&typeInfo->width) + 4, pgm_read_byte(&typeInfo->height) + 4);
	
	EntityID townCenterID = Building_Place(team, BuildingType_TownCenter, buildX + 2, buildY + 2);
	Building* townCenter = Building_Get(townCenterID);
	
	townCenter->hp = pgm_read_byte(&typeInfo->hp);
	townCenter->buildType = BuildType_None;

	for(uint8_t n = 0; n < NUM_STARTING_PEASANTS; n++)
	{
		Building_FindAdjacentSpace(townCenter, &buildX, &buildY);
		Unit_Spawn(team, UnitType_Peasant, buildX, buildY);
	}
	

	// TODO: set up view and cursor positions
}

uint8_t Player_GetPopulationCount(uint8_t team)
{
	uint8_t count = 0;
	
	for(uint8_t n = 0; n < MAX_UNITS; n++)
	{
		if(Game.Units[n].team == team && Game.Units[n].type != UnitType_Invalid && Game.Units[n].hp > 0)
		{
			count++;
		}
	}
	
	return count;
}

uint8_t Player_GetPopulationCapacity(uint8_t team)
{
	uint8_t count = 0;
	
	for(uint8_t n = 0; n < MAX_BUILDINGS; n++)
	{
		if(Game.Buildings[n].team == team)
		{
			if(Game.Buildings[n].type == BuildingType_House)
			{
				count += 3;
			}
			else if(Game.Buildings[n].type == BuildingType_TownCenter)
			{
				count += 4;
			}
		}
		
		if(count > MAX_POPULATION_COUNT)
		{
			count = MAX_POPULATION_COUNT;
		}
	}
	
	return count;
}
