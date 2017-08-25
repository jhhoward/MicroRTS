#include "System.h"
#include "Entity.h"
#include "Unit.h"
#include "Pathing.h"
#include "Building.h"
#include "Resource.h"
#include "Player.h"
#include "Fog.h"
#include "Game.h"

#define UNIT_MINING_LOOPS 8
#define UNIT_ATTACK_FRAMES 4
#define UNIT_REPAIR_FRAMES 4
#define UNIT_DYING_FRAMES 4
#define UNIT_MINE_FRAMES 4
#define UNIT_MOVE_FRAMES 4
#define RESOURCE_SEARCH_DISTANCE 16			// How far to search for alternative resource
#define RESOURCE_COLLECTION_AMOUNT 10		// How much is collected per mining trip
#define UNIT_ATTACK_COOLDOWN_TIME 10

inline void Unit_SwitchState(Unit* unit, uint8_t newState)
{
	unit->state = newState;
	unit->frame = 0;
	unit->stateData = 0;
}

inline bool Unit_CanInterruptState(uint8_t stateType)
{
	return stateType == UnitState_Constructing || stateType == UnitState_Mining || stateType == UnitState_Repairing;
}

void Unit_Kill(Unit* unit)
{
	unit->hp = 0;
	Unit_SwitchState(unit, UnitState_Dead);
}

EntityID Unit_FindClosestTarget(uint8_t targetTeam, uint8_t x, uint8_t y, uint8_t searchDistance)
{
	EntityID result;
	result.value = INVALID_ENTITY_VALUE;
	int closestDistance = 0;
	
	for(uint8_t n = 0; n < MAX_UNITS; n++)
	{
		Unit* unit = &Game.Units[n];
		if(unit->type != UnitType_Invalid && unit->team == targetTeam && unit->hp > 0)
		{
			int distance = Path_CalculateDistance(x, y, unit->agent.x, unit->agent.y);
			if(distance < searchDistance && (result.value == INVALID_ENTITY_VALUE || distance < closestDistance))
			{
				result.type = Entity_Unit;
				result.id = n;
				closestDistance = distance;
			}
		}
	}
	
	for(uint8_t n = 0; n < MAX_BUILDINGS; n++)
	{
		Building* building = &Game.Buildings[n];
		if(building->type != BuildingType_Invalid && building->team == targetTeam && building->hp > 0)
		{
			// TODO: better distance calculation for buildings
			int distance = Path_CalculateDistance(x, y, building->x, building->y);
			if(distance < searchDistance && (result.value == INVALID_ENTITY_VALUE || distance < closestDistance))
			{
				result.type = Entity_Building;
				result.id = n;
				closestDistance = distance;
			}
		}
	}
	
	return result;
}

void Unit_InflictDamage(Unit* unit, EntityID target)
{
	const UnitTypeInfo* typeInfo = &AllUnitTypeInfo[unit->type];
	uint8_t attackStrength = pgm_read_byte(&typeInfo->attackStrength);
	
	LOG("Attacking %x with attack of %d\n", target.value, attackStrength);
	
	if(target.type == Entity_Building)
	{
		attackStrength >>= 2;
		if(attackStrength == 0)
			attackStrength = 1;
		
		Building* targetBuilding = Building_Get(target);
		if(targetBuilding)
		{
			if(attackStrength >= targetBuilding->hp)
			{
				Building_Demolish(targetBuilding);
			}
			else
			{
				targetBuilding->hp -= attackStrength;
			}
		}
	}
	else if(target.type == Entity_Unit)
	{
		Unit* targetUnit = Unit_Get(target);
		
		if(targetUnit)
		{
			if(attackStrength >= targetUnit->hp)
			{
				Unit_Kill(targetUnit);
			}
			else
			{
				targetUnit->hp -= attackStrength;
			}
		}
	}
}

void Unit_OrderMove(Unit* unit, uint8_t x, uint8_t y)
{
	if(Unit_CanInterruptState(unit->state))
	{
		Unit_SwitchState(unit, UnitState_Idle);
	}
	unit->order = OrderType_Move;
	unit->target.value = INVALID_ENTITY_VALUE;
	Agent_PathTo(&unit->agent, x, y);
}

