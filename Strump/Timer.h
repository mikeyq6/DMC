#pragma once
#include <cstdint>


class Timer
{
private:
	uint16_t div;
	uint8_t tac, tma, tima;
	uint32_t ticksSinceOverflow = 0;
	bool overflow = false;

	void incTima();
	void updateDiv(uint16_t newDiv);

	public:
		void tick();
};

