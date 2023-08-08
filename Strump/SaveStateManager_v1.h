#pragma once

#include "SaveStateManager.h"
#include "CPU.h"
#include "Memory.h"
#include "Timer.h"

constexpr auto TIMER_STATE_SIZE_v1 = 12;
constexpr auto REGISTERS_STATE_SIZE_v1 = 18;
constexpr auto RAM_STATE_SIZE_v1 = 100;
constexpr auto INTERNAL_ROM_SIZE_v1 = 256;
constexpr auto RAM_SIZE_v1 = 0x10000;
constexpr auto RAM_BANK_SIZE_v1 = 0x8000;
constexpr auto VRAM_BANK_SIZE_v1 = 0x400;
constexpr auto WRAM_BANK_SIZE_v1 = 0x1000;
constexpr auto PALETTE_SIZE_v1 = 0x80;

class SaveStateManager_v1 : public SaveStateManager {
    public:

        SaveStateManager_v1();

        void LoadState(string *saveFileName, CPU *cpu) override;
};