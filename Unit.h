#pragma once

#include <stdint.h>
#include "Entity.h"
#include "Pathing.h"

#define MAX_UNITS 32

enum UnitType
{
	UnitType_Peasant,
	UnitType_Soldier,
	UnitType_Archer,
	UnitType_Mage,
	Num_UnitTypes,
	UnitType_Invalid = Num_UnitTypes
};

enum OrderType
{
	OrderType_None,
	OrderType_Move,
	OrderType_BuildingInteraction,
	OrderType_Attack
};

enum UnitState
{
	UnitState_Idle,
	UnitState_Moving,
	UnitState_Attacking,
	UnitState_Mining,
	UnitState_Constructing,
	UnitState_Repairing,
};

typedef struct
{
	uint8_t goldCost;
	uint8_t* sprite;
	
	uint8_t hp;
	uint8_t attackStrength;
	uint8_t abilityMask;
} UnitTypeInfo;

typedef struct
{
	uint8_t type : 3;
	uint8_t team : 1;
	uint8_t order : 2;
	
	int8_t offsetX : 4;
	int8_t offsetY : 4;
	
	uint8_t hp;
	
	uint8_t state : 3;
	uint8_t frame : 4;
	uint8_t collectedResource : 1;

	PathingAgent agent;
	
	EntityID target;
} Unit;

extern UnitTypeInfo AllUnitTypeInfo[Num_UnitTypes];
extern Unit AllUnits[MAX_UNITS];

inline Unit* Unit_Get(EntityID id)
{
	if(id.type == Entity_Unit && id.id < MAX_UNITS && AllUnits[id.id].type != UnitType_Invalid)
	{
		return &AllUnits[id.id];
	}
	return NULL;
}

void Unit_InitSystem();
EntityID Unit_GetAtLocation(uint8_t x, uint8_t y);
EntityID Unit_Spawn(uint8_t team, uint8_t type, uint8_t x, uint8_t y);
void Unit_Destroy(EntityID unit);
bool Unit_IsAdjacentTo(Unit* unit, uint8_t x, uint8_t y);

