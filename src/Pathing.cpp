#include "Pathing.h"
#include "Map.h"

void Agent_FollowWall(PathingAgent* agent, int8_t directionDelta);
bool Agent_IsDirectPathClear(PathingAgent* agent, bool horizontalFirst);
int Agent_CalculateDirectPathClearDistance(PathingAgent* agent, bool horizontalFirst);

inline direction_t WrapDirection(direction_t direction)
{
	return direction & 0x7;
}

direction_t Agent_CalculateDirection(uint8_t x, uint8_t y, uint8_t targetX, uint8_t targetY)
{
	if(x < targetX)
	{
		if(y < targetY)
		{
			return SouthEast;
		}
		else if(y > targetY)
		{
			return NorthEast;
		}
		else
		{
			return East;
		}
	}
	else if(x > targetX)
	{
		if(y < targetY)
		{
			return SouthWest;
		}
		else if(y > targetY)
		{
			return NorthWest;
		}
		else
		{
			return West;
		}
	}
	else
	{
		if(y < targetY)
		{
			return South;
		}
		else if(y > targetY)
		{
			return North;
		}
	}
	return North;
}

bool Map_IsBlockedByWorld(uint8_t x, uint8_t y)
{
	BlockedType blockType = Map_IsBlocked(x, y);
	return blockType = BlockedByWorld || blockType == BlockedByBuilding || blockType == BlockedByResource;
}

BlockedType Map_IsMovementBlockedInDirection(uint8_t x, uint8_t y, direction_t direction)
{
	switch(WrapDirection(direction))
	{
		case North:
		return Map_IsBlocked(x, y - 1);
		case NorthEast:
		if(Map_IsBlockedByWorld(x, y - 1) || Map_IsBlockedByWorld(x + 1, y))
			return BlockedByWorld;
		return Map_IsBlocked(x + 1, y - 1);
		case East:
		return Map_IsBlocked(x + 1, y);
		case SouthEast:
		if(Map_IsBlockedByWorld(x, y + 1) || Map_IsBlockedByWorld(x + 1, y))
			return BlockedByWorld;
		return Map_IsBlocked(x + 1, y + 1);
		case South:
		return Map_IsBlocked(x, y + 1);
		case SouthWest:
		if(Map_IsBlockedByWorld(x, y + 1) || Map_IsBlockedByWorld(x - 1, y))
			return BlockedByWorld;
		return Map_IsBlocked(x - 1, y + 1);
		case West:
		return Map_IsBlocked(x - 1, y);
		case NorthWest:
		if(Map_IsBlockedByWorld(x, y - 1) || Map_IsBlockedByWorld(x - 1, y))
			return BlockedByWorld;
		return Map_IsBlocked(x - 1, y - 1);
	}
	
	return NotBlocked;
}

BlockedType Map_IsBlockedInDirection(uint8_t x, uint8_t y, direction_t direction)
{
	switch(WrapDirection(direction))
	{
		case North:
		return Map_IsBlocked(x, y - 1);
		case NorthEast:
		return Map_IsBlocked(x + 1, y - 1);
		case East:
		return Map_IsBlocked(x + 1, y);
		case SouthEast:
		return Map_IsBlocked(x + 1, y + 1);
		case South:
		return Map_IsBlocked(x, y + 1);
		case SouthWest:
		return Map_IsBlocked(x - 1, y + 1);
		case West:
		return Map_IsBlocked(x - 1, y);
		case NorthWest:
		return Map_IsBlocked(x - 1, y - 1);
	}
	
	return NotBlocked;
}

bool Map_IsBlockedByStaticInDirection(uint8_t x, uint8_t y, direction_t direction)
{
	BlockedType blockType = Map_IsBlockedInDirection(x, y, direction);
	return blockType != NotBlocked && blockType != BlockedByMovingUnit;
}

bool Path_IsOnDiagonalPath(Path* path, uint8_t x, uint8_t y)
{
	uint8_t i = path->startX;
	uint8_t j = path->startY;
	
	while(i != path->targetX || j != path->targetY)
	{
		if(i < path->targetX)
		{
			i++;
		}
		else if(i > path->targetX)
		{
			i--;
		}
		if(j < path->targetY)
		{
			j++;
		}
		else if(j > path->targetY)
		{
			j--;
		}
		
		if(i == x && j == y)
		{
			return true;
		}
	}
	
	return false;
}

