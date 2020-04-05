#pragma once
#include "Memory.h"
class NoRamNoRomMemory final :
	public Memory
{
public:
	void WriteMem(uint16_t location, uint8_t value);
	uint8_t ReadMem(uint16_t location);
protected:
	uint8_t internalReadMem(uint16_t location);
};

