#pragma once
#include "Memory.h"

class MBC1Memory :
	public Memory
{
public:
	~MBC1Memory() override;
	MBC1Memory(bool hasRam, bool hasBattery);

	void WriteMem(uint16_t location, uint8_t value) override;
	uint8_t ReadMem(uint16_t location) override;
	uint16_t GetRomBank() override;
protected:
	uint8_t internalReadMem(uint16_t location) override;

private:
	const uint8_t MODE_16_8 = 0;
	const uint8_t MODE_4_32 = 1;
	bool has_RAM = false, has_Battery = false;
	uint8_t MODE = 0;
	uint8_t RAMG = 0;
	uint8_t BANK1 = 1;
	uint8_t BANK2 = 0;

	uint16_t GetRom1BankMask();
	uint16_t GetRom2BankMask();
	uint16_t GetRomBankHigh();
	uint16_t GetRomBankLow();
};

