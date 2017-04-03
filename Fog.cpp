#include <stdint.h>
#include "Map.h"

#define FOG_MAP_SIZE (MACRO_MAP_SIZE * MACRO_MAP_SIZE / 8)

uint8_t FogOfWar[2 * FOG_MAP_SIZE];

bool Fog_IsRevealed(uint8_t team, uint8_t x, uint8_t y)
{
	uint8_t* FogPtr = team == 0 ? FogOfWar : FogOfWar + FOG_MAP_SIZE;
	
	// Convert to macro map coordinates
	x >>= 3;
	y >>= 3;
	
	int index = y * MACRO_MAP_SIZE + x;
	uint8_t fogStatus = FogPtr[index >> 3];
	uint8_t bitCheckMask = 1 << (index & 7);
	return fogStatus & bitCheckMask;
}

void Fog_RevealTile(uint8_t team, uint8_t x, uint8_t y)
{
	uint8_t* FogPtr = team == 0 ? FogOfWar : FogOfWar + FOG_MAP_SIZE;
	
	// Convert to macro map coordinates
	x >>= 3;
	y >>= 3;
	
	int index = y * MACRO_MAP_SIZE + x;
	uint8_t bitMask = 1 << (index & 7);
	FogPtr[index >> 3] |= bitMask;
}

void Fog_RevealBlock(uint8_t team, uint8_t x, uint8_t y, uint8_t size)
{
	uint8_t x1 = x > size ? x - size : 0;
	uint8_t y1 = y > size ? y - size : 0;
	uint8_t x2 = x + size >= MAP_SIZE ? MAP_SIZE - 1 : x + size;
	uint8_t y2 = y + size >= MAP_SIZE ? MAP_SIZE - 1 : y + size;
	
	Fog_RevealTile(team, x1, y1);
	Fog_RevealTile(team, x2, y1);
	Fog_RevealTile(team, x2, y2);
	Fog_RevealTile(team, x1, y2);
}

void Fog_Init()
{
	for(int n = 0; n < 2 * FOG_MAP_SIZE; n++)
	{
		FogOfWar[n] = 0;
	}
}
