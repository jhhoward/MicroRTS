#include "Entity.h"
#include "Building.h"
#include "Unit.h"
#include "Resource.h"
#include "Game.h"

bool IsEntityValid(EntityID id)
{
	if(id.value == INVALID_ENTITY_VALUE)
		return false;
	if(id.type == Entity_Building && id.id < MAX_BUILDINGS && Game.Buildings[id.id].type != BuildingType_Invalid)
		return true;
	if(id.type == Entity_Unit && id.id < MAX_UNITS && Game.Units[id.id].type != UnitType_Invalid)
		return true;
	if(id.type == Entity_Resource && id.id < MAX_RESOURCES)
		return true;
			
	return false;
}
