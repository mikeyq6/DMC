#pragma once
#include "CPU.h"
#include "DrawFactory.h"
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
	bool showOAMMap, showPalettes, showTileMap, showFullBackground;

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

	void viewRegisterState();
	void editRegister();

public:
	Emulator(const char* _cartridgeFileName);
	~Emulator();

	bool Init();
	void Start();
	void End();
};

