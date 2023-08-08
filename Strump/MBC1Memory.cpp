#include "MBC1Memory.h"


MBC1Memory::MBC1Memory(bool hasRam, bool hasBattery) {
	this->has_RAM = hasRam;
	this->has_Battery = hasBattery;
	MODE = MODE_16_8; // default
	RamBank = 0;
}
MBC1Memory::~MBC1Memory() {}

// Memory
uint8_t MBC1Memory::ReadMem(uint16_t location) {
	std::lock_guard<mutex> locker(mem_mutex);

	return internalReadMem(location);
}

uint8_t MBC1Memory::internalReadMem(uint16_t location) {

	uint32_t nAddress = location;
	uint16_t bank = 0;

	if (location < 0x100 && Startup) {
		return InternalRom[(uint8_t)location];
	}
	else if (location >= 0 && location < 0x4000) {
		uint8_t data = 0;
		if(MODE == MODE_16_8)
			data = rominfo->GetCardridgeVal(location);
		else {
			bank = GetRomBankLow();
			// if(bank % 0x20 == 0) bank++;
			// if(bank > rominfo->GetNumberOfRomBanks()) {
			// 	bank &= (rominfo->GetNumberOfRomBanks() - 1);
			// }
			if(bank == 0) {
				nAddress = location;
			} else {
				nAddress = location + ((bank - 1) * 0x4000);
			}
			data = rominfo->GetCardridgeVal(nAddress);
		}
		// printf("ROM MODE: %x, location: %x, data: %x\n", MODE, location, data);
		return data;
	}
	else if (location >= 0x4000 && location < 0x8000) {
		bank = GetRomBankHigh();
		if(bank == 0) {
			nAddress = location - 0x4000;
		} else {
			// if(bank % 0x20 == 0) bank++;
			nAddress = location + ((bank - 1) * 0x4000);
		}
		if(bank != 1) {
		printf("nAddress: %x\n\n", nAddress);
		}
		
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
	} else {
		return internal_get(location);
	}
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
		printf("GetRom1BankMask:%x, value: %x, BANK1: %x, BANK2: %x, location: %x\n", GetRom1BankMask(), value, BANK1, BANK2, location);
	}
	else if (location >= 0x4000 && location < 0x6000) { // ROM/RAM Switching
		if (MODE == MODE_16_8) {
			if((((value & 3) << 5) | BANK1) < rominfo->GetNumberOfRomBanks()) {
			BANK2 = value & 3;
			}
			printf("MODE_16_8 value: %x, BANK1: %x, BANK2: %x, location: %x\n", value, BANK1, BANK2, location);
		} else {
			RamBank = value & 3;
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
			// cout << "Trying to write to RAM but it is not enabled" << endl;
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

// uint16_t MBC1Memory::GetRomBank() {
// 	uint16_t rawBank = BANK1;
// 	// rawBank |= (BANK2 << 5);
// 	uint16_t bank = rawBank;
// 	uint16_t fullBank = rawBank | (BANK2 << 5);
// 	// if(bank % 0x20 == 0) bank++;
// 	uint16_t numAvailableBanks = rominfo->GetNumberOfRomBanks();
// 	if(bank > 1) {
// 	printf("BANK1: %x, BANK2: %x, raw bank: %x, numAvailableBanks:%x\n", BANK1, BANK2, bank, numAvailableBanks);
// 	}
// 	if(fullBank  > numAvailableBanks) {
// 		bank &= GetRom1BankMask();   
// 	} 
// 	if(rawBank == 0) {
// 		bank++;
// 	}
// 	if(bank != 1) {                                                                                                                                                     
// 	printf("calculated bank: %x\n", bank);
// 	}
// 	return bank;
// }
uint16_t MBC1Memory::GetRomBank() {
	return 0;
}

uint16_t MBC1Memory::GetRomBankHigh() {
	uint16_t rawBank = BANK1;
	uint16_t bank = rawBank;
	uint16_t fullBank = rawBank | (BANK2 << 5);
	bool addOne = false;

	bank &= GetRom1BankMask();
	if (MODE == MODE_16_8) {
		bank |= (BANK2 << 5);
	}

	if(bank > rominfo->GetNumberOfRomBanks()) {
		bank &= (rominfo->GetNumberOfRomBanks() - 1);
	}
	if(BANK1 == 0) {
		bank++;
	}
	return bank;
}

uint16_t MBC1Memory::GetRomBankLow() {
	uint16_t rawBank = BANK1;
	uint16_t bank = (BANK2 << 5);
	// uint16_t fullBank = rawBank | (BANK2 << 5);
	// bool addOne = false;

	// bank &= GetRom1BankMask();
	// if(BANK1 == 0) {
	// 	bank++;
	// }
	// bank |= (BANK2 << 5);

	// if(bank > rominfo->GetNumberOfRomBanks()) {
	// 	bank &= (rominfo->GetNumberOfRomBanks() - 1);
	// }
	return bank;
}

uint16_t MBC1Memory::GetRom1BankMask() {
	uint16_t romBankMask = rominfo->GetNumberOfRomBanks() - 1;
	romBankMask = romBankMask & 0x1f;

	return romBankMask;
}

uint16_t MBC1Memory::GetRom2BankMask() {
	uint16_t romBankMask = rominfo->GetNumberOfRomBanks() - 1;
	// romBankMask = (romBankMask >> 5) & 0x3;
	romBankMask = 0x3;

	return romBankMask;
}