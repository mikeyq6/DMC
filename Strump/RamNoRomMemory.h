#pragma once
#include "Memory.h"
class RamNoRomMemory :
	public Memory
{
public:
	~RamNoRomMemory() override;
	RamNoRomMemory(bool _hasBattery);

	void WriteMem(uint16_t location, uint8_t value) override;
	uint8_t ReadMem(uint16_t location) override;
	uint16_t GetRomBank() override;
protected:
	uint8_t internalReadMem(uint16_t location) override;

private:
	bool hasBattery = false;
};

