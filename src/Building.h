#pragma once

#include <stdint.h>
#include "Entity.h"

#define MAX_BUILDINGS 28
#define BUILDINGS_UPDATED_PER_FRAME 4

enum BuildType
{
	BuildType_None = 0,
	BuildType_Construct,
	BuildType_TrainPeasant,
	BuildType_TrainSoldier,
	BuildType_TrainArcher,
	BuildType_TrainMage,
	BuildType_Upgrade1,
	BuildType_Upgrade2,
};

enum BuildingType
{
	BuildingType_TownCenter = 0,
	BuildingType_House,
	BuildingType_Barracks,
	BuildingType_ArcheryRange,
	BuildingType_MageTower,
	BuildingType_DefenseTower,
	Num_BuildingTypes,
	BuildingType_Invalid = Num_BuildingTypes
};

typedef struct
{
	uint8_t goldCost;
	uint8_t* sprite;
	uint8_t width;
	uint8_t height;
	uint8_t hp;
	uint8_t prerequisiteMask;
	uint8_t buildTypeMask;
} BuildingTypeInfo;

typedef struct
{
	uint8_t team : 1;
	uint8_t type : 3;
	uint8_t buildType : 4;
	
	uint8_t x;
	uint8_t y;
	
	uint8_t hp;
	
	uint8_t buildProgress;
} Building;

extern const BuildingTypeInfo AllBuildingTypeInfo[Num_BuildingTypes];

Building* Building_Get(EntityID id);
void Building_InitSystem();
EntityID Building_GetAtLocation(uint8_t x, uint8_t y);
EntityID Building_Place(uint8_t team, uint8_t type, uint8_t x, uint8_t y);
void Building_Destroy(EntityID building);
bool Building_IsAdjacentTo(Building* building, uint8_t x, uint8_t y);
EntityID Building_FindClosestOfType(uint8_t type, uint8_t team, uint8_t x, uint8_t y);
void Building_Update(Building* building);
void Building_FindAdjacentSpace(Building* building, uint8_t* outX, uint8_t* outY);
void Building_TrainUnit(Building* building, uint8_t unitType);
void Building_Demolish(Building* building);
