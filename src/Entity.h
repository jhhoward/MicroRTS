#pragma once

#include <stdint.h>

typedef union
{
	struct
	{
		uint8_t id : 6;
		uint8_t type : 2;
	};
	uint8_t value;
} EntityID;

#define INVALID_ENTITY_VALUE 0xff

bool IsEntityValid(EntityID id);

enum EntityType
{
	Entity_Unit,
	Entity_Building,
	Entity_Resource
};

