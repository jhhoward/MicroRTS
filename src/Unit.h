#pragma once

#include <stdint.h>
#include "Entity.h"
#include "Pathing.h"

#define MAX_UNITS 32
#define UNIT_SIGHT_DISTANCE 5

#define UNITS_UPDATED_PER_FRAME 4

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
	UnitState_Dead
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
	
	union
	{
		uint8_t stateData;
		
		// For move state:
		struct
		{
			int8_t offsetX : 4;
			int8_t offsetY : 4;
		};
		
		// For mining state:
		uint8_t miningProgress;
		
		// For attack state:
		uint8_t attackCooldown;
		EntityID attackTarget;
	};
	
	uint8_t hp;
	
	uint8_t state : 3;
	uint8_t frame : 3;
	uint8_t mirror : 1;
	uint8_t collectedResource : 1;

	PathingAgent agent;
	
	EntityID target;
} Unit;

extern const UnitTypeInfo AllUnitTypeInfo[Num_UnitTypes];

Unit* Unit_Get(EntityID id);
void Unit_InitSystem();
EntityID Unit_GetAtLocation(uint8_t x, uint8_t y);
EntityID Unit_Spawn(uint8_t team, uint8_t type, uint8_t x, uint8_t y);
void Unit_Destroy(EntityID unit);
bool Unit_IsAdjacentTo(Unit* unit, uint8_t x, uint8_t y);

void Unit_Kill(Unit* unit);

void Unit_Update(Unit* unit);

void Unit_OrderMove(Unit* unit, uint8_t x, uint8_t y);
void Unit_OrderAttack(Unit* unit, EntityID target);
void Unit_OrderBuildingInteraction(Unit* unit, EntityID target);
void Unit_OrderStop(Unit* unit);

EntityID Unit_FindClosestTarget(uint8_t targetTeam, uint8_t x, uint8_t y, uint8_t searchDistance);
