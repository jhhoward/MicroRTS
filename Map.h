#pragma once

#define TILE_SIZE 8

#define MAP_SIZE 192

#define MACRO_MAP_SIZE 12

#define MACRO_TILE_SIZE 8

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

uint8_t Map_GetDrawTile(uint8_t x, uint8_t y);
bool Map_IsWalkable(uint8_t x, uint8_t y);

inline bool Map_IsBlocked(uint8_t x, uint8_t y)
{
	return !Map_IsWalkable(x, y);
}

bool Map_IsClear(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
