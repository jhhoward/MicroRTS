#pragma once

bool Fog_IsRevealed(uint8_t team, uint8_t x, uint8_t y);
void Fog_RevealTile(uint8_t team, uint8_t x, uint8_t y);
void Fog_RevealBlock(uint8_t team, uint8_t x, uint8_t y, uint8_t size);
void Fog_Init();

