#include "Entity.h"
#include "Unit.h"
#include "Pathing.h"
#include "Building.h"
#include "Resource.h"
#include "Player.h"

#define UNIT_ATTACK_FRAMES 4
#define UNIT_REPAIR_FRAMES 4
#define UNIT_MINE_FRAMES 4
#define UNIT_MOVE_FRAMES 4
#define RESOURCE_SEARCH_DISTANCE 16			// How far to search for alternative resource
#define RESOURCE_COLLECTION_AMOUNT 10		// How much is collected per mining trip

inline void Unit_SwitchState(Unit* unit, uint8_t newState)
{
	unit->state = newState;
	unit->frame = 0;
}

void Unit_OrderMove(Unit* unit, uint8_t x, uint8_t y)
{
	if(unit->state != UnitState_Moving)
	{
		Unit_SwitchState(unit, UnitState_Idle);
	}
	unit->order = OrderType_Move;
	unit->target.value = INVALID_ENTITY_VALUE;
	Agent_PathTo(&unit->agent, x, y);
}

void Unit_OrderAttack(Unit* unit, EntityID target)
{
	if(unit->state != UnitState_Moving)
	{
		Unit_SwitchState(unit, UnitState_Idle);
	}
	unit->order = OrderType_Attack;
	unit->target = target;
	Agent_StopPathing(&unit->agent);
}

void Unit_OrderBuildingInteraction(Unit* unit, EntityID target)
{
	if(unit->state != UnitState_Moving)
	{
		Unit_SwitchState(unit, UnitState_Idle);
	}
	unit->order = OrderType_BuildingInteraction;
	unit->target = target;
	Agent_StopPathing(&unit->agent);
}

void Unit_OrderStop(Unit* unit)
{
	if(unit->state != UnitState_Moving)
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
			
			uint8_t targetX = targetBuilding->x;
			uint8_t targetY = targetBuilding->y;
			
			if(!Agent_IsPathing(&unit->agent) || unit->agent.path.targetX != targetX || unit->agent.path.targetY != targetY)
			{
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
			
			if(!Agent_IsPathing(&unit->agent) || (unit->agent.pathingState != Pathing_FollowLeftWall && unit->agent.pathingState != Pathing_FollowRightWall && unit->agent.path.targetX != targetUnit->agent.x && unit->agent.path.targetY != targetUnit->agent.y))
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
			return true;
		}
		
		if(!Agent_IsPathing(&unit->agent) || unit->agent.path.targetX != targetX || unit->agent.path.targetY != targetY)
		{
			Agent_PathTo(&unit->agent, targetX, targetY);
		}
	}
	else
	{
		Agent_StopPathing(&unit->agent);
	}
	
	return false;
}

void Unit_Update(Unit* unit)
{
	switch(unit->state)
	{
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
				return;
			}
			else if(unit->offsetX > 0)
			{
				unit->offsetX--;
				return;
			}
			if(unit->offsetY < 0)
			{
				unit->offsetY++;
				return;
			}
			else if(unit->offsetY > 0)
			{
				unit->offsetY--;
				return;
			}

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
			if(targetBuilding)
			{
				BuildingTypeInfo* buildingInfo = &AllBuildingTypeInfo[targetBuilding->type];
				if((unit->state == UnitState_Repairing || targetBuilding->buildType == BuildType_Construct) && targetBuilding->hp < buildingInfo->hp)
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
			unit->frame++;
			
			if(!IsEntityValid(unit->target))
			{
				Unit_OrderStop(unit);
			}
			else if(unit->frame < UNIT_ATTACK_FRAMES)
			{
				return;
			}
			else
			{
				// TODO: inflict damage on target
				Unit_SwitchState(unit, UnitState_Idle);
			}
		}
		break;
		
		case UnitState_Mining:
		{
			unit->frame++;
			
			if(!IsEntityValid(unit->target))
			{
				Unit_OrderStop(unit);
			}
			else if(!Resource_GetRemaining(unit->target))
			{
				// TODO: find closest alternative
			}
			else if(unit->frame < UNIT_MINE_FRAMES)
			{
				return;
			}
			else
			{
				Unit_SwitchState(unit, UnitState_Idle);
				Resource_ReduceCount(unit->target);
				unit->collectedResource = 1;
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
					BuildingTypeInfo* targetBuildingInfo = &AllBuildingTypeInfo[targetBuilding->type];
					
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
							AllPlayers[unit->team].gold += RESOURCE_COLLECTION_AMOUNT;
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
								Unit_SwitchState(unit, UnitState_Mining);
							}
						}
						else
						{
							unit->target = Resource_FindClosest(unit->agent.x, unit->agent.y, RESOURCE_SEARCH_DISTANCE);
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
			if(IsEntityValid(unit->target))
			{
				if(Unit_PathToTarget(unit, unit->target))
				{
					Unit_SwitchState(unit, UnitState_Attacking);
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