bool Path_OnDirectPath(Path* path, uint8_t x, uint8_t y, bool horizontalFirst)
{
	if(horizontalFirst)
	{
	return 
		(y == path->startY && ((x > path->startX && x <= path->targetX) || (x >= path->targetX && x < path->startX))) ||
		(x == path->targetX && ((y > path->startY && y <= path->targetY) || (y >= path->targetY && y < path->startY)));
	}
	else
	{
	return 
		(y == path->targetY && ((x > path->startX && x <= path->targetX) || (x >= path->targetX && x < path->startX))) ||
		(x == path->startX && ((y > path->startY && y <= path->targetY) || (y >= path->targetY && y < path->startY)));
	}
}

void MoveForward(uint8_t* x, uint8_t* y, direction_t direction)
{
	switch(WrapDirection(direction))
	{
		case North:
		(*y) --;
		break;
		case NorthEast:
		(*y) --;
		(*x) ++;
		break;
		case East:
		(*x) ++;
		break;
		case SouthEast:
		(*x) ++;
		(*y) ++;
		break;
		case South:
		(*y) ++;
		break;
		case SouthWest:
		(*y) ++;
		(*x) --;
		break;
		case West:
		(*x) --;
		break;
		case NorthWest:
		(*x) --;
		(*y) --;
		break;
	}
}

void Agent_MoveForward(PathingAgent* agent)
{
	MoveForward(&agent->x, &agent->y, agent->direction);
}

int Path_CalculateDistance(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	int distance = 0;
	distance += x2 > x1 ? x2 - x1 : x1 - x2;
	distance += y2 > y1 ? y2 - y1 : y1 - y2;
	return distance;
}

int Agent_DistanceToTarget(PathingAgent* agent)
{
	return Path_CalculateDistance(agent->x, agent->y, agent->path.targetX, agent->path.targetY);
}

void Agent_AvoidObstacle(PathingAgent* agent)
{
//	if(agent->pathingState != WaitingForObstacle && Map_IsBlockedInDirection(agent->x, agent->y, agent->direction) == BlockedByMovingAgent)
//	{
//		printf("Waiting for obstacle\n");
//		agent->pathingState = WaitingForObstacle;
//		return;
//	}
	
#if ALLOW_DIAGONAL_MOVEMENT
	if(agent->direction & 1)
	{
		if(Map_IsBlockedInDirection(agent->x, agent->y, WrapDirection(agent->direction + 1)))
		{
			agent->direction = WrapDirection(agent->direction + 1);
		}
		else
		{
			agent->direction = WrapDirection(agent->direction - 1);
		}
	}
#endif
	
	agent->path.startX = agent->x;
	agent->path.startY = agent->y;
	PathingAgent lookAheadLeft = *agent;
	PathingAgent lookAheadRight = *agent;
	
	lookAheadLeft.pathingState = Pathing_FollowLeftWall;
	lookAheadRight.pathingState = Pathing_FollowRightWall;
	
	uint8_t iterations = PATHING_LOOK_AHEAD_DISTANCE;
	
	while(iterations > 0)
	{
		Agent_UpdatePathing(&lookAheadLeft);
		
		if(lookAheadLeft.pathingState != Pathing_FollowLeftWall)
		{
			agent->pathingState = Pathing_FollowLeftWall;
			Agent_UpdatePathing(agent);
			//printf("Follow left wall got back on track faster: %d\n", iterations);
			return;
		}
		
		Agent_UpdatePathing(&lookAheadRight);
		if(lookAheadRight.pathingState != Pathing_FollowRightWall)
		{
			agent->pathingState = Pathing_FollowRightWall;
			Agent_UpdatePathing(agent);
			//printf("Follow right wall got back on track faster: %d\n", iterations);
			return;
		}
		
		iterations --;
	}
	
	int currentDistanceToTarget = Agent_DistanceToTarget(agent);
	int lookAheadLeftDistanceToTarget = Agent_DistanceToTarget(&lookAheadLeft);
	int lookAheadRightDistanceToTarget = Agent_DistanceToTarget(&lookAheadRight);
	
	if(currentDistanceToTarget < lookAheadLeftDistanceToTarget && currentDistanceToTarget < lookAheadRightDistanceToTarget)
	{
#if ALLOW_DIAGONAL_MOVEMENT
		if(agent->pathingState == Pathing_DiagonalPath)
		{
			agent->pathingState = Pathing_DirectPathHorizontalFirst;
			return;
		}
		else
#endif
		if(agent->pathingState == Pathing_DirectPathHorizontalFirst)
		{
			agent->pathingState = Pathing_DirectPathVerticalFirst;
			return;
		}
		else
		{
			agent->pathingState = Pathing_Failed;
			return;
		}
	}
	
	if(lookAheadLeftDistanceToTarget < lookAheadRightDistanceToTarget)
	{
		//printf("Follow left wall got closer to target\n");
		agent->pathingState = Pathing_FollowLeftWall;
		Agent_UpdatePathing(agent);
	}
	else
	{
		//printf("Follow right wall got closer to target\n");
		agent->pathingState = Pathing_FollowRightWall;
		Agent_UpdatePathing(agent);
	}
}

