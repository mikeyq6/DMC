#pragma once

#include "alias.h"
#include "opcodes.h"
#include "registers.h"
#include "SDL.h"
#include "ROMInfo.h"
#include "Commands.h"
#include "Draw.h"
#include "Memory.h"
#include "test.h"
#include <thread>

#define _CRT_SECURE_NO_WARNINGS 1
#pragma once



constexpr auto FREQ = 4194304;
constexpr auto SCOUNTER_HZ = 5000;

constexpr auto I_VBlank = 0x40;
constexpr auto I_LCDC = 0x48;
constexpr auto I_Timer = 0x50;
constexpr auto I_Serial = 0x58;
constexpr auto I_Joypad = 0x60;

constexpr uint8_t A_BUTTON = 4;
constexpr uint8_t B_BUTTON = 5;
constexpr uint8_t SELECT_BUTTON = 6;
constexpr uint8_t START_BUTTON = 7;
constexpr uint8_t INPUT_UP = 2;
constexpr uint8_t INPUT_DOWN = 3;
constexpr uint8_t INPUT_LEFT = 1;
constexpr uint8_t INPUT_RIGHT = 0;

constexpr auto BIT0 = 0x01;
constexpr auto BIT1 = 0x02;
constexpr auto BIT2 = 0x04;
constexpr auto BIT3 = 0x08;
constexpr auto BIT4 = 0x10;
constexpr auto BIT5 = 0x20;
constexpr auto BIT6 = 0x40;
constexpr auto BIT7 = 0x80;

//#define LOGPIXELS 	1

// If not Windows, then don't use scanf_s
#ifndef _WIN32
#define scanf_s scanf
#endif


//#define RUNTESTS
//#define STEPTHROUGH 0
#define LOG_COMMANDS 0
#define LOG_STATS 1
//#define DEBUG_TIMER
//#define PRINT_COMMANDS 1


class CPU
{
public:

	CPU();
	~CPU();

	// Init
	void initCPU();
	uint8_t ProcessNextInstruction();
	void Start();
	void Stop();
	bool IsRunning();
	void Pause();
	void Unpause();
	bool IsPaused();
	bool IsHalted();
	bool IsStopped();

	// Debug methods
	void NextStep();
	void ActivateStepMode();
	void DeactivateStepMode();
	bool StepModeActive();
	
	// Interrupts & Timers
	void CheckInterrupts();
	void SetInterrupt(uint8_t iRegister);
	void ResetInterrupt(uint8_t iRegister);
	void UpdateTimer(uint8_t opcode);

	// Instructions
	uint8_t GetNextInstruction();
	void Run(uint8_t opcode, uint8_t param1, uint8_t param2);
	void RunCBInstruction(uint8_t opcode);

	// LCDC 
	uint8_t IsLCDEnabled();
	void SetLCDStatus();

	// Display
	void DisplayCartridgeInfo();

	ROMInfo* GetRomInfo();

	Memory* GetMemory();
	Registers* GetRegisters();

	string GetLastInstruction();

	// Controls
	void InputProcess(uint8_t type);

private:
	mutex cpu_mutex;

	uint32_t InstructionStats[512];
	uint8_t Halted, Stopped, WillDisableInterrupts, WillEnableInterrupts, InterruptsEnabled, JoypadState;
	ROMInfo* rominfo;
	Commands* commands;
	Memory* memory;
	Registers* registers;
	Test* test;

	void setDefaults();
	short GetParameters(uint8_t opcode, uint8_t* param1, uint8_t* param2);
	uint8_t GetCycles(uint8_t opcode);
	short ShouldPrint();
	void PushPCOntoStack();
	void UpdateGraphics(uint8_t opcode);
	string ToHexString(uint8_t val);
	string ToHexString(uint16_t val);
	void CheckLYCAndSetInterruptIfValid();
	void DoCPUWait(chrono::system_clock::time_point* tp, uint8_t inst);

	uint8_t running = true;
	uint8_t pause = false;
	uint8_t shouldRefresh = 0;
	uint8_t curInst = 0;
	uint8_t param1 = 0;
	uint8_t param2 = 0;
	uint8_t inst = 0;
	string lastInst;

	bool isRunning = false;
	bool isPaused = false;
	bool stepModeActive = false;
	bool runNextStep = false;

#if defined STEPTHROUGH || defined LOG_COMMANDS
	FILE* clog;
#endif
#ifdef STEPTHROUGH 
	void DisplayState();
	void LogState();
	uint64_t skip = 0;
	int skipuntil = 0;
	short tempShow = 1;
	uint8_t bdata[BACKGROUNDTILES * 16];
	FILE* fp;
	string command = "";
	string instruction = "";
	long long int iCounter = 0;
#endif
#ifdef LOG_COMMANDS
	void LogCommand(uint16_t* pc, uint8_t* inst, uint8_t* param1, uint8_t* param2, short* params);
#endif
#ifdef LOG_STATS
	FILE* statlog;
	void LogStats();
#endif
};

