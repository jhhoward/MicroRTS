#include <stdint.h>
#include "Game.h"
#include "Player.h"
#include "Map.h"

uint8_t vram[1];

#define SCREEN_TILES_H 24
#define SCREEN_TILES_V 28

#define UI_DISPLAY_HEIGHT 2
#define SINGLE_PLAYER_VIEW_HEIGHT (SCREEN_TILES_V - UI_DISPLAY_HEIGHT)
#define MULTIPLAYER_VIEW_HEIGHT (SCREEN_TILES_V / 2 - UI_DISPLAY_HEIGHT)
#define PLAYER1_VIEW_TOP UI_DISPLAY_HEIGHT
#define PLAYER2_VIEW_TOP (MULTIPLAYER_VIEW_HEIGHT + UI_DISPLAY_HEIGHT)

void View_UpdateSectionForTeam(uint8_t team, uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	Player* player = &Game.Players[team];

	uint8_t yCount = height;
	uint8_t worldX = x + player->viewX;
	uint8_t worldY = y + player->viewY;
	uint8_t firstRow = team == 0 ? PLAYER1_VIEW_TOP : PLAYER2_VIEW_TOP;
	uint8_t* vramPtr = vram + SCREEN_TILES_H * (firstRow + y) + x;
	
	while(yCount > 0)
	{
		uint8_t xCount = width;
		
		while(xCount > 0)
		{
			uint8_t tile = Map_GetTileType(worldX, worldY);
			*vramPtr = tile;
			vramPtr++;
			worldX ++;
			xCount --;
		}
		
		worldY ++;
		worldX -= width;
		vramPtr += SCREEN_TILES_H - width;
		yCount --;
	}
}

void View_ScrollLeft(uint8_t team)
{
	uint8_t yCount = Game.isSplitScreen ? MULTIPLAYER_VIEW_HEIGHT : SINGLE_PLAYER_VIEW_HEIGHT;
	uint8_t xCount;
	uint8_t firstRow = team == 0 ? PLAYER1_VIEW_TOP : PLAYER2_VIEW_TOP;
	uint8_t* vramPtr = vram + SCREEN_TILES_H * firstRow + (SCREEN_TILES_H - 1);
	
	while(yCount > 0)
	{
		uint8_t xCount = SCREEN_TILES_H - 1;
		
		while(xCount > 0)
		{
			*vramPtr = *(vramPtr - 1);
			
			vramPtr --;
			xCount --;
		}
		
		vramPtr += SCREEN_TILES_H * 2 - 1;
		yCount --;
	}
	
	View_UpdateSectionForTeam(team, 0, 0, 1, Game.isSplitScreen ? MULTIPLAYER_VIEW_HEIGHT : SINGLE_PLAYER_VIEW_HEIGHT);
}

void View_ScrollRight(uint8_t team)
{
	uint8_t yCount = Game.isSplitScreen ? MULTIPLAYER_VIEW_HEIGHT : SINGLE_PLAYER_VIEW_HEIGHT;
	uint8_t xCount;
	uint8_t firstRow = team == 0 ? PLAYER1_VIEW_TOP : PLAYER2_VIEW_TOP;
	uint8_t* vramPtr = vram + SCREEN_TILES_H * firstRow;
	
	while(yCount > 0)
	{
		uint8_t xCount = SCREEN_TILES_H - 1;
		
		while(xCount > 0)
		{
			*vramPtr = *(vramPtr + 1);
			
			vramPtr ++;
			xCount --;
		}
		
		vramPtr ++;
		yCount --;
	}
	
	View_UpdateSectionForTeam(team, SCREEN_TILES_H - 1, 0, 1, Game.isSplitScreen ? MULTIPLAYER_VIEW_HEIGHT : SINGLE_PLAYER_VIEW_HEIGHT);
}

void View_ScrollDown(uint8_t team)
{
	uint8_t yCount = Game.isSplitScreen ? MULTIPLAYER_VIEW_HEIGHT - 1 : SINGLE_PLAYER_VIEW_HEIGHT - 1;
	uint8_t xCount;
	uint8_t firstRow = team == 0 ? PLAYER1_VIEW_TOP : PLAYER2_VIEW_TOP;
	uint8_t* vramPtr = vram + SCREEN_TILES_H * firstRow;
	
	while(yCount > 0)
	{
		uint8_t xCount = SCREEN_TILES_H;
		
		while(xCount > 0)
		{
			*vramPtr = *(vramPtr + SCREEN_TILES_H);
			
			vramPtr ++;
			xCount --;
		}
		
		yCount --;
	}
	
	View_UpdateSectionForTeam(team, 0, Game.isSplitScreen ? MULTIPLAYER_VIEW_HEIGHT - 1 : SINGLE_PLAYER_VIEW_HEIGHT - 1, SCREEN_TILES_H, 1);
}

void View_ScrollUp(uint8_t team)
{
	uint8_t numRows = Game.isSplitScreen ? MULTIPLAYER_VIEW_HEIGHT : SINGLE_PLAYER_VIEW_HEIGHT;
	uint8_t yCount = numRows - 1;
	uint8_t firstRow = (team == 0 ? PLAYER1_VIEW_TOP : PLAYER2_VIEW_TOP) + yCount;
	uint8_t* vramPtr = vram + SCREEN_TILES_H * firstRow + (SCREEN_TILES_H - 1);
	
	while(yCount > 0)
	{
		uint8_t xCount = SCREEN_TILES_H;
		
		while(xCount > 0)
		{
			*vramPtr = *(vramPtr - SCREEN_TILES_H);
			
			vramPtr --;
			xCount --;
		}
		
		yCount --;
	}
	
	View_UpdateSectionForTeam(team, 0, 0, SCREEN_TILES_H, 1);
}

