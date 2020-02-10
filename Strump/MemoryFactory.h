#pragma once
#include "Memory.h"
#include "ROMInfo.h"
#include "NoRamNoRomMemory.h"
#include "RamNoRomMemory.h"
#include "MBC1Memory.h"
#include "MBC2Memory.h"
#include "MBC3Memory.h"
#include "MBC5Memory.h"

class MemoryFactory
{
public:
	Memory* GetMemoryByType(CartridgeInfo* cartInfo);
};

