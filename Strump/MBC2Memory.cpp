#include "MBC2Memory.h"

MBC2Memory::MBC2Memory(bool hasBattery) {
	this->has_Battery = hasBattery;
	ROMG = 1;
	RAMG = 0;
}
MBC2Memory::~MBC2Memory() {}

// Memory
uint8_t MBC2Memory::ReadMem(uint16_t location) {
	std::lock_guard<mutex> locker(mem_mutex);

	return internalReadMem(location);
}
uint8_t MBC2Memory::internalReadMem(uint16_t location) {
	uint32_t nAddress = location;
	uint8_t data = 0;

	if (location < 0x100 && Startup) {
		return InternalRom[(uint8_t)location];
	}
	else if (location >= 0 && location < 0x4000) {
		return rominfo->GetCardridgeVal(location);
	}
	else if (location >= 0x4000 && location < 0x8000) {
		uint8_t bank = ROMG;
		if(bank > rominfo->GetNumberOfRomBanks()) {
			bank &= (rominfo->GetNumberOfRomBanks() - 1);
		}
		nAddress = location + ((bank - 1) * 0x4000);
		data = rominfo->GetCardridgeVal(nAddress);
		
		// printf("bank: %x, nAddress: %x, location: %x, data: %x\n", bank, nAddress, location, data);
		return data;
	}
	else if (location >= 0x8000 && location < 0xa000) {
		return internal_get(location);
	}
	else if (location >= 0xa000 && location < 0xc000) {

		if(RAMG == 0xa) {
			nAddress = 0xa000 + (location & 0x1ff);
			data = internal_get(0xa000 + (location & 0x1ff));
		} else {
			data = 0;
		}
		printf("RAMG: %x, location: %x, nAddress: %x, data: %x\n", RAMG, location, nAddress, data);
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
void MBC2Memory::WriteMem(uint16_t location, uint8_t value) {
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
	else if (location == LY) {
		internal_set(LY, 0);
	}
	else if (location >= 0 && location < 0x4000) {
		if((location & 0x0100) == 0x100) {
			// ROM Select
			ROMG = value & 0x0f;
			if(ROMG == 0) ROMG++;
			// printf("ROMG: %x\n", ROMG);
		} else {
			// RAM Enable/Disable
			RAMG = value & 0xf;
			printf("RAMG: %x\n", RAMG);
		}
	}
	else if(location >= 0x4000 && location < 0x8000) {
		return;
	}
	else if(location >= 0x8000 && location < 0x9000) {
		internal_set(location, value);
	}
	else if (location >= 0x9000 && location <= 0x98ff) {
		internal_set(location, value);
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
	else if (location >= 0xa000 && location < 0xc000) { // Writing to RAM
		if (RAMG == 0xa) {
			uint16_t nLocation = 0xa000 + (location & 0x1ff);
			internal_set(nLocation, value);
		}
		else {
			cout << "Trying to write to RAM but it is not enabled" << endl;
		}
	}
	else {
		internal_set(location, value);
	}
}
