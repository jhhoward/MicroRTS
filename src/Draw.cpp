#include <stdint.h>

#define MAX_DRAW_ITEMS 16

typedef struct
{
	uint8_t tileIndex;
	uint8_t x, y;
	uint8_t priority;
} DrawItem;

DrawItem DrawList[MAX_DRAW_ITEMS];
uint8_t DrawListSize = 0;

void Draw_ResetList()
{
	DrawListSize = 0;
}

void Draw_AddSprite(uint8_t tileIndex, uint8_t x, uint8_t y, uint8_t priority)
{
	for(uint8_t n = 0; n < MAX_DRAW_ITEMS; n++)
	{
		if(n == DrawListSize)
		{
			DrawList[n].tileIndex = tileIndex;
			DrawList[n].x = x;
			DrawList[n].y = y;
			DrawList[n].priority = priority;
			DrawListSize++;
			return;
		}
		
		if(DrawList[n].priority > priority)
		{
			for(uint8_t i = DrawListSize; i > n; i--)
			{
				DrawList[i] = DrawList[i - 1];
			}			
			
			DrawList[n].tileIndex = tileIndex;
			DrawList[n].x = x;
			DrawList[n].y = y;
			DrawList[n].priority = priority;
			DrawListSize++;
			return;
		}
	}
}

void Draw_ProcessList()
{
	uint8_t importanceRollCounter = (DrawListSize >> 2) + 1;
	static uint8_t importanceFlagOffset = 0;
	uint8_t importanceFlag = importanceFlagOffset;
	
	for(uint8_t n = 0; n < DrawListSize; n++)
	{
		// TODO: Draw sprites
		
		if(importanceRollCounter == 0)
		{
			importanceRollCounter = (DrawListSize >> 2) + 1;
			importanceFlag = (importanceFlag + 1) & 3;
		}
		else
		{
			importanceRollCounter--;
		}
	}
	
	importanceFlagOffset = (importanceFlagOffset + 1) & 3;
}
