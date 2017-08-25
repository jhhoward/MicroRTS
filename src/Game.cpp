#include <stdint.h>
#include "Game.h"
#include "Unit.h"
#include "Building.h"
#include "Resource.h"
#include "AI.h"
#include "Player.h"
#include "System.h"
#include "Map.h"

GameData Game;

static uint8_t UnitUpdateIndex = 0;
static uint8_t BuildingUpdateIndex = 0;

extern const Map TestMap;

void Game_StartLevel(const Map* map)
{
	Game.Map = map;
	
	Player_Init(Player1_Team);
	Player_Init(Player2_Team);
	
	Game.state = GameState_Playing;
	Game.isSplitScreen = false;
//	Game.player1 = Player_Human;
	Game.player1 = Player_AI;
	Game.player2 = Player_AI;
//	Game.player2 = Player_Human;
}

void Game_Init()
{
	Unit_InitSystem();
	Building_InitSystem();
	Resource_InitSystem();
	
	Game_StartLevel(&TestMap);
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
				Unit_Update(&Game.Units[UnitUpdateIndex]);
				
				UnitUpdateIndex++;
				if(UnitUpdateIndex == MAX_UNITS)
					UnitUpdateIndex = 0;
				counter--;
			}
			
			counter = BUILDINGS_UPDATED_PER_FRAME;

			while(counter)
			{
				Building_Update(&Game.Buildings[BuildingUpdateIndex]);
				
				BuildingUpdateIndex++;
				if(BuildingUpdateIndex == MAX_BUILDINGS)
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

int main(int argc, char* argv[])
{
	System_Init();
	
	Game_Init();

	System_Run(&Game_Update);
	
	return 0;
}
