#pragma once

#include <stdio.h>

#define LOG(x, ...) printf(x, __VA_ARGS__)

typedef void(*UpdateFunction)();

void System_Init();
void System_Run(UpdateFunction update);
