#include <stdint.h>
#include "avr/pgmspace.h"
#include "Map.h"
#include "Pathing.h"
#include "Game.h"
#include "System.h"

const uint8_t* MacroTileData = 0;

uint8_t Map_GetTileTypeImpl(uint8_t x, uint8_t y)
{
	uint8_t macroTileX = x >> 3;
	uint8_t macroTileY = y >> 3;

	// Find type of macro tile, e.g. horizontal road, vertical river with bridge, etc
	uint8_t macroTileType = pgm_read_byte(&Game.Map->macroMap[macroTileY * MACRO_MAP_SIZE + macroTileX]);
	
	uint8_t tileX = x & 7;
	uint8_t tileY = y & 7;

	const uint8_t* tileDataBase = MacroTileData + (macroTileType * MACRO_TILE_SIZE * MACRO_TILE_SIZE);
	return pgm_read_byte(&tileDataBase[tileY * MACRO_TILE_SIZE + tileX]);
}

uint8_t Map_GetTileType(uint8_t x, uint8_t y)
{
	return 0;
}

BlockedType Map_IsBlocked(uint8_t x, uint8_t y)
{
	if(x >= MAP_SIZE || y >= MAP_SIZE)
	{
		return BlockedByWorld;
	}
	
	uint8_t tileType = Map_GetTileType(x, y);
	
	if(tileType < TileType_FirstWalkable || tileType > TileType_LastWalkable)
	{
		return BlockedByWorld;
	}
	
	for(uint8_t n = 0; n < MAX_UNITS; n++)
	{
		Unit* unit = &Game.Units[n];
		if(unit->type != UnitType_Invalid && unit->hp > 0 && unit->agent.x == x && unit->agent.y == y)
		{
			if(unit->agent.pathingState == Pathing_Idle || unit->agent.pathingState == Pathing_Failed)
			{
				return BlockedByStaticUnit;
			}
			else
			{
				return BlockedByMovingUnit;
			}
		}
	}
	
	{
		EntityID resource = Resource_GetAtLocation(x, y);
		if(resource.value != INVALID_ENTITY_VALUE && Resource_GetRemaining(resource) > 0)
		{
			return BlockedByResource;
		}
	}

	{
		EntityID building = Building_GetAtLocation(x, y);
		if(building.value != INVALID_ENTITY_VALUE && Building_Get(building)->hp > 0)
		{
			return BlockedByBuilding;
		}
	}	
	
	return NotBlocked;
}

bool Map_IsClear(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	if(x + width >= MAP_SIZE)
	{
		return false;
	}
	if(y + height >= MAP_SIZE)
	{
		return false;
	}
	
	for(uint8_t i = x; i < x + width; i++)
	{
		for(uint8_t j = y; j < y + height; j++)
		{
			if(Map_IsBlocked(i, j))
			{
				return false;
			}
		}
	}
	
	return true;
}

bool Map_FindClearSpace(uint8_t* buildX, uint8_t* buildY, uint8_t clearWidth, uint8_t clearHeight)
{
	uint8_t spiralSearchLength = 1;
	uint8_t spiralDirection = North;
	
	LOG("Searching for clear space for %d x %d near %d, %d\n", clearWidth, clearHeight, *buildX, *buildY);
	
	while(spiralSearchLength < 64)
	{
		for(uint8_t pass = 0; pass < 2; pass++)
		{
			for(uint8_t i = 0; i < spiralSearchLength; i++)
			{
				if(Map_IsClear(*buildX, *buildY, clearWidth, clearHeight))
				{
					LOG("Found clear space for %d x %d at %d, %d\n", clearWidth, clearHeight, *buildX, *buildY);
					return true;
				}

				switch(spiralDirection)
				{
					case North:
					if((*buildY) > 0)
						(*buildY)--;
					break;
					case East:
					if((*buildX) < MAP_SIZE - 1)
						(*buildX)++;
					break;
					case South:
					if((*buildY) < MAP_SIZE - 1)
						(*buildY)++;
					break;
					case West:
					if((*buildX) > 0)
						(*buildX)--;
					break;
				}
			}
			
			spiralDirection = (spiralDirection + 2) & 7;
		}
		
		spiralSearchLength++;		
	}	

	LOG("Cannot find clear space for size %d x %d\n", clearWidth, clearHeight);
	
	return false;
}
