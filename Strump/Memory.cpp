#include "Memory.h"
#include <iostream>


void Memory::init(ROMInfo* _rominfo, uint8_t* _zreg, JoypadState* _joypadState) {

	std::memset(memory, 0, sizeof(memory));
	std::memset(RamBankData, 0, sizeof(RamBankData));
	rominfo = _rominfo;
	zreg = _zreg;
	joypadState = _joypadState;

	RamEnabled = false;
	RomBanking = true;
}


uint8_t Memory::internal_get(uint16_t address) {
	return memory[address];
}
void Memory::internal_set(uint16_t address, uint8_t value) {
	memory[address] = value;
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
}void Memory::increment(uint16_t address) {
	internal_increment(address);
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
	uint8_t mask = ~flag;
	*zreg &= mask;
	//printf("mask: %x, flag: %x, flags: %x\n", mask, flag, flags);
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