#pragma once
#include "Memory.h"
class NoRamNoRomMemory final :
	public Memory
{
public:
	~NoRamNoRomMemory() override;

	void WriteMem(uint16_t location, uint8_t value) override;
	uint8_t ReadMem(uint16_t location) override;
protected:
	uint8_t internalReadMem(uint16_t location) override;
};

