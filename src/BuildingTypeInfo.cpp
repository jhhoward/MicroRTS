#include "avr/pgmspace.h"
#include "Building.h"

#define BIT(x) (1 << (x))

const BuildingTypeInfo AllBuildingTypeInfo[Num_BuildingTypes] PROGMEM = 
{
	// TownCenter
	{
		100,		// goldCost
		NULL,		// sprite
		4,			// width
		4,			// height
		100,		// hp
		0,			// prerequisiteMask
		
		// buildTypeMask
		BIT(BuildType_TrainPeasant) | BIT(BuildType_Upgrade1)
	},
	// House
	// Barracks
	// ArcheryRange
	// MageTower
	// DefenseTower
};
