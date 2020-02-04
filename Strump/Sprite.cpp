#include "Sprite.h"


void Sprite::GetSpriteTile(Memory *memory, tile* t) {
	uint16_t address = 0x8000 + ((TileNumber - 1) * 16);

	for (uint8_t i = 0; i < 16; i++) {
		t->data[i] = memory->ReadMem(address + i);
	}
}
