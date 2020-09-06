#pragma once
#include "Draw.h"
#include "ROMInfo.h"
#include "GBDraw.h"
#include "GBCDraw.h"

class DrawFactory
{
public:
	Draw* GetDrawByType(Memory* memory, Registers* registers, CartridgeInfo* cartInfo);
};