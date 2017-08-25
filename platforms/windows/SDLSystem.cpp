#include <SDL.h>
#include "src/System.h"
#include "src/Game.h"
#include "src/Map.h"

#include "DebugMapWindow.h"

DebugMapWindow* mapWindow;

void System_Init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	
	mapWindow = new DebugMapWindow();
	
	
}

#define DEBUG_MAP_SIZE 20
void DebugPrintMap()
{
	//int debugmap[DEBUG_MAP_SIZE * DEBUG_MAP_SIZE];
	for(int y = 0; y < DEBUG_MAP_SIZE; y++)
	{
		for(int x = 0; x < DEBUG_MAP_SIZE; x++)
		{
			switch(Map_IsBlocked(x, y))
			{
				case BlockedByWorld:
				printf("#");
				break;
				case BlockedByBuilding:
				printf("=");
				break;
				case BlockedByResource:
				printf("$");
				break;
				case BlockedByStaticUnit:
				printf("O");
				break;
				case BlockedByMovingUnit:
				printf("o");
				break;

				default:
				printf(" ");
			}
		}
		printf("\n");
	}
}

const char* GetDebugUnitOrder(Unit* unit)
{
	switch(unit->order)
	{
		case OrderType_None:
		return "None";
		case OrderType_Move:
		return "Move";
		case OrderType_BuildingInteraction:
		return "Interact";
		case OrderType_Attack:
		return "Attack";
		default:
		return "Unknown";
	}
}

const char* GetDebugUnitState(Unit* unit)
{
	switch(unit->state)
	{
		case UnitState_Idle:
		return "Idle";
		case UnitState_Moving:
		return "Moving";
		case UnitState_Attacking:
		return "Attacking";
		case UnitState_Mining:
		return "Mining";
		case UnitState_Constructing:
		return "Constructing";
		case UnitState_Repairing:
		return "Repairing";
		case UnitState_Dead:
		return "Dead";
		default:
		return "Unknown";
	}
}

const char* GetDebugUnitName(Unit* unit)
{
	switch(unit->type)
	{
		case UnitType_Peasant:
		return "Peasant";
		case UnitType_Soldier:
		return "Soldier";
		case UnitType_Archer:
		return "Archer";
		case UnitType_Mage:
		return "Mage";

		default:
		return "Unknown";
	}
}

void DumpUnitStateInfo()
{
	for(int n = 0; n < MAX_UNITS; n++)
	{
		Unit* unit = &Game.Units[n];
		
		if(unit->type != UnitType_Invalid)
		{
			printf("%d [team %d] (%d hp) : %s %s %s\n", n, unit->team, unit->hp, GetDebugUnitName(unit), GetDebugUnitOrder(unit), GetDebugUnitState(unit));
		}
	}
}

void System_Run(UpdateFunction update)
{
	SDL_Event event;

	int count = 0;
	bool Program_Running = true;
	
	bool maxSpeed = false;

	while(Program_Running)	
	{
		while(SDL_PollEvent(&event))
		{
			switch(event.type) 
			{
				case SDL_QUIT:
				Program_Running = false;
				break;
				
				case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
					case SDLK_F1:
					maxSpeed = !maxSpeed;
					break;
					case SDLK_F2:
					DumpUnitStateInfo();
					break;
					case SDLK_ESCAPE:
					Program_Running = false;
					break;
				}
				
				break;
			}
		}
		
		update();
		
		mapWindow->Update();

		SDL_Delay(maxSpeed ? 0 : 1000 / 60);
		//SDL_Delay(0);
		
		char temp[50];
		//fgets(temp, 50, stdin);
		
//		if(count % 1000 == 0)
//		{
//			DebugPrintMap();
//		}
//		count++;
	}
	
	delete mapWindow;
}
