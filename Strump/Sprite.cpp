#include "Sprite.h"

// void Sprite::Sprite() {

// }

void Sprite::GetSpriteTile(Memory *memory, tile* t) {
	uint16_t address = 0x8000 + ((TileNumber - 1) * 16);

	for (uint8_t i = 0; i < 16; i++) {
		t->data[i] = memory->ReadMem(address + i);
	}
}

void Sprite::GetSpriteByNumber(uint8_t spriteNum, Memory *memory, Sprite* sprite) {
	uint16_t address = 0xfe00 + (spriteNum * 4); // Start address of sprite data
	uint8_t attributes = memory->ReadMem(address + 3);
	uint8_t spriteMode = (memory->ReadMem(LCDC) & 0x4) > 0 ? SPRITE_MODE_8x16 : SPRITE_MODE_8x8;
	sprite->Attributes = attributes;
	// printf("attributes: %x\n", attributes);
	sprite->Y = memory->ReadMem(address);
	sprite->X = memory->ReadMem(address + 1);
	if (spriteMode == SPRITE_MODE_8x8)
		sprite->TileNumber = memory->ReadMem(address + 2);
	else
		sprite->TileNumber = memory->ReadMem(address + 2) & 0xfe;
	sprite->CGBPalette = attributes & 0x7;
	sprite->SpritePriority = ((attributes & 0x80) == 0x80) ? 1 : 0;
	sprite->YFlip = (attributes & 0x40) == 0x40;
	sprite->XFlip = (attributes & 0x20) == 0x20;
	sprite->PaletteBank = (attributes & 0x10) == 0x10;
	// sprite->CGBVbank = (attributes & 0x8) == 0x8;
	sprite->CGBVbank = (attributes & 0x8) >> 3;
	sprite->Number = spriteNum + 1;
	if(sprite->CGBPalette > 1) {
		// sprite->Draw();
		// printf("attributes: %x\n", attributes);

	}
	// sprite->Draw();
}

void Sprite::Draw() {
	printf("Sprite data: { X:%x, Y:%x, Tile:%x, Attributes:%x, CGBPalette:%x, Number:%x, CGBVbank:%x, PaletteBank:%x }\n",
		X, Y, TileNumber, Attributes, CGBPalette, Number, CGBVbank, PaletteBank);
}
