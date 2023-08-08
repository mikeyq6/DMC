#include "GBCTile.h"

GBCTile::GBCTile() {
	t = new tile();
}
GBCTile::~GBCTile() {
	delete t;
}

void GBCTile::GetGBCTile(Memory *memory, tile* t) {
	uint16_t address = 0x8000 + ((TileNumber - 1) * 16);
    // this->t = t;

	for (uint8_t i = 0; i < 16; i++) {
		t->data[i] = memory->ReadMem(address + i);
	}
}

void GBCTile::GetBackgroundTile(uint8_t attributes, GBCTile* gbcTile) {
    gbcTile->DisplayPriority = (attributes  >> 7) & 1;
    gbcTile->YFlip = ((attributes >> 6) & 1) == 1;
    gbcTile->XFlip = ((attributes >> 5) & 1) == 1;
    gbcTile->CGBVbank = (attributes  >> 3) & 1;
    gbcTile->CGBPalette = attributes & 0x7;
}