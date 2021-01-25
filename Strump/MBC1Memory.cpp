#include "MBC1Memory.h"


MBC1Memory::MBC1Memory(bool hasRam, bool hasBattery) {
	this->has_RAM = hasRam;
	this->has_Battery = hasBattery;
	MODE = MODE_16_8; // default
	RamBank = 1;
}
MBC1Memory::~MBC1Memory() {}

// Memory
uint8_t MBC1Memory::ReadMem(uint16_t location) {
	std::lock_guard<mutex> locker(mem_mutex);

	return internalReadMem(location);
}

uint8_t MBC1Memory::internalReadMem(uint16_t location) {

	uint32_t nAddress = location;
	uint8_t bank = 0;

	if (location < 0x100 && Startup) {
		return InternalRom[(uint8_t)location];
	}
	else if (location >= 0 && location < 0x4000) {
		uint8_t data = 0;
		if(MODE == MODE_16_8)
			data = rominfo->GetCardridgeVal(location);
		else {
			bank = BANK2 << 5;
			if(bank % 0x20 == 0) bank++;
			if(bank > rominfo->GetNumberOfRomBanks()) {
				bank &= (rominfo->GetNumberOfRomBanks() - 1);
			}
			nAddress = location + ((bank - 1) * 0x4000);
			data = rominfo->GetCardridgeVal(nAddress);
		}
		// printf("ROM MODE: %x, location: %x, data: %x\n", MODE, location, data);
		return data;
	}
	else if (location >= 0x4000 && location < 0x8000) {
		bank = GetRomBank();
		nAddress = location + ((bank - 1) * 0x4000);
		return rominfo->GetCardridgeVal(nAddress);
	}
	else if (location >= 0x8000 && location < 0xa000) {
		// return internal_get(location);
		return GetVramForAddress(location);
	}
	else if (location >= 0xa000 && location < 0xc000) {
		if (RAMG == 0xa) {
			uint16_t nlocation = location & 0x1fff;

			uint8_t data = 0;
			if(MODE == MODE_4_32) {
				nlocation |= (RamBank << 13);
			}
			data = RamBankData[nlocation];
			// printf("RAMG: %x, MODE: %x, location: %x, address: %x, RamBank: %x, data: %x\n", RAMG, MODE, location, nlocation, RamBank, data);
			return data;
		} else {
			// printf("RAMG: %x, MODE: %x, data: %x\n", RAMG, MODE, rominfo->GetCardridgeVal(location));
			return rominfo->GetCardridgeVal(location);
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
	else if (location == DIV) {
		internal_set(location, 0); // Always set DIV to 0 on write
	}
	else if (location == ENDSTART) {
		Startup = false;
	}
	else if (location >= 0 && location < 0x2000) {
		RAMG = value & 0xf;
		// printf("value: %x, location: %x, Ram %s\n", value, location, RAMG ? "Enabled" : "Disabled");
	}
	else if (location >= 0x2000 && location < 0x4000) { // ROM Switching
		BANK1 = value & 0x1f;
	}
	else if (location >= 0x4000 && location < 0x6000) { // ROM/RAM Switching
		BANK2 = value & 3;
		if(RAMG == 0xa) {
			if (MODE == MODE_16_8) {
				RamBank = BANK2;
			} else {
				RamBank = BANK2;
			}
			if(RamBank >= rominfo->GetNumberOfRamBanks()) {
				RamBank &= (rominfo->GetNumberOfRamBanks());
			}
		}
		// printf("MODE: %x, value: %x, BANK2: %x, RamBank:%x, AVB: %x\n", MODE, value, BANK2, RamBank, rominfo->GetNumberOfRamBanks());
	}
	else if (location >= 0x6000 && location < 0x8000) { // Set Memory mode
		if ((value & 1) == 0)
			MODE = MODE_16_8;
		else
			MODE = MODE_4_32;
	}
	else if (location >= 0x8000 && location < 0xa000) {
		// internal_set(location, value);
		SetVramForAddress(location, value);
	}
	else if (location >= 0xa000 && location < 0xc000) { // Writing to RAM
		if (RAMG == 0xa) {
			uint16_t nlocation = location & 0x1fff;
			if(MODE == MODE_4_32) {
				nlocation |= (RamBank << 13);
			}
			RamBankData[nlocation] = value;
			// printf("RAMG: %i, MODE: %x, location: %x, address: %x, RamBank: %x, value: %x\n", RAMG, MODE, location, nlocation, RamBank, value);
			// printf("Writing (%02x) to RAM at address(%04x) (Rambank: %02x)\n", value, (location + ((RamBank - 1) * 0x2000)), RamBank);
			//_getch();
		} else {
			cout << "Trying to write to RAM but it is not enabled" << endl;
			// internal_set(location, value);
		}
	}
	else if (location == LY) {
		internal_set(LY, 0);
	}

	else {
		internal_set(location, value);
	}
}

uint16_t MBC1Memory::GetRomBank() {
	uint8_t bank = BANK1;
	bank |= (BANK2 << 5);
	if(bank % 0x20 == 0) bank++;
	if(bank > rominfo->GetNumberOfRomBanks()) {
		bank &= (rominfo->GetNumberOfRomBanks() - 1);
	}
	return bank;
}