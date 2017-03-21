#include <stdint.h>
#include "avr/pgmspace.h"
#include "Map.h"

uint8_t* MacroMap;
uint8_t* MacroMapTileComposition;
uint8_t* MacroTileData;

uint8_t Map_GetTileType(uint8_t x, uint8_t y)
{
	uint8_t macroTileX = x >> 4;
	uint8_t macroTileY = y >> 4;

	// Find type of macro tile, e.g. horizontal road, vertical river with bridge, etc
	uint8_t macroTileType = pgm_read_byte(&MacroMap[macroTileY * MACRO_MAP_SIZE + macroTileX]);
	
	uint8_t tileX = x >> 1;
	uint8_t tileY = y >> 1;

	uint8_t* tileDataBase = MacroTileData + (macroTileType * MACRO_TILE_SIZE * MACRO_TILE_SIZE);
	return pgm_read_byte(&tileDataBase[tileY * MACRO_TILE_SIZE + tileX]);
}

uint8_t Map_GetDrawTile(uint8_t x, uint8_t y)
{
	uint8_t tileType = Map_GetTileType(x, y);
	uint8_t index = ((y & 1) << 1) + (x & 1);
	return pgm_read_byte(&MacroMapTileComposition[tileType * 4 + index]);
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
