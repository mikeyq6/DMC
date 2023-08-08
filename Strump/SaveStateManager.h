#pragma once

#include <cstdint>

#include "CPU.h"
#include "Memory.h"
#include "Timer.h"

constexpr auto TIMER_STATE_SIZE_v0 = 12;
constexpr auto REGISTERS_STATE_SIZE_v0 = 18;
constexpr auto RAM_STATE_SIZE_v0 = 100;
constexpr auto INTERNAL_ROM_SIZE_v0 = 256;
constexpr auto RAM_SIZE_v0 = 0x10000;
constexpr auto RAM_BANK_SIZE_v0 = 0x8000;
constexpr auto VRAM_BANK_SIZE_v0 = 0x2000;
constexpr auto WRAM_BANK_SIZE_v0 = 0x1000;
constexpr auto PALETTE_SIZE_v0 = 0x80;

class SaveStateManager {
    public:

        SaveStateManager();

        virtual void LoadState(string *saveFileName, CPU *cpu);
        virtual void SaveState(string *saveFileName, CPU *cpu);

};