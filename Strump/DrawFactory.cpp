#include "DrawFactory.h"

Draw* DrawFactory::GetDrawByType(Memory* memory, Registers* registers, CartridgeInfo* cartInfo) {
	Draw* draw = NULL;

	switch (cartInfo->gbc) {
		case 0x80:
		case 0xc0:
			draw = new GBCDraw(memory, registers);
			break;
		default:
			draw = new GBDraw(memory, registers);
            break;
	}
	return draw;
}