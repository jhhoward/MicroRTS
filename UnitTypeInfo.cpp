#include "avr/pgmspace.h"
#include "Unit.h"

#define BIT(x) (1 << (x))

UnitTypeInfo AllUnitTypeInfo[Num_UnitTypes] PROGMEM = 
{
	// Peasant
	{
		NULL,			// sprite
		100,			// hp
		1,				// attackStrength
		0,				// abilityMask
	},
	
	// Soldier
	// Archer
	// Mage
};