void Unit_OrderAttack(Unit* unit, EntityID target)
{
	if(Unit_CanInterruptState(unit->state))
	{
		Unit_SwitchState(unit, UnitState_Idle);
	}
	unit->order = OrderType_Attack;
	unit->target = target;
	Agent_StopPathing(&unit->agent);
}

void Unit_OrderBuildingInteraction(Unit* unit, EntityID target)
{
	if(Unit_CanInterruptState(unit->state))
	{
		Unit_SwitchState(unit, UnitState_Idle);
	}
	unit->order = OrderType_BuildingInteraction;
	unit->target = target;
	Agent_StopPathing(&unit->agent);
}

void Unit_OrderStop(Unit* unit)
{
	if(Unit_CanInterruptState(unit->state))
	{
		Unit_SwitchState(unit, UnitState_Idle);
	}
	unit->order = OrderType_None;
	unit->target.value = INVALID_ENTITY_VALUE;
	Agent_StopPathing(&unit->agent);
}

bool Unit_PathToTarget(Unit* unit, EntityID target)
{
	if(target.type == Entity_Building)
	{
		Building* targetBuilding = Building_Get(target);
		if(targetBuilding)
		{
			if(Building_IsAdjacentTo(targetBuilding, unit->agent.x, unit->agent.y))
			{
				Agent_StopPathing(&unit->agent);
				return true;
			}
			
			const BuildingTypeInfo* buildingInfo = &AllBuildingTypeInfo[targetBuilding->type];
			
			uint8_t buildingWidth = pgm_read_byte(&buildingInfo->width);
			uint8_t buildingHeight = pgm_read_byte(&buildingInfo->height);
			
			if(!Agent_IsPathing(&unit->agent) || unit->agent.path.targetX < targetBuilding->x - 1 || unit->agent.path.targetY < targetBuilding->y - 1
			|| unit->agent.path.targetX > targetBuilding->x + buildingWidth || unit->agent.path.targetY > targetBuilding->y + buildingHeight)
			{
				uint8_t targetX = unit->agent.x;
				uint8_t targetY = unit->agent.y;
				if(targetBuilding->x > 0 && targetX < targetBuilding->x - 1)
				{
					targetX = targetBuilding->x - 1;
				}
				else if(targetX > targetBuilding->x + buildingWidth)
				{
					targetX = targetBuilding->x + buildingWidth;
				}
				if(targetBuilding->y > 0 && targetY < targetBuilding->y - 1)
				{
					targetY = targetBuilding->y - 1;
				}
				else if(targetY > targetBuilding->y + buildingHeight)
				{
					targetY = targetBuilding->y + buildingHeight;
				}
				
				Agent_PathTo(&unit->agent, targetX, targetY);
			}
		}
	}
	else if(target.type == Entity_Unit)
	{
		Unit* targetUnit = Unit_Get(target);
		if(targetUnit)
		{
			if(Unit_IsAdjacentTo(unit, targetUnit->agent.x, targetUnit->agent.y))
			{
				Agent_StopPathing(&unit->agent);
				return true;
			}
			
			if(!Agent_IsPathing(&unit->agent) || (unit->agent.pathingState != Pathing_FollowLeftWall && unit->agent.pathingState != Pathing_FollowRightWall && (unit->agent.path.targetX != targetUnit->agent.x || unit->agent.path.targetY != targetUnit->agent.y)))
			{
				Agent_PathTo(&unit->agent, targetUnit->agent.x, targetUnit->agent.y);
			}
		}
	}
	else if(target.type == Entity_Resource)
	{
		uint8_t targetX, targetY;
		
		Resource_GetLocation(target, &targetX, &targetY);
		
		if(Unit_IsAdjacentTo(unit, targetX, targetY))
		{
			Agent_StopPathing(&unit->agent);
			LOG("%d : Arrived at resource %d %d\n", unit->team, targetX, targetY);
			return true;
		}
		
		if(!Agent_IsPathing(&unit->agent) || unit->agent.path.targetX != targetX || unit->agent.path.targetY != targetY)
		{
			Agent_PathTo(&unit->agent, targetX, targetY);
			//LOG("%d : Pathing to resource %d %d -> %d %d\n", unit->team, unit->agent.x, unit->agent.y, targetX, targetY);
		}
	}
	else
	{
		Agent_StopPathing(&unit->agent);
	}
	
	return false;
}

