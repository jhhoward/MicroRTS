#include "avr/pgmspace.h"
#include "Unit.h"
#include "Building.h"
#include "Map.h"
#include "Pathing.h"
#include "Player.h"
#include "Resource.h"

#define UNIT_TRAINING_ORDER_SIZE (MAX_UNITS / 2)

uint8_t AI_UnitTrainOrder[UNIT_TRAINING_ORDER_SIZE] =
{
	UnitType_Peasant,	UnitType_Peasant,	UnitType_Peasant,	UnitType_Peasant,
	UnitType_Soldier,	UnitType_Soldier,	UnitType_Soldier,	UnitType_Archer,
	UnitType_Archer,	UnitType_Soldier,	UnitType_Soldier,	UnitType_Archer,
	UnitType_Mage,		UnitType_Soldier,	UnitType_Archer,	UnitType_Archer,
};

EntityID AI_FindSuitableUnit(uint8_t team, uint8_t unitType, uint8_t x, uint8_t y, bool includeBusy)
{
	EntityID result;
	result.value = INVALID_ENTITY_VALUE;
	uint8_t closestDistance = 0;
	
	for(uint8_t n = 0; n < MAX_UNITS; n++)
	{
		Unit* unit = &AllUnits[n];
		if(unit->team == team && unit->type == unitType && (unit->order == OrderType_None || unit->order == (OrderType_BuildingInteraction && unit->target.type == Entity_Resource)) )
		{
			uint8_t distance = Path_CalculateDistance(unit->agent.x, unit->agent.y, x, y);
			if(result.value == INVALID_ENTITY_VALUE || distance < closestDistance)
			{
				result.type = Entity_Unit;
				result.id = n;
				closestDistance = distance;
			}
		}
	}
	
	if(result.value == INVALID_ENTITY_VALUE && includeBusy)
	{
		for(uint8_t n = 0; n < MAX_UNITS; n++)
		{
			Unit* unit = &AllUnits[n];
			if(unit->team == team && unit->type == unitType)
			{
				uint8_t distance = Path_CalculateDistance(unit->agent.x, unit->agent.y, x, y);
				if(result.value == INVALID_ENTITY_VALUE || distance < closestDistance)
				{
					result.type = Entity_Unit;
					result.id = n;
					closestDistance = distance;
				}
			}
		}
	}
	
	return result;
}

void AI_ConstructBuilding(uint8_t team, uint8_t buildingType)
{
	// Check if player can afford to construct the building
	if(AllPlayers[team].gold < AllBuildingTypeInfo[buildingType].goldCost)
	{
		return;
	}
	
	// TODO: Check prerequisite mask
	
	uint8_t clearWidth = AllBuildingTypeInfo[buildingType].width + 2;
	uint8_t clearHeight = AllBuildingTypeInfo[buildingType].height + 2;
	
	uint8_t buildX = pgm_read_byte(&CurrentMap->playerStart[team].x) - (clearWidth >> 1);
	uint8_t buildY = pgm_read_byte(&CurrentMap->playerStart[team].y) - (clearHeight >> 1);
	
	if(!Map_FindClearSpace(&buildX, &buildY, clearWidth, clearHeight))
	{
		return;
	}
	
	bool isPriority = buildingType == BuildingType_TownCenter;
	EntityID builderUnit = AI_FindSuitableUnit(team, UnitType_Peasant, buildX, buildY, false);
	
	if(builderUnit.value != INVALID_ENTITY_VALUE)
	{
		EntityID newBuilding = Building_Place(team, buildingType, buildX + 1, buildY + 1);
		
		Unit* unit = Unit_Get(builderUnit);
		Unit_OrderBuildingInteraction(unit, newBuilding);
	}
}

