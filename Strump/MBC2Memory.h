#pragma once
#include "Memory.h"
class MBC2Memory :
	public Memory
{
public:
	~MBC2Memory() override;
	MBC2Memory(bool hasBattery);

	void WriteMem(uint16_t location, uint8_t value) override;
	uint8_t ReadMem(uint16_t location) override;
	uint16_t GetRomBank() override;
protected:
	uint8_t internalReadMem(uint16_t location) override;

private:
	uint8_t RAMG = 0;
	uint8_t ROMG = 0;
	bool has_Battery = false;
};

