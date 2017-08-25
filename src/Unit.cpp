#include "System.h"
#include "Unit.h"
#include "Fog.h"
#include "Game.h"

void Unit_InitSystem()
{
	for(uint8_t n = 0; n < MAX_UNITS; n++)
	{
		Game.Units[n].type = UnitType_Invalid;
	}
}

EntityID Unit_GetAtLocation(uint8_t x, uint8_t y)
{
	EntityID result;

	for(uint8_t n = 0; n < MAX_UNITS; n++)
	{
		Unit* unit = &Game.Units[n];
		if(unit->type != UnitType_Invalid && unit->agent.x == x && (unit->agent.y == y || unit->agent.y - 1 == y))
		{
			result.id = n;
			result.type = Entity_Unit;
			return result;
		}
	}
	
	result.value = INVALID_ENTITY_VALUE;
	return result;
}

EntityID Unit_Spawn(uint8_t team, uint8_t type, uint8_t x, uint8_t y)
{
	EntityID result;
	uint8_t foundIndex = 0xff;
	
	for(uint8_t n = 0; n < MAX_UNITS; n++)
	{
		if(Game.Units[n].type == UnitType_Invalid)
		{
			foundIndex = n;
			break;
		}
	}
	
	if(foundIndex == 0xff)
	{
		for(uint8_t n = 0; n < MAX_UNITS; n++)
		{
			if(Game.Units[n].type == 0)
			{
				foundIndex = n;
				break;
			}
		}
	}
	
	if(foundIndex == 0xff)
	{
		result.value = INVALID_ENTITY_VALUE;
		return result;
	}
		
	Unit* unit = &Game.Units[foundIndex];
	const UnitTypeInfo* typeInfo = &AllUnitTypeInfo[type];
	
	unit->type = type;
	unit->team = team;
	unit->hp = pgm_read_byte(&typeInfo->hp);
	unit->offsetX = unit->offsetY = 0;
	unit->order = OrderType_None;
	unit->state = UnitState_Idle;
	unit->target.value = INVALID_ENTITY_VALUE;

	unit->agent.x = x;
	unit->agent.y = y;
	
	result.id = foundIndex;
	result.type = Entity_Unit;

	Fog_RevealBlock(unit->team, unit->agent.x, unit->agent.y, UNIT_SIGHT_DISTANCE);

	LOG("%d: Unit spawned of type %d at %d %d\n", team, type, x, y);
		
	return result;
}

void Unit_Destroy(EntityID id)
{
	if(id.type == Entity_Unit && id.id < MAX_UNITS && Game.Units[id.id].type != UnitType_Invalid)
	{
		Game.Units[id.id].type = UnitType_Invalid;
		
		for(uint8_t n = 0; n < MAX_UNITS; n++)
		{
			if(Game.Units[n].target.value == id.value)
			{
				Game.Units[n].target.value = INVALID_ENTITY_VALUE;
			}
		}
	}
}

bool Unit_IsAdjacentTo(Unit* unit, uint8_t x, uint8_t y)
{
	if(unit->agent.x > 0 && x < unit->agent.x - 1)
		return false;
	if(unit->agent.y > 0 && y < unit->agent.y - 1)
		return false;
	if(x > unit->agent.x + 1)
		return false;
	if(y > unit->agent.y + 1)
		return false;
	return true;
}

Unit* Unit_Get(EntityID id)
{
	if(id.type == Entity_Unit && id.id < MAX_UNITS && Game.Units[id.id].type != UnitType_Invalid)
	{
		return &Game.Units[id.id];
	}
	return NULL;
}

