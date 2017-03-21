#pragma once

#include <stdlib.h>
#include "Entity.h"

#define MAX_RESOURCES 40

extern uint8_t ResourceState[MAX_RESOURCES >> 1];
extern uint8_t* ResourceLocations;

inline void Resource_GetLocation(EntityID id, uint8_t* x, uint8_t* y)
{
	if(id.type == Entity_Resource && id.id < MAX_RESOURCES)
	{
		*x = ResourceLocations[id.id * 2];
		*y = ResourceLocations[id.id * 2 + 1];
	}
	else
	{
		*x = 0xff;
		*y = 0xff;
	}
}

uint8_t Resource_GetRemaining(EntityID id);
EntityID Resource_GetAtLocation(uint8_t x, uint8_t y);
EntityID Resource_FindClosest(uint8_t x, uint8_t y, uint8_t maxDistance);
void Resource_ReduceCount(EntityID id);