#pragma once
#include "Memory.h"

class MBC1Memory :
	public Memory
{
public:
	MBC1Memory(bool hasRam, bool hasBattery);

	void WriteMem(uint16_t location, uint8_t value);
	uint8_t ReadMem(uint16_t location);
protected:
	uint8_t internalReadMem(uint16_t location);

private:
	const uint8_t MODE_16_8 = 0;
	const uint8_t MODE_4_32 = 1;
	bool has_RAM = false, has_Battery = false;
	uint8_t memoryMode = 0;
};

