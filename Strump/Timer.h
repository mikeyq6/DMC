#pragma once
#include <cstdint>
constexpr auto FREQ = 4194304;
//constexpr auto FREQ = 8388608;

class Timer
{
public:
	uint8_t* tac, *tima, *tma, *div;
	uint32_t timerCounter = 0;
	uint32_t divCounter = 0;

	Timer(uint8_t* _tac, uint8_t* _tima, uint8_t* _tma, uint8_t* _div);

	bool UpdateWillInterrupt(uint8_t cycles);
	void GetState(uint8_t* state, uint32_t index);
	void SetState(uint8_t* state, uint32_t index);

private:

	bool ClockEnabled();
	uint32_t GetClockSpeed();
	void UpdateDivRegister(uint8_t cycles);
};

