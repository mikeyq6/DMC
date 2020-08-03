#include "MBC3Memory.h"

MBC3Memory::~MBC3Memory() {}

MBC3Memory::MBC3Memory(bool _hasRam, bool _hasBattery) {
	this->hasRAM = _hasRam;
	this->hasBattery = _hasBattery;
	memoryMode = MODE_16_8; // default
}

// Memory
uint8_t MBC3Memory::ReadMem(uint16_t location) {
	std::lock_guard<mutex> locker(mem_mutex);

	return internalReadMem(location);
}
uint8_t MBC3Memory::internalReadMem(uint16_t location) {

	//printf("\nReadMem(%04x)\n", location);
// #ifdef STEPTHROUGH
	// if(location == LY && tempShow) { return 0x90; tempShow = 0; } // debug
// #else
	// 
// #endif
	//if(location == LY) { return (Startup ? 0x90 : 0x91); }

	uint32_t nAddress = location;

	if (location == 0xffff) {
		int x = 1;
	}

	if (location < 0x100 && Startup) {
		return InternalRom[(uint8_t)location];
	}
	else if (location >= 0 && location < 0x4000) {
		return rominfo->GetCardridgeVal(location);
	}
	else if (location >= 0x4000 && location < 0x8000) {
		if (location == 0x4000) {
			int x = 1;
		}
		if (RomBank <= 1) {
			return rominfo->GetCardridgeVal(location);
		}
		else {
			nAddress = location + ((RomBank - 1) * 0x4000);
			return rominfo->GetCardridgeVal(nAddress);
		}
	}
	else if (location >= 0x8000 && location < 0xa000) {
		return internal_get(location);
	}
	else if (location >= 0xa000 && location < 0xc000) {
		if (RamEnabled && this->hasRAM) {
			location -= 0xa000;
			return RamBankData[location + ((RamBank - 1) * 0x2000)];
		}
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
		// Internal Work RAM
		return internal_get(location);
	}
	return 0;
}
void MBC3Memory::WriteMem(uint16_t location, uint8_t value) {
	std::lock_guard<mutex> locker(mem_mutex);

	//if (value == 0xca) {
	//	int x = 1;
	//}
	//if (location == IE) {
	//	int x = 1;
	//}
	if (location == DMA) {
		doDMATransfer(value);
	}
	else if (location == P1) {
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
	else if (location >= 0 && location < 0x2000) {
		//printf("Enabling RAM/ROM: %02x\n", value);
		if ((value & 0xf) == 0xa) {
			RamEnabled = true;
		}
		else {
			RamEnabled = false;
		}
	}
	else if (location >= 0x2000 && location < 0x4000) { // ROM Switching
		RomBank = value & 0x7f;
		if (RomBank == 0)
			RomBank = 1;
	}
	else if (location >= 0x6000 && location < 0x8000) { // Set Memory mode
		if ((value & 1) == 1)
			memoryMode = MODE_4_32;
		else
			memoryMode = MODE_16_8;
	}
	else if (location >= 0x9000 && location <= 0x98ff) {
		if (value == 0x30)
			int c = 1;
		internal_set(location, value);
	}
	else if (location >= 0xa000 && location < 0xc000) { // Writing to RAM
		if (RamEnabled && this->hasRAM) {
			location -= 0xa000;
			RamBankData[location + ((RamBank - 1) * 0x2000)] = value;
			//_getch();
		}
		else {
			cout << "Trying to write to RAM but it is not enabled" << endl;
			//_getch();
		}
	}
	else if (location >= 0xc000 && location < 0xe000) { // Allow for the mirrored internal RAM
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

	else {
		internal_set(location, value);
	}
}