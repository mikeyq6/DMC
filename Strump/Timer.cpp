#include "Timer.h"


Timer::Timer(uint8_t* _tac, uint8_t* _tima, uint8_t *_tma, uint8_t* _div) {
	tac = _tac;
	tima = _tima;
	tma = _tma;
	div = _div;
	
	timerCounter = 1024;
	divCounter = 0;
}


bool Timer::ClockEnabled() {
	return ((*tac) & 0x4) == 0x4;
}
uint32_t Timer::GetClockSpeed() {
	uint32_t speed = 1024;

	uint8_t rTAC = (*tac);
	switch (rTAC & 0x3) {
		case 0:
			speed = FREQ / 4096; break;
		case 1:
			speed = FREQ / 262144; break;
		case 2:
			speed = FREQ / 65536; break;
		case 3:
			speed = FREQ / 16384; break;
	}
	return speed;
}

bool Timer::UpdateWillInterrupt(uint8_t cycles) {
	bool willInterrupt = false;

	UpdateDivRegister(cycles);

	if (ClockEnabled()) {
		
		timerCounter -= cycles;
		//if (timerCounter <= 0) {
			timerCounter = GetClockSpeed();
			uint8_t currentCount = (*tima);

			if (currentCount == 255) {
				(*tima) = (*tma);
				willInterrupt = true;
			}
			else {
				(*tima) = currentCount + 1;
			}

		//}
	}

	return willInterrupt;
}

void Timer::UpdateDivRegister(uint8_t cycles) {
	divCounter += cycles;
	uint8_t currentDiv = (*div);

	if (divCounter >= 255) {
		divCounter = 0;
		(*div) = currentDiv + 1;
	}
}

void Timer::GetState(uint8_t* state, uint32_t index) {
	*(state+index++) = *tac;
	*(state+index++) = *tima;
	*(state+index++) = *tma;
	*(state+index++) = *div;
	for(int i=0; i<4; i++) {
		*(state+index) = (timerCounter >> (i * 8)) & 0xff;
		*(state + 4 + index++) = (divCounter >> (i * 8)) & 0xff;
	}
}
void Timer::SetState(uint8_t* state, uint32_t index) {
	*tac = *(state+index++);
	*tima = *(state+index++);
	*tma = *(state+index++);
	*div = *(state+index++);
	timerCounter = 0;
	divCounter = 0;
	for(int i=0; i<4; i++) {
		timerCounter |= (*(state + index + i) >> (i * 8));
		divCounter |= (*(state + 4 + index + i) >> (i * 8));
	}
}