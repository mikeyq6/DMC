#include "MBC1Memory.h"

uint8_t getRomBank();

MBC1Memory::MBC1Memory(bool hasRam, bool hasBattery) {
	this->has_RAM = hasRam;
	this->has_Battery = hasBattery;
	MODE = MODE_16_8; // default
}
MBC1Memory::~MBC1Memory() {}

// Memory
uint8_t MBC1Memory::ReadMem(uint16_t location) {
	std::lock_guard<mutex> locker(mem_mutex);

	return internalReadMem(location);
}

uint8_t MBC1Memory::GetRomBank() {
	uint8_t bank = BANK1;
	bank |= (BANK2 << 5);
	if(bank % 0x20 == 0) bank++;
	if(bank > rominfo->GetNumberOfRomBanks()) {
		bank &= (rominfo->GetNumberOfRomBanks() - 1);
	}
	return bank;
}

uint8_t MBC1Memory::internalReadMem(uint16_t location) {

	//printf("\nReadMem(%04x)\n", location);
// #ifdef STEPTHROUGH
	// if(location == LY && tempShow) { return 0x90; tempShow = 0; } // debug
// #else
	// 
// #endif
	//if(location == LY) { return (Startup ? 0x90 : 0x91); }



	uint32_t nAddress = location;
	uint8_t bank = 0;

	if (location == 0xffff) {
		int x = 1;
	}

	if (location < 0x100 && Startup) {
		return InternalRom[(uint8_t)location];
	}
	else if (location >= 0 && location < 0x4000) {
		if(MODE == MODE_16_8)
			return rominfo->GetCardridgeVal(location);
		else {
			bank = BANK2 << 5;
			if(bank % 0x20 == 0) bank++;
			if(bank > rominfo->GetNumberOfRomBanks()) {
				bank &= (rominfo->GetNumberOfRomBanks() - 1);
			}
			nAddress = location + ((bank - 1) * 0x4000);
			return rominfo->GetCardridgeVal(nAddress);
		}
	}
	else if (location >= 0x4000 && location < 0x8000) {
		bank = GetRomBank();
		nAddress = location + ((bank - 1) * 0x4000);
		return rominfo->GetCardridgeVal(nAddress);
	}
	else if (location >= 0x8000 && location < 0xa000) {
		return internal_get(location);
	}
	else if (location >= 0xa000 && location < 0xc000) {
		if (RamEnabled && this->has_RAM) {
			location -= 0xa000;
			return RamBankData[location + ((RamBank - 1) * 0x2000)];
		} else {
			return internal_get(location);
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
void MBC1Memory::WriteMem(uint16_t location, uint8_t value) {
	std::lock_guard<mutex> locker(mem_mutex);
	uint8_t tempBank;

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
		RAMG = (value == 0xa);
	}
	else if (location >= 0x2000 && location < 0x4000) { // ROM Switching
		BANK1 = value & 0x1f;
	}
	else if (location >= 0x4000 && location < 0x6000) { // ROM/RAM Switching
		// printf("Current RomBank: %x, value: %x, ", RomBank, value);
		BANK2 = value & 3;
		if (MODE == MODE_4_32) {
			if(this->has_RAM && RAMG) {
				RamBank = value + 1;
			}
		}
	}
	else if (location >= 0x6000 && location < 0x8000) { // Set Memory mode
		if ((value & 1) == 0)
			MODE = MODE_16_8;
		else
			MODE = MODE_4_32;
	}
	else if (location >= 0x9000 && location <= 0x98ff) {
		if (value == 0x30)
			int c = 1;
		internal_set(location, value);
	}
	else if (location >= 0xa000 && location < 0xc000) { // Writing to RAM
		if (RAMG && this->has_RAM) {
			location -= 0xa000;
			RamBankData[location + ((RamBank - 1) * 0x2000)] = value;
			printf("Writing (%02x) to RAM at address(%04x) (Rambank: %02x)\n", value, (location + ((RamBank - 1) * 0x2000)), RamBank);
			//_getch();
		} else {
			cout << "Trying to write to RAM but it is not enabled" << endl;
			internal_set(location, value);
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