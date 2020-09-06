#include "MBC3Memory.h"

MBC3Memory::~MBC3Memory() {}

MBC3Memory::MBC3Memory(bool _hasRam, bool _hasBattery) {
	this->hasRAM = _hasRam;
	this->hasBattery = _hasBattery;
	memoryMode = MODE_16_8; // default
}

MBC3Memory::MBC3Memory(bool _hasRam, bool _hasBattery, bool _hasTimer) {
	this->hasRAM = _hasRam;
	this->hasBattery = _hasBattery;
	this->hasTimer = _hasTimer;
	memoryMode = MODE_16_8; // default
}

// Memory
uint8_t MBC3Memory::ReadMem(uint16_t location) {
	std::lock_guard<mutex> locker(mem_mutex);

	return internalReadMem(location);
}
uint8_t MBC3Memory::internalReadMem(uint16_t location) {

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
		uint8_t bank = ROMB;
		if(bank > rominfo->GetNumberOfRomBanks()) {
			bank &= (rominfo->GetNumberOfRomBanks() - 1);
		}
		nAddress = location + ((bank - 1) * 0x4000);
		return rominfo->GetCardridgeVal(nAddress);
	}
	else if (location >= 0x8000 && location < 0xa000) {
		return internal_get(location);
	}
	else if (location >= 0xa000 && location < 0xc000) {
		if (RAMG == 0xa) {
			uint16_t nlocation = location & 0x1fff;
			uint8_t data = 0;
			nlocation |= (RAMB << 13);
			
			data = RamBankData[nlocation];
			printf("RAMG: %x, RAMB: %x, location: %x, address: %x, RamBank: %x, data: %x\n", RAMG, RAMB, location, nlocation, RamBank, data);
			return data;
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
	else if (location >= 0 && location < 0x2000) {
		RAMG = value & 0xf;
	}
	else if (location >= 0x2000 && location < 0x4000) { // ROM Switching
		ROMB = value & 0x7f;
		if (ROMB == 0)
			ROMB = 1;
	}
	else if (location >= 0x4000 && location < 0x6000) { // ROM Switching
		RAMB = value & 0x7;
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
		if (RAMG == 0xa) {
			uint16_t nlocation = location & 0x1fff;
			nlocation |= (RAMB << 13);
			
			RamBankData[nlocation] = value;
			printf("RAMG: %x, RAMB: %x, location: %x, address: %x, RamBank: %x, value: %x\n", RAMG, RAMB, location, nlocation, RamBank, value);
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