void AI_TrainUnit(uint8_t team, uint8_t unitType)
{
	// Check if player can afford to train the unit
	if(AllPlayers[team].gold < AllUnitTypeInfo[unitType].goldCost)
	{
		return;
	}
	
	// Search for which building type is needed to train this unit
	uint8_t buildTypeMask = 1 << (BuildType_TrainPeasant + unitType);
	uint8_t requiredBuildingType = 0;
	
	while(requiredBuildingType < Num_BuildingTypes)
	{
		const BuildingTypeInfo* typeInfo = &AllBuildingTypeInfo[requiredBuildingType];
		
		if((typeInfo->buildTypeMask & buildTypeMask) != 0)
		{
			break;
		}
		requiredBuildingType++;
	}
	
	if(requiredBuildingType < Num_BuildingTypes)
	{
		for(uint8_t n = 0; n < MAX_BUILDINGS; n++)
		{
			Building* building = &AllBuildings[n];
			if(building->team == team && building->type == requiredBuildingType)
			{
				if(building->buildType == BuildType_None)
				{
					// TODO: Train unit
					
				}
				return;
			}
		}
		
		// No building constructed yet
		AI_ConstructBuilding(team, requiredBuildingType);
	}
}

void AI_Update(uint8_t team)
{
	// Build a new town center if it has been destroyed
	EntityID townCenter = Building_FindClosestOfType(BuildingType_TownCenter, team, 0, 0);

	if(townCenter.value == INVALID_ENTITY_VALUE)
	{
		AI_ConstructBuilding(team, BuildingType_TownCenter);
	}
	
	// Check all buildings under construction have peasants assigned
	// Repair buildings that have less than half HP left
	for(uint8_t n = 0; n < MAX_BUILDINGS; n++)
	{
		Building* building = &AllBuildings[n];
		if(building->type != BuildingType_Invalid && building->team == team)
		{
			const BuildingTypeInfo* buildingInfo = &AllBuildingTypeInfo[building->type];
			
			if(building->buildType == BuildType_Construct || building->hp < (buildingInfo->hp >> 1))
			{
				EntityID targetBuilding;
				targetBuilding.type = Entity_Building;
				targetBuilding.id = n;
				
				bool foundWorker = false;
				
				for(uint8_t u = 0; u < MAX_UNITS; u++)
				{
					if(AllUnits[u].team == team && AllUnits[u].type == UnitType_Peasant && AllUnits[u].order == OrderType_BuildingInteraction && AllUnits[u].target.value == targetBuilding.value)
					{
						foundWorker = true;
						break;
					}
				}
				
				if(!foundWorker)
				{
					bool isPriority = building->buildType == BuildType_Construct && building->type == BuildingType_TownCenter;
					
					EntityID worker = AI_FindSuitableUnit(team, UnitType_Peasant, building->x, building->y, isPriority);
					if(worker.value != INVALID_ENTITY_VALUE)
					{
						Unit_OrderBuildingInteraction(Unit_Get(worker), targetBuilding);
					}
				}
				else
				{
					break;
				}
			}
		}
	}
	
	// Keep peasants busy mining
	for(uint8_t n = 0; n < MAX_UNITS; n++)
	{
		Unit* unit = &AllUnits[n];
		if(unit->type == UnitType_Peasant && unit->team == team && unit->order == OrderType_None)
		{
			EntityID targetResource = Resource_FindClosest(unit->agent.x, unit->agent.y, 127);
			if(targetResource.value != INVALID_ENTITY_VALUE)
			{
				Unit_OrderBuildingInteraction(unit, targetResource);
			}
		}
	}
	
	// Train new units
	{
		uint8_t unitByTypeCount[Num_UnitTypes];
		uint8_t requiredByTypeCount[Num_UnitTypes];
		
		for(uint8_t n = 0; n < Num_UnitTypes; n++)
		{
			unitByTypeCount[n] = 0;
			requiredByTypeCount[n] = 0;
		}
		
		for(uint8_t n = 0; n < MAX_UNITS; n++)
		{
			Unit* unit = &AllUnits[n];
			if(unit->type < Num_UnitTypes && unit->team == team)
			{
				unitByTypeCount[unit->type]++;
			}
		}
		
		for(uint8_t n = 0; n < UNIT_TRAINING_ORDER_SIZE; n++)
		{
			uint8_t unitTrainType = pgm_read_byte(&AI_UnitTrainOrder[n]);
			requiredByTypeCount[unitTrainType]++;
			
			if(requiredByTypeCount[unitTrainType] > unitByTypeCount[unitTrainType])
			{
				AI_TrainUnit(team, unitTrainType);
				break;
			}
		}
	}
}
