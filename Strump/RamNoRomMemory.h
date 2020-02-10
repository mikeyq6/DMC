#pragma once
#include "Memory.h"
class RamNoRomMemory :
	public Memory
{
public:
	RamNoRomMemory(bool _hasBattery);

	void WriteMem(uint16_t location, uint8_t value);
	uint8_t ReadMem(uint16_t location);
protected:
	uint8_t internalReadMem(uint16_t location);

private:
	bool hasBattery = false;
};

