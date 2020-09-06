#pragma once
#include "Memory.h"
class MBC3Memory :
	public Memory
{
public:
	~MBC3Memory() override;
	MBC3Memory(bool _hasRam, bool _hasBattery);
	MBC3Memory(bool _hasRam, bool _hasBattery, bool _hasTimer);

	void WriteMem(uint16_t location, uint8_t value) override;
	uint8_t ReadMem(uint16_t location) override;
protected:
	uint8_t internalReadMem(uint16_t location) override;

private:
	bool hasRAM = false, hasBattery = false, hasTimer = false;
	const uint8_t MODE_16_8 = 0;
	const uint8_t MODE_4_32 = 1;
	uint8_t memoryMode = 0;
};

