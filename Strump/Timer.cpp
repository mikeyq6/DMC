#include "Timer.h"


void Timer::tick() {

}

void Timer::incTima() {
	tima++;
	if (tima == 0) {
		overflow = true;
		ticksSinceOverflow = 0;
	}
}
void Timer::updateDiv(uint16_t newDiv) {

}