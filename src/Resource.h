#pragma once

#define MAX_RESOURCES 40

#include <stdlib.h>
#include "avr/pgmspace.h"
#include "Entity.h"

void Resource_GetLocation(EntityID id, uint8_t* x, uint8_t* y);

void Resource_InitSystem();
uint8_t Resource_GetRemaining(EntityID id);
EntityID Resource_GetAtLocation(uint8_t x, uint8_t y);
EntityID Resource_FindClosest(uint8_t x, uint8_t y, uint8_t maxDistance);
void Resource_ReduceCount(EntityID id);