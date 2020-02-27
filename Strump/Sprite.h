#pragma once

#include "alias.h"
#include "SDL.h"
#include "Memory.h"

const int SPRITE_MODE_8x8 = 0;
const int SPRITE_MODE_8x16 = 1;

typedef struct _tile {
	uint8_t data[16];
} tile;

class Sprite
{
	public:
		uint8_t X, Y, TileNumber, Attributes, CGBPalette, Number, SpritePriority;
		bool YFlip, XFlip, CGBVbank;

		void GetSpriteTile(Memory* memory, tile* t);
};

