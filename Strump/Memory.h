#pragma once

#include <SDL2/SDL.h>
#include "alias.h"
#include "ROMInfo.h"
#include <mutex>
#include "JoypadState.h"

constexpr auto INTERNAL_ROM_SIZE = 256;
constexpr auto RAM_SIZE = 0x10000;
constexpr auto RAM_BANK_SIZE = 0x8000;
constexpr auto VRAM_BANK_SIZE = 0x2000;
constexpr auto PALETTE_SIZE = 0x80;

class Memory
{
public:

	uint8_t RamEnabled, RomBanking;
	uint8_t RomBank, RamBank;
	bool Startup;

	virtual ~Memory() = 0;
	void init(ROMInfo* _rominfo, uint8_t* _zreg, JoypadState* _joypadState);

	// Flags
	uint8_t getFlag(uint8_t flag);
	void setFlag(uint8_t flag);
	void resetFlag(uint8_t flag);

	uint8_t get(uint16_t address);
	void set(uint16_t address, uint8_t value);
	void increment(uint16_t address);
	virtual void WriteMem(uint16_t location, uint8_t value) = 0;
	virtual uint8_t ReadMem(uint16_t location) = 0;
	//uint8_t GetValueAt(uint16_t address);

	uint8_t GetRamBankData(uint16_t address);
	void SetRamBankData(uint16_t address, uint8_t value);

	bool CheckBitSet(uint8_t val, uint8_t bit);
	void SetBit(uint8_t* val, uint8_t bit);
	void ResetBit(uint8_t* val, uint8_t bit);
	uint8_t GetVramForAddress(uint16_t address);
	void SetVramForAddress(uint16_t address, uint8_t value);
	uint16_t GetPaletteColourInfo(uint8_t baseAddress);
	void SetVramBank(uint8_t value);

	uint8_t* GetPointerTo(uint16_t location);

	virtual void GetState(uint8_t* state, uint32_t index);
	virtual void SetState(uint8_t* state, uint32_t index);

private:
	uint8_t lastWriteAddress = 0;
	bool incrementAddress = false;
	uint8_t internal_getFlag(uint8_t flag);
	void internal_setFlag(uint8_t flag);
	void internal_resetFlag(uint8_t flag);
	void internal_increment(uint16_t address);
	void CopyRamCartridgeData();
	//uint8_t internal_GetValueAt(uint16_t address);

protected:
	std::mutex mem_mutex;

	uint8_t VramBank = 0;
	uint8_t internal_get(uint16_t address);
	void internal_set(uint16_t address, uint8_t value);
	void doDMATransfer(uint8_t startAddress);
	virtual uint8_t internalReadMem(uint16_t location) = 0;
	void setPaletteWrite(bool isObj, uint8_t value);
	void setPaletteData(uint8_t value);
	void setHDMASourceLow(uint8_t value);
	void setHDMASourceHigh(uint8_t value);
	void setHDMADestinationLow(uint8_t value);
	void setHDMADestinationHigh(uint8_t value);
	void doHDMATransfer(uint8_t value);
	uint8_t getPaletteData();
	uint16_t romAddress {0};
	uint16_t dmaSource = 0;
	uint16_t dmaDestination = 0;

	// Internal startup ROM
	uint8_t InternalRom[INTERNAL_ROM_SIZE] = { 0x31,0xFE,0xFF,0xAF,0x21,0xFF,0x9F,0x32,0xCB,0x7C,0x20,0xFB,0x21,0x26,0xFF,0x0E,0x11,0x3E,0x80,0x32,0xE2,0x0C,0x3E,0xF3,0xE2,0x32,0x3E,0x77,0x77,0x3E,0xFC,0xE0,0x47,0x11,0x04,0x01,0x21,0x10,0x80,0x1A,0xCD,0x95,0x00,0xCD,0x96,0x00,0x13,0x7B,0xFE,0x34,0x20,0xF3,0x11,0xD8,0x00,0x06,0x08,0x1A,0x13,0x22,0x23,0x05,0x20,0xF9,0x3E,0x19,0xEA,0x10,0x99,0x21,0x2F,0x99,0x0E,0x0C,0x3D,0x28,0x08,0x32,0x0D,0x20,0xF9,0x2E,0x0F,0x18,0xF3,0x67,0x3E,0x64,0x57,0xE0,0x42,0x3E,0x91,0xE0,0x40,0x04,0x1E,0x02,0x0E,0x0C,0xF0,0x44,0xFE,0x90,0x20,0xFA,0x0D,0x20,0xF7,0x1D,0x20,0xF2,0x0E,0x13,0x24,0x7C,0x1E,0x83,0xFE,0x62,0x28,0x06,0x1E,0xC1,0xFE,0x64,0x20,0x06,0x7B,0xE2,0x0C,0x3E,0x87,0xE2,0xF0,0x42,0x90,0xE0,0x42,0x15,0x20,0xD2,0x05,0x20,0x4F,0x16,0x20,0x18,0xCB,0x4F,0x06,0x04,0xC5,0xCB,0x11,0x17,0xC1,0xCB,0x11,0x17,0x05,0x20,0xF5,0x22,0x23,0x22,0x23,0xC9,0xCE,0xED,0x66,0x66,0xCC,0x0D,0x00,0x0B,0x03,0x73,0x00,0x83,0x00,0x0C,0x00,0x0D,0x00,0x08,0x11,0x1F,0x88,0x89,0x00,0x0E,0xDC,0xCC,0x6E,0xE6,0xDD,0xDD,0xD9,0x99,0xBB,0xBB,0x67,0x63,0x6E,0x0E,0xEC,0xCC,0xDD,0xDC,0x99,0x9F,0xBB,0xB9,0x33,0x3E,0x3C,0x42,0xB9,0xA5,0xB9,0xA5,0x42,0x3C,0x21,0x04,0x01,0x11,0xA8,0x00,0x1A,0x13,0xBE,0x20,0xFE,0x23,0x7D,0xFE,0x34,0x20,0xF5,0x06,0x19,0x78,0x86,0x23,0x05,0x20,0xFB,0x86,0x20,0xFE,0x3E,0x01,0xE0,0x50 };

	uint8_t memory[RAM_SIZE];
	uint8_t RamBankData[RAM_BANK_SIZE];
	uint8_t VRamBankData[2][VRAM_BANK_SIZE];
	uint8_t PaletteData[PALETTE_SIZE];

	ROMInfo* rominfo;

	uint8_t* zreg;
	JoypadState* joypadState;
};

