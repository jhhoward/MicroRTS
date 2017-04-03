#include <stdint.h>
#include "avr/pgmspace.h"
#include "Map.h"
#include "Pathing.h"

const Map* CurrentMap;

const uint8_t* MacroTileData = 0;

uint8_t Map_GetTileType(uint8_t x, uint8_t y)
{
	uint8_t macroTileX = x >> 3;
	uint8_t macroTileY = y >> 3;

	// Find type of macro tile, e.g. horizontal road, vertical river with bridge, etc
	uint8_t macroTileType = pgm_read_byte(&CurrentMap->macroMap[macroTileY * MACRO_MAP_SIZE + macroTileX]);
	
	uint8_t tileX = x & 7;
	uint8_t tileY = y & 7;

	const uint8_t* tileDataBase = MacroTileData + (macroTileType * MACRO_TILE_SIZE * MACRO_TILE_SIZE);
	return pgm_read_byte(&tileDataBase[tileY * MACRO_TILE_SIZE + tileX]);
}

bool Map_IsWalkable(uint8_t x, uint8_t y)
{
	uint8_t tileType = Map_GetTileType(x, y);
	
	return tileType >= TileType_FirstWalkable && tileType <= TileType_LastWalkable;
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
			if(!Map_IsWalkable(i, j))
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
	
	while(spiralSearchLength < 64)
	{
		for(uint8_t pass = 0; pass < 2; pass++)
		{
			for(uint8_t i = 0; i < spiralSearchLength; i++)
			{
				if(Map_IsClear(*buildX, *buildY, clearWidth, clearHeight))
				{
					return true;
				}

				switch(spiralDirection)
				{
					case North:
					if(*buildY > 0)
						*buildY--;
					break;
					case East:
					if(*buildX < MAP_SIZE - 1)
						*buildX++;
					break;
					case South:
					if(*buildY < MAP_SIZE - 1)
						*buildY++;
					break;
					case West:
					if(*buildX > 0)
						*buildX--;
					break;
				}
			}
			
			spiralDirection = (spiralDirection + 2) & 7;
		}
		
		spiralSearchLength++;		
	}	
	
	return false;
}
