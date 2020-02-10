#pragma once
#include "Memory.h"
class MBC2Memory :
	public Memory
{
public:
	MBC2Memory(bool hasBattery);

	void WriteMem(uint16_t location, uint8_t value);
	uint8_t ReadMem(uint16_t location);
protected:
	uint8_t internalReadMem(uint16_t location);

private:
	bool has_Battery = false;
};

