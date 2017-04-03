#include <stdio.h>

#include "Unit.h"
#include "Building.h"

Building AllBuildings[MAX_BUILDINGS];
Unit AllUnits[MAX_UNITS];

int main(int argc, char* argv[])
{
	printf("Building size: %d bytes x %d = %d bytes total\n", sizeof(Building), MAX_BUILDINGS, (sizeof(Building) * MAX_BUILDINGS));

	printf("Unit size: %d bytes x %d = %d bytes total\n", sizeof(Unit), MAX_UNITS, (sizeof(Unit) * MAX_UNITS));
	
	printf("Units + buildings: %d bytes\n", (sizeof(Building) * MAX_BUILDINGS) + (sizeof(Unit) * MAX_UNITS));
	
	AllUnits[0].offsetX = 0;
	AllUnits[0].offsetY = 0;
	
	printf("%d %d\n", AllUnits[0].offsetX, AllUnits[0].offsetY);

	AllUnits[0].offsetX = -7;
	AllUnits[0].offsetY = 7;

	printf("%d %d\n", AllUnits[0].offsetX, AllUnits[0].offsetY);

	AllUnits[0].offsetX = 7;
	AllUnits[0].offsetY = -7;
	
	printf("%d %d\n", AllUnits[0].offsetX, AllUnits[0].offsetY);
	
	return 0;
}