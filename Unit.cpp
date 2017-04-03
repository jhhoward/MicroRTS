#include "Unit.h"
#include "Fog.h"

void Unit_InitSystem()
{
	for(uint8_t n = 0; n < MAX_UNITS; n++)
	{
		AllUnits[n].type = UnitType_Invalid;
	}
}

EntityID Unit_GetAtLocation(uint8_t x, uint8_t y)
{
	EntityID result;

	for(uint8_t n = 0; n < MAX_UNITS; n++)
	{
		Unit* unit = &AllUnits[n];
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
	
	uint8_t index = 0;
	
	while(index < MAX_UNITS)
	{
		Unit* unit = &AllUnits[index];
		if(unit->type == UnitType_Invalid)
		{
			const UnitTypeInfo* typeInfo = &AllUnitTypeInfo[type];
			
			unit->type = type;
			unit->team = team;
			unit->hp = typeInfo->hp;
			unit->offsetX = unit->offsetY = 0;
			unit->order = OrderType_None;
			unit->target.value = INVALID_ENTITY_VALUE;

			unit->agent.x = x;
			unit->agent.y = y;
			
			result.id = index;
			result.type = Entity_Unit;

			Fog_RevealBlock(unit->team, unit->agent.x, unit->agent.y, UNIT_SIGHT_DISTANCE);

			return result;
		}
		index ++;
	}	
	
	result.value = INVALID_ENTITY_VALUE;
	return result;
}

void Unit_Destroy(EntityID id)
{
	if(id.type == Entity_Unit && id.id < MAX_UNITS && AllUnits[id.id].type != UnitType_Invalid)
	{
		AllUnits[id.id].type = UnitType_Invalid;
		
		for(uint8_t n = 0; n < MAX_UNITS; n++)
		{
			if(AllUnits[n].target.value == id.value)
			{
				AllUnits[n].target.value = INVALID_ENTITY_VALUE;
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
