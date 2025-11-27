#include "MBC3Memory.h"

MBC3Memory::~MBC3Memory() {}

MBC3Memory::MBC3Memory(bool _hasRam, bool _hasBattery) {
	this->hasRAM = _hasRam;
	this->hasBattery = _hasBattery;
	WRamBank = 1;
}

MBC3Memory::MBC3Memory(bool _hasRam, bool _hasBattery, bool _hasTimer) {
	this->hasRAM = _hasRam;
	this->hasBattery = _hasBattery;
	this->hasTimer = _hasTimer;
	WRamBank = 1;
}

// Memory
uint8_t MBC3Memory::ReadMem(uint16_t location) {
	std::lock_guard<mutex> locker(mem_mutex);

	return internalReadMem(location);
}
uint8_t MBC3Memory::internalReadMem(uint16_t location) {

	uint32_t nAddress = location;

	if (location < 0x100 && Startup) {
		return InternalRom[(uint8_t)location];
	}
	else if (location >= 0 && location < 0x4000) {
		return rominfo->GetCardridgeVal(location);
	}
	else if (location >= 0x4000 && location < 0x8000) {
		uint8_t bank = GetRomBank();
		nAddress = location + ((bank - 1) * 0x4000);
		return rominfo->GetCardridgeVal(nAddress);
	}
	else if (location >= 0x8000 && location < 0xa000) {
		return GetVramForAddress(location);
	}
	else if (location >= 0xa000 && location < 0xc000) {
		if (RAMG != 0xa) return 0;
		if(useRAM) {
			uint8_t rBank = RAMB & 0x3;
			if(rBank > rominfo->GetNumberOfRamBanks())
				rBank &= (rominfo->GetNumberOfRamBanks() - 1);
			uint16_t nlocation = (location & 0x1fff) | (rBank << 13);
			return RamBankData[nlocation];
		}
		if(RAMB >= 0x8 && RAMB <= 0xc)
			return (&RTC_S)[RAMB - 0x8];
		return 0;
	}
	else if (location == P1) {
		uint8_t state = internal_get(P1);
		if ((state & 0x10) == 0)
			return joypadState->GetDirectionalState();
		if ((state & 0x20) == 0)
			return joypadState->GetKeypadState();
		return 0xff;
	}
	else if (location >= 0xc000 && location <= 0xe000 && rominfo->UseColour()) {
		if(location >= 0xd000 && WRamBank > 1)
			return WRamBankData[WRamBank][location - 0xd000];
		return internal_get(location);
	}
	else if (location >= 0xc000 && location <= 0xffff) {
		// Internal Work RAM
		return internal_get(location);
	} else {
		return internal_get(location);
	}
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
	else if(location == HDMA1 && rominfo->UseColour()) {
		setHDMASourceHigh(value);
	}
	else if(location == HDMA2 && rominfo->UseColour()) {
		setHDMASourceLow(value);
	}
	else if(location == HDMA3 && rominfo->UseColour()) {
		setHDMADestinationHigh(value);
	}
	else if(location == HDMA4 && rominfo->UseColour()) {
		setHDMADestinationLow(value);
	}
	else if(location == HDMA5 && rominfo->UseColour()) {
		doHDMATransfer(value);
	}
	else if(location == VBK) {
		VramBank = value & 1;
	}
	else if(location == SVBK) {
		WRamBank = value & 0x7;
		if(WRamBank == 0) WRamBank = 1;
	}
	else if (location >= 0 && location < 0x2000) {
		RAMG = value & 0xf;
	}
	else if (location >= 0x2000 && location < 0x4000) { // ROM Switching
		ROMB = value & 0x7f;
		if (ROMB == 0)
			ROMB = 1;
	}
	else if (location >= 0x4000 && location < 0x6000) { // RAM Switching
		if((value & 0x3) == value) {
			useRAM = true;
		} else {
			useRAM = false;
		}
		RAMB = value & 0xf;
	}
	else if (location >= 0x8000 && location <= 0x98ff) {
		SetVramForAddress(location, value);
	}
	else if (location >= 0xa000 && location < 0xc000) {
		if (RAMG != 0xa) return;
		if(useRAM) {
			uint8_t rBank = RAMB & 0x3;
			if(rBank > rominfo->GetNumberOfRamBanks())
				rBank &= (rominfo->GetNumberOfRamBanks() - 1);
			uint16_t nlocation = (location & 0x1fff) | (rBank << 13);
			RamBankData[nlocation] = value;
		} else if(RAMB >= 0x8 && RAMB <= 0xc) {
			(&RTC_S)[RAMB - 0x8] = value;
		}
	}
	else if (location >= 0xc000 && location < 0xe000) {
		if(location >= 0xd000 && rominfo->UseColour() && WRamBank > 1) {
			WRamBankData[WRamBank][location - 0xd000] = value;
		} else {
			if (location + 0x2000 < 0xfe00)
				internal_set(location + 0x2000, value);
			internal_set(location, value);
		}
	}
	else if (location >= 0xe000 && location < 0xfe00) { // Allow for the mirrored internal RAM
		internal_set(location - 0x2000, value);
		// internal_set(location, value);
	}
	else if (location == LY) {
		internal_set(LY, 0);
	}
	else {
		internal_set(location, value);
	}
}

void MBC3Memory::GetState(uint8_t* state, uint32_t *index) {
	Memory::GetState(state, index);
	uint32_t val = *index;
	*(state+val++) = (uint8_t)ROMB;
	*(state+val++) = (uint8_t)RAMB;
	*(state+val++) = (uint8_t)RAMG;
	*(state+val++) = (uint8_t)RTC_S;
	*(state+val++) = (uint8_t)RTC_M;
	*(state+val++) = (uint8_t)RTC_H;
	*(state+val++) = (uint8_t)RTC_DL;
	*(state+val++) = (uint8_t)RTC_DH;
	*(state+val++) = hasRAM ? 0x1 : 0;
	*index = val;
}
void MBC3Memory::SetState(uint8_t* state, uint32_t *index) {
	Memory::SetState(state, index);
	uint32_t val = *index;
	ROMB = *(state+val++);
	RAMB = *(state+val++);
	RAMG = *(state+val++);
	RTC_S = *(state+val++);
	RTC_M = *(state+val++);
	RTC_H = *(state+val++);
	RTC_DL = *(state+val++);
	RTC_DH = *(state+val++);
	hasRAM = *(state+val++) == 0x1;
	*index = val;
}

inline uint16_t MBC3Memory::GetRomBank() {
	uint16_t bank = ROMB;
	if(bank > rominfo->GetNumberOfRomBanks())
		bank &= (rominfo->GetNumberOfRomBanks() - 1);
	return bank;
}