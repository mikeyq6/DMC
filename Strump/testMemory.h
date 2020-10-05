#pragma once
#include "Commands.h"
#include "opcodes.h"
#include "Memory.h"
#include "registers.h"
#include "Sprite.h"
#include "GBCDraw.h"

class TestMemory
{
private:
	Commands* commands;
	Memory* memory;
	Registers* registers;

	void clearFlags();
    void TestVramBanking();
    void TestWramBanking();
    void TestPalettes();

public:
	TestMemory(Commands* _commands, Memory* _memory, Registers* _registers);
	~TestMemory();

    void RunTests();
};