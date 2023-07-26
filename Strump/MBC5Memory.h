#pragma once
#include "Memory.h"
#include "ROMInfo.h"
#include "JoypadState.h"

constexpr auto MAX_RAM_BANKS = 0x0f;
constexpr auto CART_RAM_BANK_SIZE = 0x2000;

class MBC5Memory :
	public Memory
{
public:
	~MBC5Memory() override;
	MBC5Memory(bool _hasRam, bool _hasBattery, bool _hasRumble, bool _hasSRAM);

	void init(ROMInfo* _rominfo, uint8_t* _zreg, JoypadState* _joypadState) override;

	void WriteMem(uint16_t location, uint8_t value) override;
	uint8_t ReadMem(uint16_t location) override;
	uint16_t GetRomBank() override;
	void GetState(uint8_t* state, uint32_t *index) override;
	void SetState(uint8_t* state, uint32_t *index) override;
	uint32_t GetMemorySize() override;
protected:
	uint8_t internalReadMem(uint16_t location) override;

private:
	uint8_t RAMG = 0;
	uint16_t ROMB0 = 0;
	uint16_t ROMB1 = 0;
	uint8_t RAMB = 0;
	uint8_t CartRamBankData[MAX_RAM_BANKS][CART_RAM_BANK_SIZE];
	bool hasRAM = false, hasBattery = false, hasRumble = false, hasSRAM = false;
};

