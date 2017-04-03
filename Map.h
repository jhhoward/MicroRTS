#pragma once

#include <stdint.h>

#define TILE_SIZE 8

#define MACRO_MAP_SIZE 12
#define MACRO_TILE_SIZE 8

#define MAP_SIZE (MACRO_MAP_SIZE * MACRO_TILE_SIZE)

#define MAX_RESOURCES 40

enum TileType
{
	TileType_Grass = 0,
	TileType_Path,
	TileType_Mud,
	TileType_Bridge,
	TileType_WaterEdge,
	TileType_Water,
	TileType_Forest,
	TileType_ForestEdge,
	Num_TileTypes,
	TileType_FirstWalkable = TileType_Grass,
	TileType_LastWalkable = TileType_Bridge
};

typedef struct
{
	uint8_t x, y;
} LocationType;

typedef struct
{
	uint8_t macroMap[MAP_SIZE * MAP_SIZE];
	LocationType resourceLocations[MAX_RESOURCES];
	LocationType playerStart[2];
} Map;

extern const Map* CurrentMap;

bool Map_IsWalkable(uint8_t x, uint8_t y);

uint8_t Map_GetTileType(uint8_t x, uint8_t y);

inline bool Map_IsBlocked(uint8_t x, uint8_t y)
{
	return !Map_IsWalkable(x, y);
}

bool Map_IsClear(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
bool Map_FindClearSpace(uint8_t* buildX, uint8_t* buildY, uint8_t clearWidth, uint8_t clearHeight);
