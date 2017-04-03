#pragma once

#include <stdlib.h>
#include "avr/pgmspace.h"
#include "Entity.h"
#include "Map.h"

extern uint8_t ResourceState[MAX_RESOURCES >> 1];

inline void Resource_GetLocation(EntityID id, uint8_t* x, uint8_t* y)
{
	if(id.type == Entity_Resource && id.id < MAX_RESOURCES)
	{
		*x = pgm_read_byte(&CurrentMap->resourceLocations[id.id]);
		*y = pgm_read_byte(&CurrentMap->resourceLocations[id.id]);
	}
	else
	{
		*x = 0xff;
		*y = 0xff;
	}
}

void Resource_InitSystem();
uint8_t Resource_GetRemaining(EntityID id);
EntityID Resource_GetAtLocation(uint8_t x, uint8_t y);
EntityID Resource_FindClosest(uint8_t x, uint8_t y, uint8_t maxDistance);
void Resource_ReduceCount(EntityID id);