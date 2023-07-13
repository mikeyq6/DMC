#pragma once

#include <stdint.h>
#include "alias.h"
#include "tile.h"
#include "Memory.h"

class GBCTile
{
	public:
    	GBCTile();
	    ~GBCTile();

		uint8_t DisplayPriority, CGBVbank, CGBPalette, TileNumber;
        tile* t;
		bool YFlip, XFlip;

		void GetGBCTile(Memory* memory, tile* t);
		static void GetBackgroundTile(uint8_t attributes, GBCTile* gbcTile);
        // static void GetBackgroundTileByNumber(uint8_t spriteNum, Memory *memory, GBCTile* gbcTile);
};