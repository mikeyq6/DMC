#pragma once
#include <cstdint>

class JoypadState
{
public:
	static const uint8_t A_BUTTON = 0;
	static const uint8_t B_BUTTON = 1;
	static const uint8_t SELECT_BUTTON = 2;
	static const uint8_t START_BUTTON = 3;
	static const uint8_t RIGHT_BUTTON = 4;
	static const uint8_t LEFT_BUTTON = 5;
	static const uint8_t UP_BUTTON = 6;
	static const uint8_t DOWN_BUTTON = 7;

	JoypadState();

	bool IsActive(uint8_t button);
	void ButtonDown(uint8_t button);
	void ButtonUp(uint8_t button);

	uint8_t GetKeypadState();
	uint8_t GetDirectionalState();

private:
	uint8_t state;

	uint8_t getState(uint8_t bit);
};

