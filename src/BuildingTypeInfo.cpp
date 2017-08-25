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
	{
		50,			// goldCost
		NULL,		// sprite
		2,			// width
		2,			// height
		20,			// hp
		0,			// prerequisiteMask
		
		// buildTypeMask
		0
	},
	
	// Barracks
	{
		100,		// goldCost
		NULL,		// sprite
		4,			// width
		4,			// height
		100,		// hp
		0,			// prerequisiteMask
		
		// buildTypeMask
		BIT(BuildType_TrainSoldier) | BIT(BuildType_Upgrade2)
	},
	
	// ArcheryRange
	{
		100,		// goldCost
		NULL,		// sprite
		4,			// width
		4,			// height
		100,		// hp
		0,			// prerequisiteMask
		
		// buildTypeMask
		BIT(BuildType_TrainArcher) 
	},

	// MageTower
	{
		100,		// goldCost
		NULL,		// sprite
		4,			// width
		4,			// height
		100,		// hp
		0,			// prerequisiteMask
		
		// buildTypeMask
		BIT(BuildType_TrainMage) 
	},

	// DefenseTower
	{
		100,		// goldCost
		NULL,		// sprite
		2,			// width
		2,			// height
		100,		// hp
		0,			// prerequisiteMask
		
		// buildTypeMask
		0
	},
};
