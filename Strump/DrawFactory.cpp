#include "DrawFactory.h"

Draw* DrawFactory::GetDrawByType(Memory* memory, Registers* registers, CartridgeInfo* cartInfo) {
	Draw* draw = NULL;

	switch (cartInfo->gbc) {
		case 0x80:
			draw = new GBDraw(memory, registers);
			break;
		default:
			draw = new GBDraw(memory, registers);
            break;
	}
	return draw;
}