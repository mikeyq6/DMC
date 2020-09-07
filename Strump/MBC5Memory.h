#pragma once
#include "Memory.h"
class MBC5Memory :
	public Memory
{
public:
	~MBC5Memory() override;
	MBC5Memory(bool _hasRam, bool _hasBattery, bool _hasRumble, bool _hasSRAM);

	void WriteMem(uint16_t location, uint8_t value) override;
	uint8_t ReadMem(uint16_t location) override;
	void GetState(uint8_t* state, uint32_t index) override;
	void SetState(uint8_t* state, uint32_t index) override;
protected:
	uint8_t internalReadMem(uint16_t location) override;

private:
	uint8_t RAMG = 0;
	uint8_t ROMB0 = 0;
	uint8_t ROMB1 = 0;
	uint8_t RAMB = 0;
	bool hasRAM = false, hasBattery = false, hasRumble = false, hasSRAM = false;
};

