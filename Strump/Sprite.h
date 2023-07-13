#pragma once

#include <stdint.h>
#include "alias.h"
#include "tile.h"
#include "Memory.h"

const int SPRITE_MODE_8x8 = 0;
const int SPRITE_MODE_8x16 = 1;

class Sprite
{
	public:

		uint8_t X, Y, TileNumber, PaletteBank, Attributes, CGBPalette, Number, SpritePriority, CGBVbank;
		bool YFlip, XFlip;

		void GetSpriteTile(Memory* memory, tile* t);
		static void GetSpriteByNumber(uint8_t spriteNum, Memory *memory, Sprite* sprite);

		void Draw();
};