bool IsTargetAlive(EntityID target)
{
	if(target.type == Entity_Unit)
	{
		return Game.Units[target.id].type != UnitType_Invalid && Game.Units[target.id].hp > 0;
	}
	else if(target.type == Entity_Building)
	{
		return Game.Buildings[target.id].type != BuildingType_Invalid && Game.Buildings[target.id].hp > 0;
	}
	return false;
}

void Unit_Update(Unit* unit)
{
	if(unit->hp == 0 && unit->state != UnitState_Dead)
	{
		Unit_SwitchState(unit, UnitState_Dead);
	}
	
	switch(unit->state)
	{
		case UnitState_Dead:
		{
			if(unit->frame < UNIT_DYING_FRAMES - 1)
			{
				unit->frame ++;
			}

			return;
		}
		case UnitState_Moving:
		{
			unit->frame ++;
			if(unit->frame >= UNIT_MOVE_FRAMES)
			{
				unit->frame = 0;
			}
			
			if(unit->offsetX < 0)
			{
				unit->offsetX++;
			}
			else if(unit->offsetX > 0)
			{
				unit->offsetX--;
			}
			if(unit->offsetY < 0)
			{
				unit->offsetY++;
			}
			else if(unit->offsetY > 0)
			{
				unit->offsetY--;
			}
			
			if(unit->offsetX != 0 || unit->offsetY != 0)
			{
				return;
			}

			Fog_RevealBlock(unit->team, unit->agent.x, unit->agent.y, UNIT_SIGHT_DISTANCE);
			
			if(!Agent_IsPathing(&unit->agent))
			{
				Unit_SwitchState(unit, UnitState_Idle);
			}
		}
		break;
		
		case UnitState_Constructing:
		case UnitState_Repairing:
		{
			Building* targetBuilding = Building_Get(unit->target);
			if(targetBuilding && targetBuilding->hp > 0)
			{
				const BuildingTypeInfo* buildingInfo = &AllBuildingTypeInfo[targetBuilding->type];
				if((unit->state == UnitState_Repairing || targetBuilding->buildType == BuildType_Construct) && targetBuilding->hp < buildingInfo->hp)
				{
					if(!Building_IsAdjacentTo(targetBuilding, unit->agent.x, unit->agent.y))
					{
						Unit_SwitchState(unit, UnitState_Idle);
					}
					else
					{
						unit->frame ++;
						
						if(unit->frame < UNIT_REPAIR_FRAMES)
						{
							return;
						}
						else
						{
							unit->frame = 0;
							// TODO: proper repairing / constructing logic
							targetBuilding->hp ++;
							
							if(targetBuilding->buildType == BuildType_Construct)
							{
								if(targetBuilding->hp == buildingInfo->hp)
								{
									targetBuilding->buildType = BuildType_None;
								}
							}
						}
					}
				}
				else
				{
					Unit_OrderStop(unit);
				}
			}
			else
			{
				Unit_OrderStop(unit);
			}
		}
		break;
		
		case UnitState_Attacking:
		{
			if(unit->frame == UNIT_ATTACK_FRAMES)
			{
				if(unit->attackCooldown > 0)
				{
					unit->attackCooldown--;
					return;
				}
				else
				{
					Unit_SwitchState(unit, UnitState_Idle);
				}
			}
			else if(!IsEntityValid(unit->attackTarget))
			{
				Unit_OrderStop(unit);
				Unit_SwitchState(unit, UnitState_Idle);
			}
			else
			{
				unit->frame++;
				
				if(unit->frame < UNIT_ATTACK_FRAMES)
				{
					return;
				}

				// TODO: ranged combat
				Unit_InflictDamage(unit, unit->attackTarget);
				unit->attackCooldown = UNIT_ATTACK_COOLDOWN_TIME;
			}
		}
		break;
		
		case UnitState_Mining:
		{
			if(!IsEntityValid(unit->target))
			{
				Unit_OrderStop(unit);
			}
			else
			{
				uint8_t resourceX, resourceY;
				Resource_GetLocation(unit->target, &resourceX, &resourceY);

				if(!Resource_GetRemaining(unit->target))
				{
					unit->target = Resource_FindClosest(resourceX, resourceY, RESOURCE_SEARCH_DISTANCE);
					Unit_SwitchState(unit, UnitState_Idle);
				}
				else if(!Unit_IsAdjacentTo(unit, resourceX, resourceY))
				{
					Unit_SwitchState(unit, UnitState_Idle);
				}
				else 
				{
					unit->frame++;
					
					if(unit->frame < UNIT_MINE_FRAMES)
					{
						return;
					}
					
					unit->frame = 0;
					if(unit->miningProgress >= UNIT_MINING_LOOPS)
					{
						LOG("%d Mined resource %d\n", unit->team, unit->target.id);
						Resource_ReduceCount(unit->target);
						Unit_SwitchState(unit, UnitState_Idle);
						unit->collectedResource = 1;
					}
					else
					{
						unit->miningProgress++;
						return;
					}
				}
			}
		}
		break;
	}

	switch(unit->order)
	{
		case OrderType_None:
		// TODO: check for nearby threats and attack
		break;
		case OrderType_Move:
		{
			if(!Agent_IsPathing(&unit->agent))
			{
				unit->order = OrderType_None;
			}
		}
		break;
		case OrderType_BuildingInteraction:
		{
			// Depending on the target type can be:
			// - Constructing a building
			// - Repairing a building
			// - Mining a resource
			if(IsEntityValid(unit->target))
			{
				if(unit->target.type == Entity_Building)
				{
					Building* targetBuilding = Building_Get(unit->target);
					const BuildingTypeInfo* targetBuildingInfo = &AllBuildingTypeInfo[targetBuilding->type];
					
					if(targetBuilding->buildType == BuildType_Construct)
					{
						if(Unit_PathToTarget(unit, unit->target))
						{
							Unit_SwitchState(unit, UnitState_Constructing);
						}
					}
					else if(targetBuilding->hp < targetBuildingInfo->hp)
					{
						if(Unit_PathToTarget(unit, unit->target))
						{
							Unit_SwitchState(unit, UnitState_Repairing);
						}
					}
					else
					{
						Unit_OrderStop(unit);
					}
				}
				else if(unit->target.type == Entity_Resource)
				{
					if(unit->collectedResource)
					{
						EntityID closestTownCenter = Building_FindClosestOfType(BuildingType_TownCenter, unit->team, unit->agent.x, unit->agent.y);
						
						if(Unit_PathToTarget(unit, closestTownCenter))
						{
							unit->collectedResource = 0;
							Game.Players[unit->team].gold += RESOURCE_COLLECTION_AMOUNT;
							LOG("%d Collected resource\n", unit->team);
						}
					}
					else
					{
						uint8_t resourceX, resourceY;
						Resource_GetLocation(unit->target, &resourceX, &resourceY);
						
						if(Resource_GetRemaining(unit->target) > 0)
						{
							if(Unit_PathToTarget(unit, unit->target))
							{
								LOG("%d Begin mining\n", unit->team);
								Unit_SwitchState(unit, UnitState_Mining);
							}
							else if(unit->agent.pathingState == Pathing_Failed)
							{
								unit->target = Resource_FindClosest(unit->agent.x, unit->agent.y, RESOURCE_SEARCH_DISTANCE);
							}
						}
						else
						{
							unit->target = Resource_FindClosest(resourceX, resourceY, RESOURCE_SEARCH_DISTANCE);
						}
					}
				}
			}
			else
			{
				Unit_OrderStop(unit);
			}
		}
		break;
		case OrderType_Attack:
		{
			if(IsEntityValid(unit->target) && IsTargetAlive(unit->target))
			{
				if(Unit_PathToTarget(unit, unit->target))
				{
					Unit_SwitchState(unit, UnitState_Attacking);
					unit->attackTarget = unit->target;
				}
			}
			else
			{
				Unit_OrderStop(unit);
			}
		}
		break;
	}

	{
		uint8_t oldX = unit->agent.x;
		uint8_t oldY = unit->agent.y;
		
		Agent_UpdatePathing(&unit->agent);
		
		if(oldX != unit->agent.x || oldY != unit->agent.y)
		{
//			LOG("%d : unit moved to %d %d\n", unit->team, unit->agent.x, unit->agent.y);
			if(oldX < unit->agent.x)
			{
				unit->offsetX = -8;
			}
			else if(oldX > unit->agent.x)
			{
				unit->offsetX = 8;
			}
			if(oldY < unit->agent.y)
			{
				unit->offsetY = -8;
			}
			else if(oldY > unit->agent.y)
			{
				unit->offsetY = 8;
			}
			Unit_SwitchState(unit, UnitState_Moving);
		}
	}
}
