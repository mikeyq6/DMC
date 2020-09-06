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
		return internal_get(location);
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
			location -= 0xa000;
			RamBankData[location + (RAMB * 0x2000)] = value;
			printf("Writing (%02x) to RAM at address(%04x)\n", value, (location + (RamBank * 0x2000)));
			//_getch();
		}
		else {
			// cout << "Trying to write to RAM but it is not enabled" << endl;
			//_getch();
		}
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

void MBC5Memory::GetState(uint8_t* state, uint32_t index) {
	for(int i=0; i<RAM_SIZE; i++) {
		*(state+index+i) = memory[i];
	}
	for(int i=0; i<RAM_BANK_SIZE; i++) {
		*(state+index+i+RAM_SIZE) = RamBankData[i];
	}
	index = RAM_SIZE + RAM_BANK_SIZE;
	*(state+index) = (uint8_t)RAMB;
	*(state+index+1) = (uint8_t)RAMG;
	*(state+index+2) = (uint8_t)ROMB0;
	*(state+index+3) = (uint8_t)ROMB1;
}
void MBC5Memory::SetState(uint8_t* state, uint32_t index) {
	for(int i=0; i<RAM_SIZE; i++) {
		memory[i] = *(state+index+i);
	}
	for(int i=RAM_SIZE; i<RAM_BANK_SIZE+RAM_SIZE; i++) {
		RamBankData[i] = *(state+index+i);
	}
	index = RAM_SIZE + RAM_BANK_SIZE;
	RAMB = *(state+index);
	RAMG = *(state+index+1);
	ROMB0 = *(state+index+2);
	ROMB1 = *(state+index+3);
}