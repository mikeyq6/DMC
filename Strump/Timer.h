#pragma once
#include <cstdint>
constexpr auto FREQ = 4194304;


class Timer
{
public:
	Timer(uint8_t* _tac, uint8_t* _tima, uint8_t* _tma, uint8_t* _div);

	bool UpdateWillInterrupt(uint8_t cycles);

private:
	uint32_t timerCounter = 0;
	uint32_t divCounter = 0;

	uint8_t* tac, *tima, *tma, *div;

	bool ClockEnabled();
	uint32_t GetClockSpeed();
	void UpdateDivRegister(uint8_t cycles);
};

