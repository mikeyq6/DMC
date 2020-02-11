#include "JoypadState.h"

JoypadState::JoypadState() {
	state = 0;
}

bool JoypadState::IsActive(uint8_t button) {
	uint8_t bit = 1 << button;
	return (state & bit) == bit;
}
void JoypadState::ButtonDown(uint8_t button) {
	state |= (1 << button);
}
void JoypadState::ButtonUp(uint8_t button) {
	state &= ~(1 << button);
}
uint8_t JoypadState::GetKeypadState() {
	uint8_t bit = 1;
	return getState(bit);
}
uint8_t JoypadState::GetDirectionalState() {
	uint8_t bit = 0x10;
	return getState(bit);
}

uint8_t JoypadState::getState(uint8_t bit) {
	uint8_t result = 0;
	if ((state & bit) == bit)
		result = 1;
	bit <<= 1;
	if ((state & bit) == bit)
		result |= 2;
	bit <<= 1;
	if ((state & bit) == bit)
		result |= 4;
	bit <<= 1;
	if ((state & bit) == bit)
		result |= 8;

	return ~(result);
}