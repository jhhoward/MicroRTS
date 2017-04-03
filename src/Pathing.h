#pragma once

#include <stdint.h>

#define PATHING_LOOK_AHEAD_DISTANCE 20

typedef struct
{
	uint8_t startX, startY;
	uint8_t targetX, targetY;
} Path;

typedef uint8_t direction_t;

enum PathingState
{
	Pathing_Idle,
	Pathing_Failed,
	Pathing_DirectPathHorizontalFirst,
	Pathing_DirectPathVerticalFirst,
	Pathing_FollowLeftWall,
	Pathing_FollowRightWall,
};

enum PathingDirection
{
	North, NorthEast, East, SouthEast, South, SouthWest, West, NorthWest
};

typedef struct
{
	uint8_t x, y;
	Path path;
	uint8_t pathingState : 4;
	direction_t direction : 4;
} PathingAgent;

void Agent_UpdatePathing(PathingAgent* agent);
void Agent_PathTo(PathingAgent* agent, uint8_t targetX, uint8_t targetY);

int Path_CalculateDistance(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

inline void Agent_StopPathing(PathingAgent* agent)
{
	agent->pathingState = Pathing_Idle;
}

inline bool Agent_IsPathing(PathingAgent* agent)
{
	return agent->pathingState > Pathing_Failed;
}
