#include "Building.h"
#include "Unit.h"
#include "Pathing.h"
#include "Map.h"
#include "Game.h"

#define MAX_BUILD_PROGRESS 128

void Building_InitSystem()
{
	for(uint8_t n = 0; n < MAX_BUILDINGS; n++)
	{
		Game.Buildings[n].type = BuildingType_Invalid;
	}
}

EntityID Building_GetAtLocation(uint8_t x, uint8_t y)
{
	EntityID result;
	
	uint8_t index = 0;
	
	while(index < MAX_BUILDINGS)
	{
		Building* building = &Game.Buildings[index];
		if(building->type != BuildingType_Invalid)
		{
			const BuildingTypeInfo* typeInfo = &AllBuildingTypeInfo[building->type];
			if(x >= building->x && y >= building->y && x < building->x + typeInfo->width && y < building->y + typeInfo->height)
			{
				result.type = Entity_Building;
				result.id = index;
				return result;
			}
		}
		index++;
	}
	
	result.value = INVALID_ENTITY_VALUE;
	return result;
}

EntityID Building_Place(uint8_t team, uint8_t type, uint8_t x, uint8_t y)
{
	EntityID result;
	
	uint8_t index = 0;
	
	while(index < MAX_BUILDINGS)
	{
		Building* building = &Game.Buildings[index];
		if(building->type == BuildingType_Invalid)
		{
			building->type = type;
			building->x = x;
			building->y = y;
			building->hp = 1;
			building->team = team;
			building->buildType = BuildType_Construct;
			building->buildProgress = 0;
			
			result.id = index;
			result.type = Entity_Building;
			return result;
		}
		index ++;
	}	
	
	result.value = INVALID_ENTITY_VALUE;
	return result;
}

void Building_Destroy(EntityID id)
{
	if(id.type == Entity_Building && id.id < MAX_BUILDINGS && Game.Buildings[id.id].type != BuildingType_Invalid)
	{
		Game.Buildings[id.id].type = BuildingType_Invalid;
		
		for(uint8_t n = 0; n < MAX_UNITS; n++)
		{
			if(Game.Units[n].target.value == id.value)
			{
				Game.Units[n].target.value = INVALID_ENTITY_VALUE;
			}
		}
	}
}

bool Building_IsAdjacentTo(Building* building, uint8_t x, uint8_t y)
{
	const BuildingTypeInfo* typeInfo = &AllBuildingTypeInfo[building->type];
	
	if(building->x > 0 && x < building->x - 1)
		return false;
	if(building->y > 0 && y < building->y - 1)
		return false;
	if(x > building->x + typeInfo->width + 1)
		return false;
	if(y > building->y + typeInfo->height + 1)
		return false;
	return true;
}

EntityID Building_FindClosestOfType(uint8_t type, uint8_t team, uint8_t x, uint8_t y)
{
	EntityID result;
	
	result.value = INVALID_ENTITY_VALUE;
	
	uint8_t index = 0;
	int minDistance = 0;
	
	while(index < MAX_BUILDINGS)
	{
		Building* building = &Game.Buildings[index];
		if(building->type == type && building->team == team)
		{
			int distance = Path_CalculateDistance(building->x, building->y, x, y);
			
			if(result.value == INVALID_ENTITY_VALUE || distance < minDistance)
			{
				minDistance = distance;
				result.type = Entity_Building;
				result.id = index;
			}
		}
		index++;
	}
	
	return result;
}

void Building_FindAdjacentSpace(Building* building, uint8_t* outX, uint8_t* outY)
{
	const BuildingTypeInfo* info = &AllBuildingTypeInfo[building->type];
	
	for(uint8_t ring = 0; ring < 64; ring++)
	{
		for(int x = building->x - ring; x < building->x + info->width + ring; x++)
		{
			if(x >= 0 && x < MAP_SIZE)
			{
				if(building->y - ring - 1 >= 0 && Map_IsWalkable(x, building->y - ring - 1))
				{
					*outX = x;
					*outY = building->y - ring - 1;
					return;
				}
				if(building->y + ring + info->height < MAP_SIZE && Map_IsWalkable(x, building->y + ring + info->height))
				{
					*outX = x;
					*outY = building->y + ring + info->height;
					return;
				}
			}
		}
		for(int y = building->y - ring - 1; y <= building->y + info->height + ring + 1; y++)
		{
			if(y >= 0 && y < MAP_SIZE)
			{
				if(building->x - ring - 1 >= 0 && Map_IsWalkable(building->x - ring - 1, y))
				{
					*outX = building->x - ring - 1;
					*outY = y;
					return;
				}
				if(building->x + info->width + ring + 1 < MAP_SIZE && Map_IsWalkable(building->x + info->width + ring + 1, y))
				{
					*outX = building->x + ring + info->width + 1;
					*outY = y;
					return;
				}
			}
		}
	}
}

void Building_Update(Building* building)
{
	if(building->buildType > BuildType_Construct)
	{
		building->buildProgress++;
		
		if(building->buildProgress >= MAX_BUILD_PROGRESS)
		{
			switch(building->buildType)
			{
				case BuildType_TrainPeasant:
				case BuildType_TrainSoldier:
				case BuildType_TrainArcher:
				case BuildType_TrainMage:
				{
					uint8_t unitType = building->buildType - BuildType_TrainPeasant;
					uint8_t spawnX, spawnY;
					
					Building_FindAdjacentSpace(building, &spawnX, &spawnY);
					
					Unit_Spawn(building->team, unitType, spawnX, spawnY);
				}
				break;
				case BuildType_Upgrade1:
				case BuildType_Upgrade2:
				{
					// TODO
				}
				break;
			}
			
			building->buildType = BuildType_None;
		}
		
		building->buildProgress = 0;
	}
}

Building* Building_Get(EntityID id)
{
	if(id.type == Entity_Building && id.id < MAX_BUILDINGS && Game.Buildings[id.id].type != BuildingType_Invalid)
	{
		return &Game.Buildings[id.id];
	}
	return NULL;
}
