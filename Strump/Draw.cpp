#include "Draw.h"

Draw::~Draw() {}

void Draw::getPixel(tile* t, uint8_t col, uint8_t row, uint8_t* val) {
	return getPixel(t, col, row, val, false, false);
}
void Draw::getPixel(tile* t, uint8_t col, uint8_t row, uint8_t* val, bool xFlip, bool yFlip) {
	uint8_t bit = 0;
	uint8_t rIndex = 0;
	if (xFlip)
		bit = 0x80 >> (7 - col);
	else
		bit = 1 << (7 - col);
	if (yFlip)
		rIndex = 16 - (row * 2);
	else
		rIndex = (row * 2);

	*val = ((t->data[rIndex] & bit) ? 1 : 0) + (((t->data[rIndex + 1]) & bit) ? 2 : 0);
}
uint16_t Draw::GetBackgroundTileMapLocation() {
	uint8_t val = memory->get(LCDC);
	return (((val & (1 << 3)) == 0) ? 0x9800 : 0x9c00);
}
uint16_t Draw::GetWindowTileMapLocation() {
	uint8_t val = memory->get(LCDC);
	return (((val & (1 << 6)) == 0) ? 0x9800 : 0x9c00);
}
uint16_t Draw::BGWindowTileLocation() {
	uint8_t val = memory->get(LCDC);
	return (((val & (1 << 4)) == 0) ? 0x9000 : 0x8000);
}
bool Draw::GetWindowEnabled() {
	uint8_t val = memory->get(LCDC);
	return (val & (1 << 5)) == 1;
}

bool Draw::tileIsNotEmpty(tile* t) {
	for (int i = 0; i < 16; i++) {
		if (t->data[i] > 0)
			return true;
	}
	return false;
}

bool Draw::SpritesEnabled() {
	return (memory->ReadMem(LCDC) & 0x2) == 0x2;
}