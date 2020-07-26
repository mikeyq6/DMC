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
	string cartridgeFileName;
	string saveFileName; 

	void handleEvents();
	void update();
	bool running() { return isRunning; }

	bool isRunning;

	void processKeyEvent(SDL_Event* event);

	void saveGameState();
	void loadGameState();

public:
	Emulator(const char* _cartridgeFileName);
	~Emulator();

	bool Init();
	void Start();
	void End();
};

