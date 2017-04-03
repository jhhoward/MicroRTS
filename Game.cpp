#include <stdint.h>
#include "Game.h"
#include "Unit.h"
#include "Building.h"
#include "Resource.h"
#include "AI.h"
#include "Player.h"

GameData Game;

static uint8_t UnitUpdateIndex = 0;
static uint8_t BuildingUpdateIndex = 0;

void Game_StartLevel()
{
	Unit_InitSystem();
	Building_InitSystem();
	Resource_InitSystem();
	
	Game.state = GameState_Playing;
	Game.isSplitScreen = false;
	Game.player1 = Player_Human;
	Game.player2 = Player_AI;
}

void Game_Update()
{
	switch(Game.state)
	{
		case GameState_Playing:
		{
			uint8_t counter = UNITS_UPDATED_PER_FRAME;
			
			while(counter)
			{
				Unit_Update(&AllUnits[UnitUpdateIndex]);
				
				UnitUpdateIndex++;
				if(UnitUpdateIndex == MAX_UNITS)
					UnitUpdateIndex = 0;
				counter--;
			}
			
			counter = BUILDINGS_UPDATED_PER_FRAME;

			while(counter)
			{
				Building_Update(&AllBuildings[BuildingUpdateIndex]);
				
				BuildingUpdateIndex++;
				if(BuildingUpdateIndex == MAX_UNITS)
					BuildingUpdateIndex = 0;
				counter--;
			}

			if(Game.player1 == Player_AI)
			{
				AI_Update(Player1_Team);
			}
			if(Game.player2 == Player_AI)
			{
				AI_Update(Player2_Team);
			}
		}
		break;
	}
}

int Game_Main()
{
	Game_StartLevel();
	
	while(1)
	{
		Game_Update();
	}
}
