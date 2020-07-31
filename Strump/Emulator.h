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
	string windowTitle;

	void handleEvents();
	void update();
	bool running() { return isRunning; }
	void setWindowTitle();

	bool isRunning;

	void processKeyEvent(SDL_Event* event);

	void saveGameState();
	void loadGameState();

	void viewMemory();
	void editMemory();

public:
	Emulator(const char* _cartridgeFileName);
	~Emulator();

	bool Init();
	void Start();
	void End();
};

