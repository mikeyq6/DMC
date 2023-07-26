#pragma once

#include "alias.h"
#include <string>
#include <iostream>
#include <mutex>

using namespace std;

constexpr auto NO_ROMBANK = 0x00;
constexpr auto MBC1 = 0x01;
constexpr auto MBC2 = 0x02;
constexpr auto MBC3 = 0x04;
constexpr auto MBC5 = 0x08;
constexpr auto MM01 = 0x10;

constexpr auto NO_RAM = 0;
constexpr auto RAM_1_2 = 1;
constexpr auto RAM_1_8 = 2;
constexpr auto RAM_4_32 = 3;
constexpr auto RAM_16_128 = 4;

const long long CARTRIDGE_SIZE = 0x800000;

typedef struct _cartridgeInfo {
	string title;
	uint8_t gbc;
	uint8_t sgb;
	uint8_t type;
	string typeName;
	uint8_t romType;
	string romTypeName;
	uint8_t ramType;
	string ramTypeName;
	uint8_t destinationCode;
	uint8_t licenseeCode;
	uint8_t controllerType;
	uint8_t hasBattery;
	uint8_t hasRumble;
	uint8_t hasTimer;
	uint8_t hasSRAM;
	string ROMcontrollerName;

} CartridgeInfo;

class ROMInfo
{
public:
	ROMInfo();
	~ROMInfo();

	uint8_t *cartridge;
	CartridgeInfo* CartInfo;


	uint8_t GetCardridgeVal(uint32_t address);
	void SetCartridgeNames();
	void SetCartridgeInfo();
	void DisplayCartridgeInfo();
	uint16_t GetNumberOfRomBanks();
	uint8_t GetNumberOfRamBanks();
	bool UseColour();

private:
	mutex rom_mutex;

};
