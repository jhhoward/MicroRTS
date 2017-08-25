#include "avr/pgmspace.h"
#include "Unit.h"

#define BIT(x) (1 << (x))

const UnitTypeInfo AllUnitTypeInfo[Num_UnitTypes] PROGMEM = 
{
	// Peasant
	{
		40,			// goldCost
		NULL,			// sprite
		20,				// hp
		0,				// attackStrength
		0,				// abilityMask
	},
	
	// Soldier
	{
		50,				// goldCost
		NULL,			// sprite
		100,			// hp
		20,				// attackStrength
		0,				// abilityMask
	},

	// Archer
	{
		75,				// goldCost
		NULL,			// sprite
		50,				// hp
		15,				// attackStrength
		0,				// abilityMask
	},

	// Mage
	{
		100,			// goldCost
		NULL,			// sprite
		30,				// hp
		10,				// attackStrength
		0,				// abilityMask
	},
};
