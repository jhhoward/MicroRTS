#include "System.h"
#include "Resource.h"
#include "Pathing.h"
#include "Map.h"
#include "Game.h"

void Resource_InitSystem()
{
	for(uint8_t n = 0; n < MAX_RESOURCES / 2; n++)
	{
		Game.Resources[n] = 0xff;
	}
}

uint8_t Resource_GetRemaining(EntityID id)
{
	if(id.type == Entity_Resource && id.id < MAX_RESOURCES)
	{
		uint8_t state = Game.Resources[id.id >> 1];
		
		if(id.id & 1)
		{
			return state >> 4;
		}
		else
		{
			return state & 0xf;
		}
	}
	
	return 0;	
}

void Resource_ReduceCount(EntityID id)
{
	if(id.type == Entity_Resource && id.id < MAX_RESOURCES)
	{
		uint8_t index = id.id >> 1;
		uint8_t state = Game.Resources[index];
		
		if(id.id & 1)
		{ 
			if(state & 0xf0)
			{
				Game.Resources[index] = (state & 0x0f) | ((state & 0xf0) - 0x10);
			}
		}
		else
		{
			if(state & 0x0f)
			{
				Game.Resources[index] = (state & 0xf0) | ((state & 0x0f) - 0x01);
			}
		}
		
		LOG("Resource %d reduced to %d\n", id.id, Resource_GetRemaining(id));
	}
}

EntityID Resource_GetAtLocation(uint8_t x, uint8_t y)
{
	EntityID found;
	found.value = 0xff;
	
	uint8_t resourceIndex = 0;
	
	while(resourceIndex < MAX_RESOURCES)
	{
		if(x == pgm_read_byte(&Game.Map->resourceLocations[resourceIndex].x)
			&& y == pgm_read_byte(&Game.Map->resourceLocations[resourceIndex].y))
		{
			found.id = resourceIndex;
			found.type = Entity_Resource;
			return found;
		}
		
		resourceIndex++;
	}
	
	return found;
}

EntityID Resource_FindClosest(uint8_t x, uint8_t y, uint8_t maxDistance)
{
	uint8_t index = 0;
	EntityID result;
	
	result.value = INVALID_ENTITY_VALUE;
	int closestDistance = 0;
	
	while(index < MAX_RESOURCES)
	{
		EntityID check;
		check.type = Entity_Resource;
		check.id = index;
		
		if(Resource_GetRemaining(check))
		{
			uint8_t checkX, checkY;
			
			Resource_GetLocation(check, &checkX, &checkY);
			
			int distance = Path_CalculateDistance(x, y, checkX, checkY);
			
			if(distance < maxDistance)
			{				
				if(result.value == INVALID_ENTITY_VALUE || distance < closestDistance)
				{
					result.value = check.value;
					closestDistance = distance;
				}
			}
		}
		
		index++;
	}
	
	return result;
}

inline void Resource_GetLocation(EntityID id, uint8_t* x, uint8_t* y)
{
	if(id.type == Entity_Resource && id.id < MAX_RESOURCES)
	{
		*x = pgm_read_byte(&Game.Map->resourceLocations[id.id].x);
		*y = pgm_read_byte(&Game.Map->resourceLocations[id.id].y);
	}
	else
	{
		*x = 0xff;
		*y = 0xff;
	}
}
