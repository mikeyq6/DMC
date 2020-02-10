#include "NoRamNoRomMemory.h"

// Memory
uint8_t NoRamNoRomMemory::ReadMem(uint16_t location) {
	std::lock_guard<mutex> locker(mem_mutex);

	return internalReadMem(location);
}
uint8_t NoRamNoRomMemory::internalReadMem(uint16_t location) {

	uint32_t nAddress = location;

	if (location == IE) {
		int x = 1;
	}

	if (location < 0x100 && Startup) {
		return InternalRom[(uint8_t)location];
	}
	else if (location >= 0 && location < 0x8000) {
		return rominfo->GetCardridgeVal(location);
	}
	else if (location >= 0x8000 && location < 0xc000) {
		return internal_get(location);
	}
	else if (location == P1) { // Joypad register
		lastJoypadState = *JoypadState;
		return GetJoypadState();
	}
	else if (location >= 0xc000 && location <= 0xffff) {
		return internal_get(location);
	}
	return 0;
}
void NoRamNoRomMemory::WriteMem(uint16_t location, uint8_t value) {
	std::lock_guard<mutex> locker(mem_mutex);

	if (value == 0xca) {
		int x = 1;
	}
	if (location == IE) {
		int x = 1;
	}
	if (location == DMA) {
		doDMATransfer(value);
	}
	else if (location == P1) {
		if (value == 0x20) {
			int g = 1;
		}
		uint8_t cur = internal_get(P1) >> 4;
		value >>= 4;
		cur |= value;
		value <<= 4;
		cur = internal_get(P1) & 0xf;
		value |= cur;
		internal_set(location, value);
	}
	//else if (location == IE) {
	//	internal_set(location, value);
	//}
	else if (location == DIV) {
		internal_set(location, 0); // Always set DIV to 0 on write
	}
	else if (location == ENDSTART) {
		Startup = false;
	}
	//else if (rominfo->CartInfo->controllerType == NO_ROMBANK) {
	//	if (location >= 0x8000) {
	//		internal_set(location, value);
	//	}
	//} 
	else if (location >= 0 && location < 0x8000) {
		printf("No Ram/Rom Banking allowed at address(%04x)\n", value);
	}
	else if (location >= 0xa000 && location < 0xc000) { // Writing to RAM
		if (RamEnabled) {
			cout << "Trying to write to RAM but it is not enabled" << endl;
		}
	}
	else if (location >= 0xe000 && location < 0xfe00) { // Allow for the mirrored internal RAM
		internal_set(location - 0x2000, value);
		internal_set(location, value);
	}
	else if (location >= 0xc000 && location < 0xe000) { // Allow for the mirrored internal RAM
		if (location + 0x2000 < 0xfe00)
			internal_set(location + 0x2000, value);
		internal_set(location, value);
	}
	else if (location == LY) {
		internal_set(LY, 0);
	}
	else if (location >= 0x9000 && location <= 0x98ff) {
		if (value == 0x30)
			int c = 1;
		internal_set(location, value);
	}
	else {
		internal_set(location, value);
	}
}