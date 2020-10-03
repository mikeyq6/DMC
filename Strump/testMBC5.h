#pragma once
#include "Commands.h"
#include "opcodes.h"
#include "Memory.h"
#include "registers.h"
#include "Sprite.h"
#include "GBCDraw.h"

class TestMBC5
{
private:
	Commands* commands;
	Memory* memory;
	Registers* registers;

	void clearFlags();

public:
	TestMBC5(Commands* _commands, Memory* _memory, Registers* _registers);
	~TestMBC5();

    void RunTests();
};