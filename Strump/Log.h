#pragma once

#include <stdio.h>
#include "alias.h"
#include "CPU.h"
#include "registers.h"

class Log
{
private:
	FILE* fp;
	int param1, param2, opcode;
	Memory* memory;
	Registers* registers;

public:
	Log(Memory* _memory, Registers* _registers);
	~Log();
	short GetParameters(uint8_t opcode, uint8_t* param1, uint8_t* param2);
};

