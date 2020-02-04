#pragma once

#include "Commands.h"
#include "opcodes.h"
#include "Memory.h"
#include "registers.h"

class Test
{
private:
	Commands* commands;
	Memory* memory;
	Registers* registers;

	void clearFlags();

public:
	Test(Commands* _commands, Memory* _memory, Registers* _registers);
	~Test();

	void TestInstructions();
};

