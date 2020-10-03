#include "MBC5Memory.h"

MBC5Memory::~MBC5Memory() {}

MBC5Memory::MBC5Memory(bool _hasRam, bool _hasBattery, bool _hasRumble, bool _hasSRAM)
{
	this->hasRAM = _hasRam;
	this->hasBattery = _hasBattery;
	this->hasSRAM = _hasSRAM;
	this->hasRumble = _hasRumble;
	
	RAMG = 0;
	ROMB0 = 0;
	ROMB1 = 0;
}

// Memory
uint8_t MBC5Memory::ReadMem(uint16_t location) {
	std::lock_guard<mutex> locker(mem_mutex);

	return internalReadMem(location);
}
uint8_t MBC5Memory::internalReadMem(uint16_t location) {

	uint32_t nAddress = location;

	if (location == 0xffff) {
		int x = 1;
	}

	if (location < 0x100 && Startup) {
		return InternalRom[(uint8_t)location];
	}
	else if(location == BCPD && rominfo->UseColour()) {
		return getPaletteData();
	}
	else if(location == OCPD && rominfo->UseColour()) {
		return getPaletteData();
	}
	else if (location >= 0 && location < 0x4000) {
		return rominfo->GetCardridgeVal(location);
	}
	else if (location >= 0x4000 && location < 0x8000) {
		uint16_t bank = (ROMB1 << 8) + ROMB0;
		// if(bank == 0) bank++;
		if(bank > rominfo->GetNumberOfRomBanks()) {
			bank &= (rominfo->GetNumberOfRomBanks() - 1);
		}
		nAddress = location + ((bank - 1) * 0x4000);
		uint8_t data = rominfo->GetCardridgeVal(nAddress);
		return data;
	}
	else if (location >= 0x8000 && location < 0xa000) {
		// printf("VramBank: %x, location:%x\n", VramBank, location);
		return GetVramForAddress(location);
	}
	else if (location >= 0xa000 && location < 0xc000) {
		// switchable Ram bank
		if (RAMG == 0xa) {
			location -= 0xa000;
			return RamBankData[location + (RamBank * 0x2000)];
		}
		else {
			return internal_get(location);
		}
	}
	else if (location >= 0xc000 && location <= 0xe000 && rominfo->UseColour()) {
		if(location < 0xd000) {
			return internal_get(location);
		} else {
			if(WRamBank == 0) {
				return internal_get(location);
			} else {
				return WRamBankData[WRamBank][location - 0xd000];
			}
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
void MBC5Memory::WriteMem(uint16_t location, uint8_t value) {
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
		setPaletteWrite(false, value);
	}
	else if(location == BCPD && rominfo->UseColour()) {
		setPaletteData(value);
	}
	else if(location == OCPS && rominfo->UseColour()) {
		setPaletteWrite(true, value);
	}
	else if(location == OCPD && rominfo->UseColour()) {
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
	else if(location == VBK) {
		VramBank = value & 1;
		// SetVramBank(value);
	}
	else if(location == SVBK) {
		printf("SVBK: %x\n", value);
		WRamBank = value & 0x7;
		if(WRamBank == 0) WRamBank = 1;
	}
	else if (location >= 0 && location < 0x2000) {
		printf("location: %x, setting RAMG: %x\n", location, RAMG);
		RAMG = value;
	}
	else if (location >= 0x2000 && location < 0x4000) { // ROM Switching
		if(location < 0x3000) {
			ROMB0 = value;
		} else {
			ROMB1 = value & 1;
		}
	}
	else if (location >= 0x4000 && location < 0x6000) { // ROM/RAM Switching
		RAMB = value & 0xf;
	}
	else if (location >= 0x6000 && location < 0x8000) { // Set Rom or Ram banking
		if (rominfo->CartInfo->controllerType == MBC1) {
			RomBanking = (value == 0) ? true : false;
			RomBank = 0;
		}
	}
	else if (location >= 0x8000 && location < 0xa000) {
		SetVramForAddress(location, value);
	}
	else if (location >= 0xa000 && location < 0xc000) { // Writing to RAM
		if (RAMG == 0xa) {
			location -= 0xa000;
			RamBankData[location + (RAMB * 0x2000)] = value;
			// printf("Writing (%02x) to RAM at address(%04x)\n", value, (location + (RamBank * 0x2000)));
			//_getch();
		}
		else {
			// cout << "Trying to write to RAM but it is not enabled" << endl;
			//_getch();
		}
	}
	else if (location >= 0xc000 && location < 0xe000) { // Allow for the mirrored internal RAM
		if(rominfo->UseColour()) {
			if(location < 0xd000) {
				if (location + 0x2000 < 0xfe00)
					internal_set(location + 0x2000, value);
				internal_set(location, value);
			} else {
				if(WRamBank == 0) {
					if (location + 0x2000 < 0xfe00)
						internal_set(location + 0x2000, value);
					internal_set(location, value);
				} else {
					WRamBankData[WRamBank][location - 0xd000] = value;
				}
			}
		} else {
			if (location + 0x2000 < 0xfe00)
				internal_set(location + 0x2000, value);
			internal_set(location, value);
		}
	}
	else if (location >= 0xe000 && location < 0xfe00) { // Allow for the mirrored internal RAM
		internal_set(location - 0x2000, value);
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

void MBC5Memory::GetState(uint8_t* state, uint32_t *index) {
	Memory::GetState(state, index);
	uint32_t val = *index;
	*(state+val++) = (uint8_t)RAMB;
	*(state+val++) = (uint8_t)RAMG;
	*(state+val++) = (uint8_t)ROMB0;
	*(state+val++) = (uint8_t)ROMB1;
	*(state+val++) = hasRAM ? 0x1 : 0;
	*index = val;
}
void MBC5Memory::SetState(uint8_t* state, uint32_t *index) {
	Memory::SetState(state, index);
	uint32_t val = *index;
	RAMB = *(state+val++);
	RAMG = *(state+val++);
	ROMB0 = *(state+val++);
	ROMB1 = *(state+val++);
	hasRAM = *(state+val++) == 0x1;
	*index = val;
}