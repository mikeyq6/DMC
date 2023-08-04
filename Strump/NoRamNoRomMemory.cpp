#include "NoRamNoRomMemory.h"

NoRamNoRomMemory::~NoRamNoRomMemory() {}
// Memory
uint8_t NoRamNoRomMemory::ReadMem(uint16_t location) {
	std::lock_guard<mutex> locker(mem_mutex);

	return internalReadMem(location);
}
uint8_t NoRamNoRomMemory::internalReadMem(uint16_t location) {

	uint32_t nAddress = location;

	if (location < 0x100 && Startup) {
		return InternalRom[(uint8_t)location];
	}
	else if (location >= 0 && location < 0x8000) {
		return rominfo->GetCardridgeVal(location);
	}
	else if (location >= 0x8000 && location < 0xc000) {
		return internal_get(location);
	}
	// Undocumented registers
	else if (location == FF6C) {
		return rominfo->UseColour() ? internal_get(location) : 0xff;
	}
	else if (location == FF74) {
		return rominfo->UseColour() ? internal_get(location) : 0xff;
	}
	else if (location == P1) { // Joypad register
		uint8_t state = internal_get(P1);
		if ((state & 0x10) == 0) // Bit 4 P14 low
			return joypadState->GetDirectionalState();
		else if ((state & 0x20) == 0) // Bit 5 P15 low
			return joypadState->GetKeypadState();

		return 0xff; // default return everything off
	}
	else if (location >= 0xc000 && location <= 0xffff) {
		return internal_get(location);
	} else {
		return internal_get(location);
	}
}
void NoRamNoRomMemory::WriteMem(uint16_t location, uint8_t value) {
	std::lock_guard<mutex> locker(mem_mutex);

	if (location == DMA) {
		doDMATransfer(value);
	}
	else if (location == P1) {
		internal_set(location, value);
	}
	else if (location == DIV) {
		internal_set(location, 0); // Always set DIV to 0 on write
	}
	else if (location == ENDSTART) {
		Startup = false;
	}
	else if(location == BCPS && rominfo->UseColour()) {
		// printf("BCPS: %x\n", value);
		setPaletteWrite(false, value);
	}
	else if(location == BCPD && rominfo->UseColour()) {
		// printf("BCPD: %x\n", value);
		setPaletteData(value);
	}
	else if(location == OCPS && rominfo->UseColour()) {
		// printf("OCPS: %x\n", value);
		setPaletteWrite(true, value);
	}
	else if(location == OCPD && rominfo->UseColour()) {
		// printf("OCPD: %x\n", value);
		setPaletteData(value);
	}
	else if(location == HDMA1) {
		printf("HDMA1 value: %x\n", value);
		setHDMASourceHigh(value);
	}
	else if(location == HDMA2) {
		printf("HDMA2 value: %x\n", value);
		setHDMASourceLow(value);
	}
	else if(location == HDMA3) {
		printf("HDMA3 value: %x\n", value);
		setHDMADestinationHigh(value);
	}
	else if(location == HDMA4) {
		printf("HDMA4 value: %x\n", value);
		setHDMADestinationLow(value);
	}
	else if(location == HDMA5) {
		printf("HDMA5 value: %x\n", value);
		printf("source: %x, dest: %x\n", dmaSource, dmaDestination);
		doHDMATransfer(value);
	}
	// Undocumented registers
	else if (location == FF6C) {
		if(rominfo->UseColour())
			internal_set(location, 0xfe + (value & 1));
	}
	else if (location == FF74) {
		if(rominfo->UseColour())
			internal_set(location, value);
	}
	else if (location >= 0 && location < 0x8000) {
		printf("No Ram/Rom Banking allowed at address(%04x)\n", value);
	}
	else if (location >= 0xc000 && location < 0xe000) {
		if (location + 0x2000 < 0xfe00)
			internal_set(location + 0x2000, value);
		internal_set(location, value);
	}
	else if (location >= 0xe000 && location < 0xfe00) { // Allow for the mirrored internal RAM
		internal_set(location - 0x2000, value);
		internal_set(location, value);
	}
	else if (location == LY) {
		internal_set(LY, 0);
	}
	else if (location >= 0x9000 && location <= 0x98ff) {
		internal_set(location, value);
	}
	else {
		internal_set(location, value);
	}
}

uint16_t NoRamNoRomMemory::GetRomBank() {
	return 0;
}