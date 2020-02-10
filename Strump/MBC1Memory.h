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
	bool has_RAM = false, has_Battery = false;
};