void Agent_PathTo(PathingAgent* agent, uint8_t targetX, uint8_t targetY)
{
	agent->path.startX = agent->x;
	agent->path.startY = agent->y;
	agent->path.targetX = targetX;
	agent->path.targetY = targetY;

#if ALLOW_DIAGONAL_MOVEMENT
	agent->pathingState = Pathing_DiagonalPath;
#else
	
#if ALLOW_SHORTCUTS
	int verticalFirstDirectDistance = Agent_CalculateDirectPathClearDistance(agent, false);
	int horizontalFirstDirectDistance = Agent_CalculateDirectPathClearDistance(agent, true);

	if(horizontalFirstDirectDistance <= verticalFirstDirectDistance)
	{
		agent->pathingState = Pathing_DirectPathHorizontalFirst;
	}
	else
	{
		agent->pathingState = Pathing_DirectPathVerticalFirst;
	}
#else
	agent->pathingState = Pathing_DirectPathHorizontalFirst;
#endif
	
#endif
}

int Agent_CalculateDirectPathClearDistance(PathingAgent* agent, bool horizontalFirst)
{
	int x = agent->x;
	int y = agent->y;
	
	if(horizontalFirst)
	{
		while(x != agent->path.targetX)
		{
			if(x < agent->path.targetX)
			{
				x ++;
			}
			else
			{
				x --;
			}
			if(Map_IsBlocked(x, y))
			{
				return Path_CalculateDistance(x, y, agent->path.targetX, agent->path.targetY) + 1;
			}
		}
		while(y != agent->path.targetY)
		{
			if(y < agent->path.targetY)
			{
				y ++;
			}
			else
			{
				y --;
			}
			if(Map_IsBlocked(x, y))
			{
				return Path_CalculateDistance(x, y, agent->path.targetX, agent->path.targetY) + 1;
			}
		}
	}
	else
	{
		while(y != agent->path.targetY)
		{
			if(y < agent->path.targetY)
			{
				y ++;
			}
			else
			{
				y --;
			}
			if(Map_IsBlocked(x, y))
			{
				return Path_CalculateDistance(x, y, agent->path.targetX, agent->path.targetY) + 1;
			}
		}
		while(x != agent->path.targetX)
		{
			if(x < agent->path.targetX)
			{
				x ++;
			}
			else
			{
				x --;
			}
			if(Map_IsBlocked(x, y))
			{
				return Path_CalculateDistance(x, y, agent->path.targetX, agent->path.targetY) + 1;
			}
		}
	}
	
	return 0;
}

bool Agent_IsDiagonalPathClear(PathingAgent* agent)
{
	uint8_t i = agent->x, j = agent->y;
	
	while(i != agent->path.targetX || j != agent->path.targetY)
	{
		direction_t direction = Agent_CalculateDirection(i, j, agent->path.targetX, agent->path.targetY);
		
		if(Map_IsMovementBlockedInDirection(i, j, direction))
		{
			return false;
		}		

		MoveForward(&i, &j, direction);
	}
	
	return true;
}

bool Agent_IsDirectPathClear(PathingAgent* agent, bool horizontalFirst)
{
	return Agent_CalculateDirectPathClearDistance(agent, horizontalFirst) == 0;
}

#define OBSTACLE_AVOIDANCE_FAILURE_HEURISTIC 20

