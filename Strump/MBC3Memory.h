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
	void GetState(uint8_t* state, uint32_t *index) override;
	void SetState(uint8_t* state, uint32_t *index) override;
protected:
	uint8_t internalReadMem(uint16_t location) override;

private:
	bool hasRAM = false, hasBattery = false, hasTimer = false;
	uint8_t ROMB = 0;
	uint8_t RAMB = 0;
	uint8_t RAMG = 0;
	uint8_t RTC_S = 0;
	uint8_t RTC_M = 0;
	uint8_t RTC_H = 0;
	uint8_t RTC_DL = 0;
	uint8_t RTC_DH = 0;
};

