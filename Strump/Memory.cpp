#include "Memory.h"
#include <iostream>

Memory::~Memory() {}

void Memory::init(ROMInfo* _rominfo, uint8_t* _zreg, JoypadState* _joypadState) {

	std::memset(memory, 0, sizeof(memory));
	std::memset(RamBankData, 0, sizeof(RamBankData));
	std::memset(VRamBankData, 0, sizeof(VRamBankData));
	std::memset(PaletteData, 0, sizeof(PaletteData));

	rominfo = _rominfo;
	zreg = _zreg;
	joypadState = _joypadState;

	RamEnabled = false;
	RomBanking = true;
	RamBank = 1;

	this->CopyRamCartridgeData();
}

void Memory::CopyRamCartridgeData() {
	for(uint16_t i=0; i<0x2000; i++) {
		this->RamBankData[i] = rominfo->GetCardridgeVal(0xa000 + i);
	}
}

uint8_t Memory::internal_get(uint16_t address) {
	if(address >= 0x8000 && address < 0xa000) {
		return GetVramForAddress(address);
	} else {
		return memory[address];
	}
}
void Memory::internal_set(uint16_t address, uint8_t value) {
	if(address >= 0x8000 && address < 0xa000) {
		SetVramForAddress(address, value);
	} else {
		memory[address] = value;
	}
}
void Memory::internal_increment(uint16_t address) {
	memory[address]++;
}
uint8_t Memory::GetRamBankData(uint16_t address) {
	std::lock_guard<mutex> locker(mem_mutex);
	return RamBankData[address];
}
void Memory::SetRamBankData(uint16_t address, uint8_t value) {
	std::lock_guard<mutex> locker(mem_mutex);
	RamBankData[address] = value;
}

uint8_t Memory::get(uint16_t address) {
	return internal_get(address);
}
void Memory::set(uint16_t address, uint8_t value) {
	internal_set(address, value);
}
void Memory::increment(uint16_t address) {
	internal_increment(address);
}

void Memory::setPaletteWrite(bool isObj, uint8_t value) {
	lastWriteAddress = (value & 0x3f) + (isObj ? 0x40 : 0);
	printf("lastWriteAddress: %x, value: %x\n", lastWriteAddress, value);
	incrementAddress = ((value & 0x80) == 0x80);
}
void Memory::setPaletteData(uint8_t value) {
	PaletteData[lastWriteAddress] = value;
	printf("paletteData at: %x, value: %x\n", lastWriteAddress, value);
	if(incrementAddress) lastWriteAddress++;
}
uint8_t Memory::getPaletteData() {
	return PaletteData[lastWriteAddress];
}
uint16_t Memory::GetPaletteColourInfo(uint8_t baseAddress) {
	uint8_t paletteIndexH = baseAddress + 1;
	uint8_t paletteIndexL = baseAddress;
	
	uint16_t data = PaletteData[paletteIndexL] + (((uint16_t)PaletteData[paletteIndexH]) << 8);
	return data;
}
uint8_t Memory::GetVramForAddress(uint16_t address) {
	return VRamBankData[VramBank][address -0x8000];
}
void Memory::SetVramForAddress(uint16_t address, uint8_t value) {
	VRamBankData[VramBank][address - 0x8000] = value;
}

void Memory::doDMATransfer(uint8_t startAddress) {
	uint16_t address = ((uint16_t)startAddress) << 8;
	uint16_t oamAddress = 0xfe00;

	for (uint8_t i = 0; i <= 0xa0; i++, address++, oamAddress++) {
		internal_set(oamAddress, internalReadMem(address)); 
	}
}

uint8_t Memory::internal_getFlag(uint8_t flag) {
	return (*zreg & flag) == flag ? 1 : 0;
}
void Memory::internal_setFlag(uint8_t flag) {
	*zreg |= flag;
}
void Memory::internal_resetFlag(uint8_t flag) {
	*zreg &= (~flag);
}
uint8_t Memory::getFlag(uint8_t flag) {
	return internal_getFlag(flag);
}
void Memory::setFlag(uint8_t flag) {
	internal_setFlag(flag);
}
void Memory::resetFlag(uint8_t flag) {
	internal_resetFlag(flag);
}
bool Memory::CheckBitSet(uint8_t val, uint8_t bit) {
	uint8_t b = 1 << bit;

	return ((val & b) == b);
}
void Memory::SetBit(uint8_t* val, uint8_t bit) {
	uint8_t b = 1 << bit;
	(*val) |= b;
}
void Memory::ResetBit(uint8_t* val, uint8_t bit) {
	uint8_t b = (0x1 << bit) ^ 0xff;
	(*val) &= b;
}

uint8_t* Memory::GetPointerTo(uint16_t location) {
	return memory + location;
}
void Memory::GetState(uint8_t* state, uint32_t index) {
	for(int i=0; i<RAM_SIZE; i++) {
		*(state+index+i) = memory[i];
	}
	for(int i=0; i<RAM_BANK_SIZE; i++) {
		*(state+index+i+RAM_SIZE) = RamBankData[i];
	}
	index = RAM_SIZE + RAM_BANK_SIZE;
	*(state+index) = (uint8_t)RamEnabled;
	*(state+index+1) = (uint8_t)RomBanking;
	*(state+index+2) = (uint8_t)RomBank;
	*(state+index+3) = (uint8_t)RamBank;
}
void Memory::SetState(uint8_t* state, uint32_t index) {
	for(int i=0; i<RAM_SIZE; i++) {
		memory[i] = *(state+index+i);
	}
	for(int i=RAM_SIZE; i<RAM_BANK_SIZE+RAM_SIZE; i++) {
		RamBankData[i] = *(state+index+i);
	}
	index = RAM_SIZE + RAM_BANK_SIZE;
	RamEnabled = *(state+index);
	RomBanking = *(state+index+1);
	RomBank = *(state+index+2);
	RamBank = *(state+index+3);
}
void Memory::setHDMASourceLow(uint8_t value) {
	dmaSource = (dmaSource & 0xff00) | value;
	dmaSource &= 0xfff0;
}
void Memory::setHDMASourceHigh(uint8_t value) {
	uint16_t nval = value << 8;
	dmaSource = (dmaSource & 0x00ff) | nval;
}
void Memory::setHDMADestinationLow(uint8_t value) {
	dmaDestination = (dmaDestination & 0xff00) | value;
	dmaDestination &= 0x1ff0;
}
void Memory::setHDMADestinationHigh(uint8_t value) {
	uint16_t nval = value << 8;
	dmaDestination = (dmaDestination & 0x00ff) | nval;
	dmaDestination &= 0x1ff0;
}
void Memory::doHDMATransfer(uint8_t value) {
	uint8_t numBytes = 0x10 + (value & 0x7f) * 0x10;

	uint16_t offsetDest = dmaDestination + 0x8000;
	for(int i=0; i<numBytes; i++) {
		internal_set(offsetDest + i, internal_get(dmaSource + i));
	}
	internal_set(HDMA5, 0xff);
}
void Memory::SetVramBank(uint8_t value) {
	VramBank = value & 1;
}