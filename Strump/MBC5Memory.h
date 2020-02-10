#pragma once
#include "Memory.h"
class MBC5Memory :
	public Memory
{
public:
	MBC5Memory(bool _hasRam, bool _hasBattery, bool _hasRumble, bool _hasSRAM);

	void WriteMem(uint16_t location, uint8_t value);
	uint8_t ReadMem(uint16_t location);
protected:
	uint8_t internalReadMem(uint16_t location);

private:
	bool hasRAM = false, hasBattery = false, hasRumble = false, hasSRAM = false;
};

