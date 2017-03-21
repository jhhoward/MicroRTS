#include "Pathing.h"
#include "Map.h"

void Agent_FollowWall(PathingAgent* agent, int8_t directionDelta);
bool Agent_IsDirectPathClear(PathingAgent* agent, bool horizontalFirst);
int Agent_CalculateDirectPathClearDistance(PathingAgent* agent, bool horizontalFirst);

inline direction_t WrapDirection(direction_t direction)
{
	return direction & 0x7;
}

bool Map_IsBlockedInDirection(uint8_t x, uint8_t y, direction_t direction)
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

void Agent_MoveForward(PathingAgent* agent)
{
	switch(WrapDirection(agent->direction))
	{
		case North:
		agent->y --;
		break;
		case NorthEast:
		agent->y --;
		agent->x ++;
		break;
		case East:
		agent->x ++;
		break;
		case SouthEast:
		agent->x ++;
		agent->y ++;
		break;
		case South:
		agent->y ++;
		break;
		case SouthWest:
		agent->y ++;
		agent->x --;
		break;
		case West:
		agent->x --;
		break;
		case NorthWest:
		agent->x --;
		agent->y --;
		break;
	}
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
	agent->path.startX = agent->x;
	agent->path.startY = agent->y;
	PathingAgent lookAheadLeft = *agent;
	PathingAgent lookAheadRight = *agent;
	
	lookAheadLeft.pathingState = Pathing_FollowLeftWall;
	lookAheadRight.pathingState = Pathing_FollowRightWall;
	
	uint8_t iterations = PATHING_LOOK_AHEAD_DISTANCE;
	
	while(iterations > 0)
	{
		Agent_FollowWall(&lookAheadLeft, 1);
		
		if(lookAheadLeft.pathingState != Pathing_FollowLeftWall)
		{
			agent->pathingState = Pathing_FollowLeftWall;
			return;
		}
		
		Agent_FollowWall(&lookAheadRight, -1);
		if(lookAheadRight.pathingState != Pathing_FollowRightWall)
		{
			agent->pathingState = Pathing_FollowRightWall;
			return;
		}
		
		iterations --;
	}
	
	int currentDistanceToTarget = Agent_DistanceToTarget(agent);
	int lookAheadLeftDistanceToTarget = Agent_DistanceToTarget(&lookAheadLeft);
	int lookAheadRightDistanceToTarget = Agent_DistanceToTarget(&lookAheadRight);
	
	if(currentDistanceToTarget < lookAheadLeftDistanceToTarget && currentDistanceToTarget < lookAheadRightDistanceToTarget)
	{
		agent->pathingState = Pathing_Failed;
		return;
	}
	
	if(lookAheadLeftDistanceToTarget < lookAheadRightDistanceToTarget)
	{
		agent->pathingState = Pathing_FollowLeftWall;
	}
	else
	{
		agent->pathingState = Pathing_FollowRightWall;
	}
}

void Agent_PathTo(PathingAgent* agent, uint8_t targetX, uint8_t targetY)
{
	agent->path.startX = agent->x;
	agent->path.startY = agent->y;
	agent->path.targetX = targetX;
	agent->path.targetY = targetY;

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
				return Path_CalculateDistance(x, y, agent->path.targetX, agent->path.targetY);
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
				return Path_CalculateDistance(x, y, agent->path.targetX, agent->path.targetY);
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
				return Path_CalculateDistance(x, y, agent->path.targetX, agent->path.targetY);
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
				return Path_CalculateDistance(x, y, agent->path.targetX, agent->path.targetY);
			}
		}
	}
	
	return 0;
}

bool Agent_IsDirectPathClear(PathingAgent* agent, bool horizontalFirst)
{
	return Agent_CalculateDirectPathClearDistance(agent, horizontalFirst) == 0;
}

void Agent_FollowWall(PathingAgent* agent, int8_t directionDelta)
{
	if(Map_IsBlockedInDirection(agent->x, agent->y, agent->direction))
	{
		agent->direction = WrapDirection(agent->direction + 2 * directionDelta);
	}
	else if(Map_IsBlockedInDirection(agent->x, agent->y, agent->direction - 2 * directionDelta)
		|| Map_IsBlockedInDirection(agent->x, agent->y, agent->direction - directionDelta))
	{
		Agent_MoveForward(agent);
		
		if(Path_OnDirectPath(&agent->path, agent->x, agent->y, true) || Agent_IsDirectPathClear(agent, true))
		{
			agent->pathingState = Pathing_DirectPathHorizontalFirst;
		}
		else if(/*Path_OnDirectPath(&agent->path, agent->x, agent->y, false) ||*/ Agent_IsDirectPathClear(agent, false))
		{
			agent->pathingState = Pathing_DirectPathVerticalFirst;
		}
		else if(!Map_IsBlockedInDirection(agent->x, agent->y, agent->direction - 2 * directionDelta))
		{
			agent->direction = WrapDirection(agent->direction - 2 * directionDelta);
		}
	}
	else
	{
		agent->pathingState = Pathing_DirectPathHorizontalFirst;
	}
}

void Agent_UpdatePathing(PathingAgent* agent)
{
	if(agent->x == agent->path.targetX && agent->y == agent->path.targetY)
	{
		agent->pathingState = Pathing_Idle;
		return;
	}

	switch(agent->pathingState)
	{
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
			
			if(Map_IsBlockedInDirection(agent->x, agent->y, agent->direction))
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
			
			if(Map_IsBlockedInDirection(agent->x, agent->y, agent->direction))
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
