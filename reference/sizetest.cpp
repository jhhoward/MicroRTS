#include <stdio.h>

#include "../src/Game.h"

int main(int argc, char* argv[])
{
	printf("Building size: %d bytes x %d = %d bytes total\n", sizeof(Building), MAX_BUILDINGS, (sizeof(Building) * MAX_BUILDINGS));

	printf("Unit size: %d bytes x %d = %d bytes total\n", sizeof(Unit), MAX_UNITS, (sizeof(Unit) * MAX_UNITS));
	
	printf("Units + buildings: %d bytes\n", (sizeof(Building) * MAX_BUILDINGS) + (sizeof(Unit) * MAX_UNITS));

	printf("Game struct size: %d bytes\n", sizeof(GameData));
	
	return 0;
}