void Agent_FollowWall(PathingAgent* agent, int8_t directionDelta)
{
	uint8_t iterations = 4;
	
	int distanceFromStart = Path_CalculateDistance(agent->x, agent->y, agent->path.startX, agent->path.startY);
	int distanceFromStartToEnd = Path_CalculateDistance(agent->path.targetX, agent->path.targetY, agent->path.startX, agent->path.startY);
	int distanceToEnd = Path_CalculateDistance(agent->x, agent->y, agent->path.targetX, agent->path.targetY);
	
	if(distanceToEnd > distanceFromStartToEnd + OBSTACLE_AVOIDANCE_FAILURE_HEURISTIC && distanceFromStart > OBSTACLE_AVOIDANCE_FAILURE_HEURISTIC)
	{
		agent->pathingState = Pathing_Failed;
		return;
	}
	
	while(iterations > 0)
	{
		if(Map_IsBlockedInDirection(agent->x, agent->y, agent->direction))
		{
			agent->direction = WrapDirection(agent->direction + 2 * directionDelta);
		}
		else if(Map_IsBlockedInDirection(agent->x, agent->y, agent->direction - 2 * directionDelta)
			|| Map_IsBlockedByStaticInDirection(agent->x, agent->y, agent->direction - directionDelta))
		{
			Agent_MoveForward(agent);

#if ALLOW_DIAGONAL_MOVEMENT
			if(Path_IsOnDiagonalPath(&agent->path, agent->x, agent->y))
			{
				agent->pathingState = Pathing_DiagonalPath;
	//			printf("Back on diagonal path\n");
				return;
			}
#else
			if(Path_OnDirectPath(&agent->path, agent->x, agent->y, true))
			{
				agent->pathingState = Pathing_DirectPathHorizontalFirst;
	//			printf("Back on direct path horizontal\n");
				return;
			}
#endif
			
			if(!Map_IsBlockedByStaticInDirection(agent->x, agent->y, agent->direction - 2 * directionDelta))
			{
				agent->direction = WrapDirection(agent->direction - 2 * directionDelta);
			}
			
			return;
		}
		else if(Map_IsBlockedByStaticInDirection(agent->x, agent->y, agent->direction + directionDelta))
		{
			agent->direction = WrapDirection(agent->direction + 2 * directionDelta);
		}
		else
		{
#if ALLOW_DIAGONAL_MOVEMENT
			agent->pathingState = Pathing_DiagonalPath;
#else
			if(agent->direction == East || agent->direction == West)
			{
				agent->pathingState = Pathing_DirectPathHorizontalFirst;
			}
			else
			{
				agent->pathingState = Pathing_DirectPathVerticalFirst;
			}
#endif
			//printf("Shouldn't be in this state! :(\n");
			return;
		}
		iterations--;
	}
}

void Agent_UpdatePathing(PathingAgent* agent)
{
	if(agent->x == agent->path.targetX && agent->y == agent->path.targetY)
	{
		agent->pathingState = Pathing_Idle;
		return;
	}
	
	// Check for clear paths	
#if ALLOW_SHORTCUTS
#if ALLOW_DIAGONAL_MOVEMENT
	if(agent->pathingState != Pathing_DiagonalPath && Agent_IsDiagonalPathClear(agent))
	{
		agent->pathingState = Pathing_DiagonalPath;
	}
#endif

	if(agent->pathingState == Pathing_FollowLeftWall || agent->pathingState == Pathing_FollowRightWall)
	{
		if(Agent_IsDirectPathClear(agent, true))
		{
			agent->pathingState = Pathing_DirectPathHorizontalFirst;
		}
		else if(Agent_IsDirectPathClear(agent, false))
		{
			agent->pathingState = Pathing_DirectPathVerticalFirst;
		}
	}
#endif

	switch(agent->pathingState)
	{
		case Pathing_DiagonalPath:
			agent->direction = Agent_CalculateDirection(agent->x, agent->y, agent->path.targetX, agent->path.targetY);
		
			if(Map_IsMovementBlockedInDirection(agent->x, agent->y, agent->direction))
			{
				Agent_AvoidObstacle(agent);
			}
			else
			{
				Agent_MoveForward(agent);
			}
			break;

		case Pathing_DirectPathHorizontalFirst:

			if(agent->x != agent->path.targetX)
			{
				if(agent->x < agent->path.targetX)
				{
					agent->direction = East;
				}
				else
				{
					agent->direction = West;
				}
			}
			else
			{
				if(agent->y < agent->path.targetY)
				{
					agent->direction = South;
				}
				else if(agent->y > agent->path.targetY)
				{
					agent->direction = North;
				}
			}
			
			if(Map_IsMovementBlockedInDirection(agent->x, agent->y, agent->direction))
			{
				Agent_AvoidObstacle(agent);
			}
			else
			{
				Agent_MoveForward(agent);
			}

			break;

		case Pathing_DirectPathVerticalFirst:
			if(agent->y != agent->path.targetY)
			{
				if(agent->y < agent->path.targetY)
				{
					agent->direction = South;
				}
				else if(agent->y > agent->path.targetY)
				{
					agent->direction = North;
				}
			}
			else
			{
				if(agent->x < agent->path.targetX)
				{
					agent->direction = East;
				}
				else
				{
					agent->direction = West;
				}
			}
			
			if(Map_IsMovementBlockedInDirection(agent->x, agent->y, agent->direction))
			{
				Agent_AvoidObstacle(agent);
			}
			else
			{
				Agent_MoveForward(agent);
			}
			break;

		case Pathing_FollowLeftWall:
			Agent_FollowWall(agent, 1);
			break;
			
		case Pathing_FollowRightWall:
			Agent_FollowWall(agent, -1);
			break;
	}
}
