#pragma once
#include "Memory.h"
class MBC3Memory :
	public Memory
{
public:
	MBC3Memory(bool _hasRam, bool _hasBattery);

	void WriteMem(uint16_t location, uint8_t value);
	uint8_t ReadMem(uint16_t location);
protected:
	uint8_t internalReadMem(uint16_t location);

private:
	bool hasRAM = false, hasBattery = false;
};

