#pragma once
#include "CPU.h"
#include <thread>

constexpr auto S_WIDTH = 160;
constexpr auto S_HEIGHT = 144;

class Emulator
{
private:
	CPU* cpu;
	FILE* fp;
	Draw* draw;
	const char* cartridgeFileName;

	void handleEvents();
	void update();
	bool running() { return isRunning; }

	bool isRunning;

	void processKeyEvent(SDL_Event* event);

public:
	Emulator(const char* _cartridgeFileName);
	~Emulator();

	bool Init();
	void Start();
	void End();
};

