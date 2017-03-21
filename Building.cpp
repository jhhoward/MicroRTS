#include "Building.h"
#include "Unit.h"
#include "Pathing.h"

void Building_InitSystem()
{
	for(uint8_t n = 0; n < MAX_BUILDINGS; n++)
	{
		AllBuildings[n].type = BuildingType_Invalid;
	}
}

EntityID Building_GetAtLocation(uint8_t x, uint8_t y)
{
	EntityID result;
	
	uint8_t index = 0;
	
	while(index < MAX_BUILDINGS)
	{
		Building* building = &AllBuildings[index];
		if(building->type != BuildingType_Invalid)
		{
			BuildingTypeInfo* typeInfo = &AllBuildingTypeInfo[building->type];
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
		Building* building = &AllBuildings[index];
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
	if(id.type == Entity_Building && id.id < MAX_BUILDINGS && AllBuildings[id.id].type != BuildingType_Invalid)
	{
		AllBuildings[id.id].type = BuildingType_Invalid;
		
		for(uint8_t n = 0; n < MAX_UNITS; n++)
		{
			if(AllUnits[n].target.value == id.value)
			{
				AllUnits[n].target.value = INVALID_ENTITY_VALUE;
			}
		}
	}
}

bool Building_IsAdjacentTo(Building* building, uint8_t x, uint8_t y)
{
	BuildingTypeInfo* typeInfo = &AllBuildingTypeInfo[building->type];
	
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
		Building* building = &AllBuildings[index];
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
