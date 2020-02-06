#include "Memory.h"
#include <iostream>

Memory::Memory(ROMInfo* _rominfo, uint8_t* _zreg, uint8_t* _joypadState) {
	std::lock_guard<mutex> locker(mem_mutex);

	std::memset(memory, 0, sizeof(memory));
	std::memset(RamBankData, 0, sizeof(RamBankData));
	rominfo = _rominfo;
	zreg = _zreg;
	JoypadState = _joypadState;

	RamEnabled = false;
	RomBanking = true;
}
Memory::~Memory() {

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

// Memory
uint8_t Memory::ReadMem(uint16_t location) {
	std::lock_guard<mutex> locker(mem_mutex);

	return internalReadMem(location);
}
uint8_t Memory::internalReadMem(uint16_t location) {

	//printf("\nReadMem(%04x)\n", location);
// #ifdef STEPTHROUGH
	// if(location == LY && tempShow) { return 0x90; tempShow = 0; } // debug
// #else
	// 
// #endif
	//if(location == LY) { return (Startup ? 0x90 : 0x91); }

	uint32_t nAddress = location;
	 
	if (location < 0x100 && Startup) {
		return InternalRom[(uint8_t)location];
	}
	else if (location >= 0 && location < 0x4000) {
		return rominfo->GetCardridgeVal(location);
	}
	else if (location >= 0x4000 && location < 0x8000) {
		if (RomBank > 1) {
			nAddress = location + ((RomBank - 1) * 0x4000);
			return rominfo->GetCardridgeVal(nAddress);
		}
		else {
			return rominfo->GetCardridgeVal(location);
		}
	}
	else if (location >= 0x8000 && location < 0xa000) {
		return internal_get(location);
	}
	else if (location >= 0xa000 && location < 0xc000) {
		// switchable Ram bank
		if (rominfo->CartInfo->controllerType == NO_RAM) {
			return internal_get(location);
		}
		else {
			location -= 0xa000;
			if (RamEnabled)
				return RamBankData[location + (RamBank * 0x2000)];
			else
				return 0;
		}
	} 
	else if (location == P1) { // Joypad register
		lastJoypadState = *JoypadState;
		return GetJoypadState();
	}
	else if (location >= 0xc000 && location < 0xffff) {
		// Internal Work RAM
		return internal_get(location);
	}
	return 0;
}
void Memory::WriteMem(uint16_t location, uint8_t value) {
	std::lock_guard<mutex> locker(mem_mutex);

	if (value == 0xca) {
		int x = 1;
	}
	if (location == 0xc26b) {
		int x = 1;
	}
	if (location == DMA) {
		doDMATransfer(value);
	} else if (location == P1) {
		internal_set(location, value);
	} 
	else if (location == DIV) {
		internal_set(location, 0); // Always set DIV to 0 on write
	}
	else if (location == ENDSTART) {
		Startup = false;
	}
	else if (rominfo->CartInfo->controllerType == NO_ROMBANK) {
		if (location >= 0x8000) {
			internal_set(location, value);
		}
	} 
	else if (location >= 0 && location < 0x2000) {
		printf("Enabling RAM/ROM: %02x\n", value);
		//_getch();
		if ((value & 0x15) == 0x0a) {
			RamEnabled = true;
		}
		else {
			RamEnabled = false;
		}
	}
	else if (location >= 0x2000 && location < 0x4000) { // ROM Switching
		if (rominfo->CartInfo->controllerType == MBC1) {
			RomBank = value & 0x0f;
			if (RomBank == 0)
				RomBank == 1;
		}
		else if (rominfo->CartInfo->controllerType == MBC3) {
			RomBank = value & 0x7f;
		}
		else {
			RomBank = 1;
		}
	}
	else if (location >= 0x4000 && location < 0x6000) { // ROM/RAM Switching
		printf("ROM/RAM Switching. RomBanking = %s, Bank=%02x\n", (RomBanking ? "true" : "false"), value);
		//_getch();
		if (rominfo->CartInfo->controllerType == MBC1 || rominfo->CartInfo->controllerType == MBC3) {
			RamBank = value & 0x3;
		}
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
		if(location + 0x2000 < 0xfe00)
			internal_set(location + 0x2000, value);
		internal_set(location, value);
	}
	else if (location >= 0xa000 && location < 0xc000) { // Writing to RAM
		if (RamEnabled) {
			location -= 0xa000;
			RamBankData[location + (RamBank * 0x2000)] = value;
			printf("Writing (%02x) to RAM at address(%04x)\n", value, (location + (RamBank * 0x2000)));
			//_getch();
		}
		else {
			cout << "Trying to write to RAM but it is not enabled" << endl;
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

//uint8_t Memory::GetValueAt(uint16_t address) {
//	std::lock_guard<mutex> locker(mem_mutex);
//	return internal_GetValueAt(address);
//}
//uint8_t Memory::internal_GetValueAt(uint16_t address) {
//
//	uint8_t val = 0;
//	uint32_t nAddress = address;
//
//	if (address == P1) { // Joypad register
//		lastJoypadState = *JoypadState;
//		return GetJoypadState();
//	}
//	else if (address >= 0 && address < 0x4000) {
//		val = rominfo->GetCardridgeVal(address);
//	}
//	else if (address >= 0x4000 && address <= 0x7fff) {
//		if (RomBank > 1) {
//			nAddress = address + ((RomBank - 1) * 0x4000);
//			val = rominfo->GetCardridgeVal(nAddress);
//		}
//		else {
//			val = rominfo->GetCardridgeVal(address);
//		}
//	}
//	else if (address >= 0xa000 && address < 0xc000) {
//		// switchable Ram bank
//		address -= 0xa000;
//		if (RamEnabled)
//			val = RamBankData[address + (RamBank * 0x2000)];
//		else
//			val = 0;
//	}
//	else if (address >= 0xc000 && address < 0xfe00) {
//		// Internal Work RAM
//		val = internal_get(address);
//	}
//	else if (address >= 0xff80 && address < 0xffff) {
//		// High-RAM area
//		val = internal_get(address);
//	}
//	else {
//		val = internal_get(address);
//	}
//
//	return val;
//}

void Memory::doDMATransfer(uint8_t startAddress) {
	uint16_t address = ((uint16_t)startAddress) << 8;
	uint16_t oamAddress = 0xfe00;

	for (uint8_t i = 0; i <= 0xa0; i++, address++, oamAddress++) {
		internal_set(oamAddress, internalReadMem(address)); 
	}
}

uint8_t Memory::internal_getFlag(uint8_t flag) {
	std::lock_guard<mutex> locker(mem_mutex);
	return (*zreg & flag) == flag ? 1 : 0;
}
void Memory::internal_setFlag(uint8_t flag) {
	std::lock_guard<mutex> locker(mem_mutex);
	*zreg |= flag;
}
void Memory::internal_resetFlag(uint8_t flag) {
	std::lock_guard<mutex> locker(mem_mutex);
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

uint8_t Memory::GetJoypadState() {

	uint8_t val = internal_get(P1) ^ 0xff;
	
	if (!CheckBitSet(val, 4)) { // Standard buttons?
		uint8_t topJoypad = lastJoypadState >> 4;
		topJoypad |= 0xF0; // turn the top 4 bits on
		val &= topJoypad; // show what buttons are pressed
	}
	else if (!CheckBitSet(val, 5))//directional buttons
	{
		uint8_t bottomJoypad = lastJoypadState & 0xF;
		bottomJoypad |= 0xF0;
		val &= bottomJoypad;
	}

	return val;
}
bool Memory::CheckBitSet(uint8_t val, uint8_t bit) {
	uint8_t b = 1 << bit;

	return ((val & b) == b);
}
void Memory::SetBit(uint8_t* val, uint8_t bit) {
	uint8_t b = 1 << bit;
	(*val) |= b;
}void Memory::ResetBit(uint8_t* val, uint8_t bit) {
	uint8_t b = (0x1 << bit) ^ 0xff;
	(*val) &= b;
}