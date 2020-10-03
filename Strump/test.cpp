#include "test.h"

#include <assert.h>


Test::Test(Commands* _commands, Memory* _memory, Registers* _registers) {
	commands = _commands;
	memory = _memory;
	registers = _registers;
}
Test::~Test() {}

uint8_t daa_input[99] = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99 };
uint8_t daa_output[10][99] = {
	{ 0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0},
	{ 0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0,0x1},
	{ 0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0,0x1,0x2},
	{ 0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0,0x1,0x2,0x3},
	{ 0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0,0x1,0x2,0x3,0x4},
	{ 0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0,0x1,0x2,0x3,0x4,0x5},
	{ 0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0,0x1,0x2,0x3,0x4,0x5,0x6},
	{ 0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0,0x1,0x2,0x3,0x4,0x5,0x6,0x7},
	{ 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8},
	{ 0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9}
};

// Testing
void Test::TestInstructions() {
	registers->AF.a = 1;
	registers->BC.c = 255;

	registers->HL.hl = 0x9fff;
	assert(registers->HL.h == 0x9f);
	assert(registers->HL.l == 0xff);

	commands->XOR(XOR_C, 0);
	assert(registers->AF.a == 0xfe);
	assert(memory->getFlag(Z) == 0);

	commands->XOR(XOR_A, 0);
	assert(registers->AF.a == 0);
	assert(memory->getFlag(Z) == 1);

	registers->BC.b = 16;
	commands->XOR(XOR_B, 0);
	assert(registers->AF.a == 16);
	assert(memory->getFlag(Z) == 0);

	registers->HL.hl = 0x8001;
	memory->WriteMem(registers->HL.hl, 48);
	commands->XOR(XOR_HL, 0);
	assert(registers->AF.a == 32);
	assert(memory->getFlag(Z) == 0);

	commands->XOR(XOR_n, 48);
	assert(registers->AF.a == 16);
	assert(memory->getFlag(Z) == 0);

	registers->AF.a = 0x2f;
	commands->XOR(XOR_A);
	assert(registers->AF.a == 0);

	registers->AF.a = 0x9b;
	registers->BC.c = 0x3;
	commands->OR(OR_C);
	assert(registers->AF.a == 0x9b);
	registers->BC.c = 0x4;
	commands->OR(OR_C);
	assert(registers->AF.a == 0x9f);
	registers->AF.a = 0x99;
	commands->OR(OR_A);
	assert(registers->AF.a == 0x99);

	registers->AF.a = 0x88;
	commands->AND(AND_n, 0x18);
	assert(registers->AF.a == 0x08);

	// ADD
	registers->AF.a = 10;
	registers->BC.b = 20;
	commands->ADD(ADD_A_B, 0);
	assert(registers->AF.a == 30);

	registers->AF.a = 10;
	registers->BC.c = 14;
	commands->ADD(ADD_A_C, 0);
	assert(registers->AF.a == 24);
	assert(memory->getFlag(H) == 1);
	assert(memory->getFlag(C) == 0);

	registers->AF.a = 10;
	commands->ADD(ADD_A_n, 14);
	assert(registers->AF.a == 24);
	assert(memory->getFlag(H) == 1);
	assert(memory->getFlag(C) == 0);

	
	registers->AF.a = 127;
	registers->DE.d = 255;
	commands->ADD(ADD_A_D, 0);
	assert(registers->AF.a == 126);
	assert(memory->getFlag(H) == 1);
	assert(memory->getFlag(C) == 1);

	registers->AF.a = 2;
	registers->DE.e = 3;
	commands->ADD(ADD_A_E, 0);
	assert(registers->AF.a == 5);
	assert(memory->getFlag(H) == 0);
	assert(memory->getFlag(C) == 0);
	assert(memory->getFlag(Z) == 0);

	registers->AF.a = 0;
	registers->HL.h = 0;
	commands->ADD(ADD_A_H, 0);
	assert(registers->AF.a == 0);
	assert(memory->getFlag(Z) == 1);

	registers->AF.a = 0x06;
	registers->HL.hl = 0x8003;
	memory->set(0x8003, 0x1a);
	commands->ADD(ADD_A_HL, 0);
	assert(registers->AF.a = 0x20);

	registers->HL.hl = 0x1000;
	registers->BC.bc = 0x1000;
	commands->ADD(ADD_HL_BC, 0);
	assert(registers->HL.hl == 0x2000);
	assert(memory->getFlag(N) == 0);
	assert(memory->getFlag(H) == 0);
	assert(memory->getFlag(C) == 0);

	registers->HL.hl = 0xffff;
	registers->BC.bc = 0x0001;
	commands->ADD(ADD_HL_BC, 0);
	assert(registers->HL.hl == 0x0);
	assert(memory->getFlag(N) == 0);
	assert(memory->getFlag(H) == 1);
	assert(memory->getFlag(C) == 1);

	registers->HL.hl = 120;
	commands->ADD(ADD_HL_HL);
	assert(registers->HL.hl == 240);

	registers->HL.hl = 0xffff;
	registers->DE.de = 0x0001;
	commands->ADD(ADD_HL_DE);
	assert(registers->HL.hl == 0);
	assert(memory->getFlag(C) == 1);
	assert(memory->getFlag(H) == 1);

	registers->HL.hl = 0x0fff;
	registers->DE.de = 0x0001;
	commands->ADD(ADD_HL_DE);
	assert(registers->HL.hl == 0x1000);
	assert(memory->getFlag(C) == 0);
	assert(memory->getFlag(H) == 1);

	registers->SP = 0xffff;
	int8_t param = -1;
	commands->ADD(ADD_SP_n, param);
	assert(registers->SP == 0xfffe);
	assert(memory->getFlag(H) == 1);
	assert(memory->getFlag(C) == 1);

	// OR
	registers->AF.a = 5; // 0101
	registers->BC.b = 9; // 1001
	commands->OR(OR_B, 0);
	assert(registers->AF.a == 13); // 1101
	assert(memory->getFlag(Z) == 0);

	registers->AF.a = 0x42;
	registers->BC.c = 0xc3;
	commands->OR(OR_C, 0);
	assert(registers->AF.a == 0xc3);
	assert(memory->getFlag(Z) == 0);

	registers->AF.a = 0x4a;
	registers->BC.c = 0xc3;
	commands->OR(OR_C, 0);
	assert(registers->AF.a == 0xcb);
	assert(memory->getFlag(Z) == 0);

	registers->AF.a = 0;
	registers->DE.d = 0;
	commands->OR(OR_D);
	assert(registers->AF.a == 0);
	assert(memory->getFlag(Z) == 1);

	registers->HL.hl = 0xdd48;
	memory->WriteMem(0xdd48, 0x45);
	registers->AF.a = 0x88;
	commands->OR(OR_HL);
	assert(registers->AF.a == 0xcd);
	assert(memory->getFlag(Z) == 0);
	

	// SWAP  10010100 -> 01001001
	registers->AF.a = 0x94;
	commands->SWAP(SWAP_A);
	assert(registers->AF.a == 0x49);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	memory->setFlag(N);
	registers->HL.hl = 0xdd11;
	memory->WriteMem(0xdd11, 0xac);
	commands->SWAP(SWAP_HL);
	assert(memory->ReadMem(0xdd11) == 0xca);
	assert(memory->getFlag(N) == 0);

	// PUSH/POP
	registers->AF.af = 0x2450;
	registers->SP = 0xe010;
	commands->PUSH(PUSH_AF);
	assert(registers->SP == 0xe00e);
	assert(memory->get(registers->SP) == 0x50);
	assert(memory->get(registers->SP + 1) == 0x24);
	registers->AF.af = 0;
	commands->POP(POP_AF);
	assert(registers->SP == 0xe010);
	assert(registers->AF.af == 0x2450);

	registers->SP = 0xd000;
	registers->BC.bc = 0xabcd;
	commands->PUSH(PUSH_BC);
	assert(registers->SP == 0xcffe);
	assert(memory->get(registers->SP) == 0xcd);
	assert(memory->get(registers->SP + 1) == 0xab);
	registers->BC.bc = 0;
	commands->POP(POP_BC);
	assert(registers->SP == 0xd000);
	assert(registers->BC.bc == 0xabcd);

	registers->SP = 0xd000;
	registers->DE.de = 0xabcd;
	commands->PUSH(PUSH_DE);
	assert(registers->SP == 0xcffe);
	assert(memory->get(registers->SP) == 0xcd);
	assert(memory->get(registers->SP + 1) == 0xab);
	registers->DE.de = 0;
	commands->POP(POP_DE);
	assert(registers->SP == 0xd000);
	assert(registers->DE.de == 0xabcd);

	registers->SP = 0xd000;
	registers->HL.hl = 0xabcd;
	commands->PUSH(PUSH_HL);
	assert(registers->SP == 0xcffe);
	assert(memory->get(registers->SP) == 0xcd);
	assert(memory->get(registers->SP + 1) == 0xab);
	registers->HL.hl = 0;
	commands->POP(POP_HL);
	assert(registers->SP == 0xd000);
	assert(registers->HL.hl == 0xabcd);

	// RLC
	registers->HL.h = 0x9a;
	memory->resetFlag(C);
	commands->RLC(RLC_H);
	assert(registers->HL.h == 0x35);
	assert(memory->getFlag(C) == 1);

	registers->HL.h = 0x9a;
	memory->resetFlag(C);
	commands->RL(RL_H);
	assert(registers->HL.h == 0x34);
	assert(memory->getFlag(C) == 1);

	registers->BC.b = 0x9a;
	memory->resetFlag(C);
	commands->RL(RL_B);
	assert(registers->BC.b == 0x34);
	assert(memory->getFlag(C) == 1);

	// DAA
	clearFlags();
	registers->AF.a = 0x25;
	registers->DE.e = 0x17;
	commands->ADD(ADD_A_E, 0);
	assert(registers->AF.a == 0x3c);
	assert(memory->getFlag(H) == 0);
	assert(memory->getFlag(C) == 0);
	commands->DAA_();
	assert(registers->AF.a == 0x42);
	clearFlags();
	registers->AF.a = 0x60;
	registers->DE.e = 0x60;
	commands->ADD(ADD_A_E, 0);
	assert(registers->AF.a == 0xc0);
	assert(memory->getFlag(H) == 0);
	assert(memory->getFlag(C) == 0);
	commands->DAA_();
	assert(registers->AF.a == 0x20);
	clearFlags();
	registers->AF.a = 0x90;
	registers->DE.e = 0x90;
	commands->ADD(ADD_A_E, 0);
	assert(registers->AF.a == 0x20);
	assert(memory->getFlag(H) == 0);
	assert(memory->getFlag(C) == 1);
	commands->DAA_();
	assert(registers->AF.a == 0x80);
	clearFlags();
	registers->AF.a = 0x10;
	registers->DE.e = 0x01;
	commands->SUB(SUB_E, 0);
	assert(registers->AF.a == 0xf);
	assert(memory->getFlag(H) == 1);
	assert(memory->getFlag(C) == 0);
	commands->DAA_();
	assert(registers->AF.a == 0x09);

	for (int i = 1; i <= 10; i++) {
		for (int j = 0; j < 99; j++) {
			registers->AF.f = 0;
			registers->AF.a = daa_input[j];
			commands->ADD(ADD_A_n, i);
			commands->DAA_();
			assert(registers->AF.a == daa_output[i-1][j]);
		}
	}
	
	// ADC
	clearFlags();
	registers->BC.c = 0x67;
	registers->AF.a = 0x3;
	commands->ADC(ADC_A_C);
	assert(registers->AF.a == 0x6a);
	assert(memory->getFlag(Z) == 0);
	assert(memory->getFlag(C) == 0);
	assert(memory->getFlag(H) == 0);

	clearFlags();
	memory->setFlag(C);
	registers->AF.a = 0xf;
	registers->DE.e = 0;
	commands->ADC(ADC_A_E);
	assert(registers->AF.a == 0x10);
	assert(memory->getFlag(C) == 0);
	assert(memory->getFlag(H) == 1);

	clearFlags();
	memory->setFlag(C);
	registers->AF.a = 0xf;
	commands->ADC(ADC_A_n, 0x1);
	assert(registers->AF.a == 0x11);
	assert(memory->getFlag(C) == 0);
	assert(memory->getFlag(H) == 1);
	

	clearFlags();
	registers->AF.a = 0xff;
	registers->HL.hl = 0xdd56;
	memory->WriteMem(0xdd56, 0x1);
	commands->ADC(ADC_A_HL);
	assert(registers->AF.a == 0);
	assert(memory->getFlag(C) == 1);
	assert(memory->getFlag(H) == 1);
	assert(memory->getFlag(Z) == 1);


	// SUB
	clearFlags();
	registers->AF.a = 0x72;
	registers->BC.c = 0x15;
	commands->SUB(SUB_C, 0);
	assert(registers->AF.a == 0x5d);
	assert(memory->getFlag(H) == 1);
	assert(memory->getFlag(C) == 0);
	commands->DAA_();
	assert(registers->AF.a == 0x57);

	clearFlags();
	registers->AF.a = 0x82;
	registers->BC.b = 0x01;
	commands->SUB(SUB_B, 0);
	assert(registers->AF.a == 0x81);
	assert(memory->getFlag(H) == 0);
	assert(memory->getFlag(C) == 0);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	registers->AF.a = 0x82;
	registers->BC.b = 0x82;
	commands->SUB(SUB_B, 0);
	assert(registers->AF.a == 0x00);
	assert(memory->getFlag(H) == 0);
	assert(memory->getFlag(C) == 0);
	assert(memory->getFlag(Z) == 1);

	clearFlags();
	registers->AF.a = 0x81;
	commands->SUB(SUB_n, 0x83);
	assert(registers->AF.a == 0xfe);
	assert(memory->getFlag(H) == 1);
	assert(memory->getFlag(C) == 1);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	registers->AF.a = 0x12;
	commands->SUB(SUB_n, 0x05);
	assert(registers->AF.a == 0xd);
	assert(memory->getFlag(H) == 1);
	assert(memory->getFlag(C) == 0); 
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	registers->AF.a = 0x3;
	commands->SUB(SUB_n, 0x5);
	assert(registers->AF.a == 0xfe);
	assert(memory->getFlag(H) == 1);
	assert(memory->getFlag(C) == 1);
	assert(memory->getFlag(Z) == 0);

	// RLA
	clearFlags();
	registers->AF.a = 0xa1;
	commands->RLA_();
	assert(memory->getFlag(C) == 1);
	assert(registers->AF.a == 0x42);
	commands->RLA_();
	assert(memory->getFlag(C) == 0);
	assert(registers->AF.a == 0x85);

	// LD
	clearFlags();

	registers->BC.b = 0x04;
	registers->AF.a = 0x99;
	commands->LD(LD_A_B, 0, 0);
	assert(registers->AF.a == 0x04);

	registers->BC.c = 0xaa;
	registers->AF.a = 0x99;
	commands->LD(LD_A_C, 0, 0);
	assert(registers->AF.a == 0xaa);

	registers->DE.d = 0x87;
	registers->AF.a = 0x99;
	commands->LD(LD_A_D, 0, 0);
	assert(registers->AF.a == 0x87);

	registers->BC.c = 0x42;
	commands->LD(LD_C_n, 0x5a, 0);
	assert(registers->BC.c == 0x5a);

	registers->AF.a = 0x09;
	memory->set(0x8001, 0x76);
	registers->DE.de = 0x8001;
	commands->LD(LD_A_DE, 0, 0);
	assert(registers->AF.a == 0x76);

	registers->BC.b = 0x42;
	commands->LD(LD_B_n, 0x5a, 0);
	assert(registers->BC.b == 0x5a);

	registers->AF.a = 0x09;
	memory->set(0xff50, 0x40);
	commands->LD(LDH_A_n, 0x50, 0);
	assert(registers->AF.a == 0x40);

	registers->AF.a = 0x09;
	memory->set(0xffff, 0x40);
	commands->LD(LDH_n_A, 0xff, 0);
	assert(memory->get(0xffff) == 0x09);

	commands->LD(LD_BC_nn, 0x1e, 0xfb);
	assert(registers->BC.bc == 0xfb1e);

	registers->DE.d = 0;
	commands->LD(LD_D_n, 0x45);
	assert(registers->DE.d == 0x45);

	registers->AF.a = 0x01;
	registers->HL.l = 0x25;
	registers->HL.h = 0x65;
	registers->DE.e = 0x0d;
	commands->LD(LD_A_L, 0, 0);
	assert(registers->AF.a == 0x25);
	commands->LD(LD_A_H, 0, 0);
	assert(registers->AF.a == 0x65);
	commands->LD(LD_A_E, 0, 0);
	assert(registers->AF.a == 0x0d);

	commands->LD(LD_BC_nn, 0x1b, 0xd8);
	assert(registers->BC.bc == 0xd81b);
	commands->LD(LD_DE_nn, 0x1b, 0xd8);
	assert(registers->DE.de == 0xd81b);
	commands->LD(LD_SP_nn, 0x1b, 0xd8);
	assert(registers->SP == 0xd81b);
	commands->LD(LD_HL_nn, 0x1b, 0xd8);
	assert(registers->HL.hl == 0xd81b);
	memory->WriteMem(0xd81b, 0x80);
	commands->LD(LD_A_HLP, 0, 0);
	assert(registers->AF.a == 0x80);
	assert(registers->HL.hl == 0xd81c);

	registers->DE.d = 0x76;
	registers->HL.hl = 0xdd20;
	commands->LD(LD_HL_D);
	assert(memory->ReadMem(0xdd20) == 0x76);

	registers->HL.hl = 0xd81c;
	registers->AF.a = 0x99;
	commands->LD(LD_HL_A);
	assert(memory->ReadMem(0xd81c) == 0x99);
	commands->LD(LD_L_HL, 0, 0);
	assert(registers->HL.l == 0x99);

	registers->HL.hl = 0xdd11;
	memory->WriteMem(0xdd11, 0x50);
	commands->LD(LD_A_HLM);
	assert(registers->AF.a == 0x50);
	assert(registers->HL.hl == 0xdd10);

	memory->WriteMem(0xdd34, 0x8);
	commands->LD(LD_A_nn, 0x34, 0xdd);
	assert(registers->AF.a == 0x8);

	registers->AF.a = 0;
	commands->LD(LD_A_n, 0x54);
	assert(registers->AF.a == 0x54);

	memory->WriteMem(0xff22, 0x16);
	commands->LD(LDH_A_n, 0x22);
	assert(registers->AF.a == 0x16);

	registers->AF.a = 0x12;
	commands->LD(LD_nn_A, 0x99, 0xdd);
	assert(memory->ReadMem(0xdd99) == 0x12);

	registers->HL.h = 0x13;
	commands->LD(LD_H_n, 0x45);
	assert(registers->HL.h = 0x45);

	registers->HL.hl = 0xdd51;
	registers->AF.a = 0x9;
	commands->LD(LD_HLP_A);
	assert(memory->ReadMem(0xdd51) == 0x9);

	registers->AF.a = 0x91;
	registers->BC.c = 0x3f;
	memory->WriteMem(0xff3f, 0);
	commands->LD(LD__C__A);
	assert(memory->ReadMem(0xff3f) == 0x91);

	registers->AF.a = 0;
	registers->BC.c = 0x3f;
	memory->WriteMem(0xff3f, 0x91);
	commands->LD(LD_A__C_);
	assert(registers->AF.a == 0x91);

	// CP
	clearFlags();
	registers->AF.a = 0x65;
	registers->BC.b = 0x03;
	commands->CP(CP_B, 0);
	assert(memory->getFlag(Z) == 0);
	assert(memory->getFlag(C) == 0);

	clearFlags();
	registers->AF.a = 0x6a;
	registers->BC.b = 0x6a;
	commands->CP(CP_B, 0);
	assert(memory->getFlag(Z) == 1);
	assert(memory->getFlag(C) == 0);

	clearFlags();
	registers->AF.a = 0x6;
	registers->BC.b = 0x30;
	commands->CP(CP_B, 0);
	assert(memory->getFlag(Z) == 0);
	assert(memory->getFlag(C) == 1);

	clearFlags();
	registers->AF.a = 0x65;
	commands->CP(CP_n, 0x3);
	assert(memory->getFlag(Z) == 0);
	assert(memory->getFlag(C) == 0);

	clearFlags();
	registers->AF.a = 0x6a;
	commands->CP(CP_n, 0x6a);
	assert(memory->getFlag(Z) == 1);
	assert(memory->getFlag(C) == 0);

	clearFlags();
	registers->AF.a = 0x6;
	commands->CP(CP_n, 0x30);
	assert(memory->getFlag(Z) == 0);
	assert(memory->getFlag(C) == 1);

	clearFlags();
	registers->AF.a = 0x65;
	registers->HL.hl = 0x8002;
	memory->set(0x8002, 0x3);
	commands->CP(CP_HL, 0);
	assert(memory->getFlag(Z) == 0);
	assert(memory->getFlag(C) == 0);

	clearFlags();
	registers->AF.a = 0x6a;
	registers->HL.hl = 0x8002;
	memory->set(0x8002, 0x6a);
	commands->CP(CP_HL, 0);
	assert(memory->getFlag(Z) == 1);
	assert(memory->getFlag(C) == 0);

	clearFlags();
	registers->AF.a = 0x6;
	registers->HL.hl = 0x8002;
	memory->set(0x8002, 0x30);
	commands->CP(CP_HL, 0);
	assert(memory->getFlag(Z) == 0);
	assert(memory->getFlag(C) == 1);

	clearFlags();
	registers->AF.a = 0x8;
	commands->CP(CP_n, 0x7);
	assert(memory->getFlag(Z) == 0);
	clearFlags();
	registers->AF.a = 0x7;
	commands->CP(CP_n, 0x7);
	assert(memory->getFlag(Z) == 1);

	// DEC
	clearFlags();
	registers->DE.e = 0x90;
	commands->DEC(DEC_E);
	assert(registers->DE.e == 0x8f);
	assert(memory->getFlag(Z) == 0);
	assert(memory->getFlag(N) == 1);

	clearFlags();
	registers->DE.e = 0x01;
	commands->DEC(DEC_E);
	assert(registers->DE.e == 0x00);
	assert(memory->getFlag(Z) == 1);
	assert(memory->getFlag(N) == 1);

	clearFlags();
	registers->DE.e = 0x00;
	commands->DEC(DEC_E);
	assert(registers->DE.e == 0xff);
	assert(memory->getFlag(Z) == 0);
	assert(memory->getFlag(N) == 1);

	clearFlags();
	registers->AF.a = 0x90;
	commands->DEC(DEC_A);
	assert(registers->AF.a == 0x8f);
	assert(memory->getFlag(Z) == 0);
	assert(memory->getFlag(N) == 1);

	clearFlags();
	registers->AF.a = 0x01;
	commands->DEC(DEC_A);
	assert(registers->AF.a == 0x00);
	assert(memory->getFlag(Z) == 1);
	assert(memory->getFlag(N) == 1);

	clearFlags();
	registers->AF.a = 0x00;
	commands->DEC(DEC_A);
	assert(registers->AF.a == 0xff);
	assert(memory->getFlag(Z) == 0);
	assert(memory->getFlag(N) == 1);

	clearFlags();
	registers->HL.hl = 0xd144;
	memory->WriteMem(0xd144, 0x7);
	commands->DEC(DEC__HL_);
	assert(memory->ReadMem(0xd144) == 0x6);
	assert(memory->getFlag(Z) == 0);
	assert(memory->getFlag(N) == 1);

	registers->HL.hl = 0xde51;
	registers->AF.a = 0x90;
	commands->LD(LD_HLM_A);
	assert(memory->ReadMem(0xde51) == 0x90);
	assert(registers->HL.hl == 0xde50);

	// INC
	clearFlags();
	registers->HL.hl = 0x1002;
	commands->INC(INC_HL);
	assert(registers->HL.hl == 0x1003);
	assert(memory->getFlag(H) == 0);
	assert(memory->getFlag(Z) == 0);
	assert(memory->getFlag(N) == 0);

	clearFlags();
	registers->HL.hl = 0xffff;
	commands->INC(INC_HL);
	assert(registers->HL.hl == 0x0);
	assert(memory->getFlag(H) == 0);
	assert(memory->getFlag(Z) == 0);
	assert(memory->getFlag(N) == 0);

	clearFlags();
	registers->DE.e = 0xff;
	commands->INC(INC_E);
	assert(registers->DE.e == 0x0);
	assert(memory->getFlag(H) == 1);
	assert(memory->getFlag(Z) == 1);
	assert(memory->getFlag(N) == 0);

	clearFlags();
	registers->DE.e = 0x0f;
	commands->INC(INC_E);
	assert(registers->DE.e == 0x10);
	assert(memory->getFlag(H) == 1);
	assert(memory->getFlag(Z) == 0);
	assert(memory->getFlag(N) == 0);

	clearFlags();
	registers->BC.bc = 0x0fff;
	commands->INC(INC_BC);
	assert(registers->BC.bc == 0x1000);

	

	// RRA / RRCA / RLA / RRCA
	clearFlags();
	registers->AF.a = 0xb5;
	commands->RRCA_();
	assert(registers->AF.a == 0xda);
	assert(memory->getFlag(C) == 1);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	registers->AF.a = 0xb4;
	commands->RRCA_();
	assert(registers->AF.a == 0x5a);
	assert(memory->getFlag(C) == 0);

	clearFlags();
	registers->AF.a = 0;
	commands->RRCA_();
	assert(registers->AF.a == 0);
	assert(memory->getFlag(C) == 0);

	clearFlags();
	registers->AF.a = 0xb5;
	commands->RRA();
	assert(registers->AF.a == 0x5a);
	assert(memory->getFlag(C) == 1);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	registers->AF.a = 0xb4;
	commands->RRA();
	assert(registers->AF.a == 0x5a);
	assert(memory->getFlag(C) == 0);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	registers->AF.a = 0;
	commands->RRA();
	assert(registers->AF.a == 0);
	assert(memory->getFlag(C) == 0);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	memory->setFlag(C);
	registers->AF.a = 0xb5;
	commands->RRA();
	assert(registers->AF.a == 0xda);
	assert(memory->getFlag(C) == 1);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	registers->AF.a = 0xb5;
	commands->RLCA_();
	assert(registers->AF.a == 0x6b);
	assert(memory->getFlag(C) == 1);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	registers->AF.a = 0x6b;
	commands->RLCA_();
	assert(registers->AF.a == 0xd6);
	assert(memory->getFlag(C) == 0);

	clearFlags();
	registers->AF.a = 0;
	commands->RLCA_();
	assert(registers->AF.a == 0);
	assert(memory->getFlag(C) == 0);

	clearFlags();
	registers->AF.a = 0xb5;
	commands->RLA_();
	assert(registers->AF.a == 0x6a);
	assert(memory->getFlag(C) == 1);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	registers->AF.a = 0x35;
	commands->RLA_();
	assert(registers->AF.a == 0x6a);
	assert(memory->getFlag(C) == 0);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	registers->AF.a = 0;
	commands->RLA_();
	assert(registers->AF.a == 0);
	assert(memory->getFlag(C) == 0);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	memory->setFlag(C);
	registers->AF.a = 0xb5;
	commands->RLA_();
	assert(registers->AF.a == 0x6b);
	assert(memory->getFlag(C) == 1);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	memory->setFlag(C);
	registers->HL.l = 0x53;
	commands->RR(RR_L);
	assert(registers->HL.l == 0xa9);
	assert(memory->getFlag(C) == 1);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	registers->HL.l = 0x53;
	commands->RR(RR_L);
	assert(registers->HL.l == 0x29);
	assert(memory->getFlag(C) == 1);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	registers->BC.b = 0x53;
	commands->RR(RR_B);
	assert(registers->BC.b == 0x29);
	assert(memory->getFlag(C) == 1);
	assert(memory->getFlag(Z) == 0);
	clearFlags();
	registers->BC.c = 0x53;
	commands->RR(RR_C);
	assert(registers->BC.c == 0x29);
	assert(memory->getFlag(C) == 1);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	memory->setFlag(C);
	registers->HL.l = 0x53;
	commands->RL(RL_L);
	assert(registers->HL.l == 0xa7);
	assert(memory->getFlag(C) == 0);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	registers->HL.l = 0x53;
	commands->RL(RL_L);
	assert(registers->HL.l == 0xa6);
	assert(memory->getFlag(C) == 0);
	assert(memory->getFlag(Z) == 0);

	// SRA / SLA
	clearFlags();
	registers->HL.l = 0x9d;
	commands->SRA(SRA_L);
	assert(registers->HL.l == 0xce);
	assert(memory->getFlag(C) == 1);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	registers->AF.a = 0x9c;
	commands->SRA(SRA_A);
	assert(registers->AF.a == 0xce);
	assert(memory->getFlag(C) == 0);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	registers->HL.l = 0x1d;
	commands->SRA(SRA_L);
	assert(registers->HL.l == 0xe);
	assert(memory->getFlag(C) == 1);
	assert(memory->getFlag(Z) == 0); 

	clearFlags();
	registers->DE.e = 0xbf;
	commands->SRA(SRA_E);
	assert(registers->DE.e == 0xdf);
	assert(memory->getFlag(C) == 1);
	assert(memory->getFlag(Z) == 0); 

	clearFlags();
	registers->BC.b = 0xff;
	commands->SLA(SLA_B);
	assert(registers->BC.b == 0xfe);
	assert(memory->getFlag(C) == 1);
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	registers->BC.c = 0;
	commands->SLA(SLA_C);
	assert(registers->BC.c == 0);
	assert(memory->getFlag(C) == 0);
	assert(memory->getFlag(Z) == 1);

	clearFlags();
	registers->DE.d = 1;
	commands->SLA(SLA_D);
	assert(registers->DE.d == 0x2);
	assert(memory->getFlag(C) == 0);
	assert(memory->getFlag(Z) == 0);

	// SET / RES / BIT
	registers->BC.b = 0;
	commands->SET(SET2_B, 2);
	assert(registers->BC.b == 0x4);
	commands->RES(RES2_B, 2);
	assert(registers->BC.b == 0);
	registers->HL.hl = 0xe055;
	memory->WriteMem(0xe055, 0);
	commands->SET(SET2_HL, 7);
	assert(memory->get(0xe055) == 0x80);
	commands->RES(RES2_HL, 7);
	assert(memory->get(0xe055) == 0x0);

	clearFlags();
	memory->setFlag(N);
	registers->DE.e = 0x94;
	commands->BIT(BIT4_E, 4);
	assert(memory->getFlag(Z) == 0);
	assert(memory->getFlag(H) == 1);
	assert(memory->getFlag(N) == 0);
	commands->BIT(BIT3_E, 3);
	assert(memory->getFlag(Z) == 1);
	commands->BIT(BIT7_E, 7);
	assert(memory->getFlag(Z) == 0);
	registers->HL.hl = 0xe055;
	memory->WriteMem(0xe055, 0x94);
	commands->BIT(BIT0_HL, 0);
	assert(memory->getFlag(Z) == 1);
	commands->BIT(BIT7_HL, 7);
	assert(memory->getFlag(Z) == 0);

	// JR / JP
	uint8_t skip = 0;
	registers->HL.hl = 0xe055;
	commands->JR(JP_HL, 0, 0, &skip);
	assert(registers->PC == 0xe055);
	commands->JR(JP_nn, 0x44, 0xe1, &skip);
	assert(registers->PC == 0xe144);
	
	clearFlags();
	registers->PC = 0xe122;
	commands->JR(JP_Z_nn, 0x44, 0xe1, &skip);
	assert(registers->PC == 0xe122);
	memory->setFlag(Z);
	commands->JR(JP_Z_nn, 0x44, 0xe1, &skip);
	assert(registers->PC == 0xe144);

	clearFlags();
	registers->PC = 0xe122;
	commands->JR(JP_NC_nn, 0x44, 0xe1, &skip);
	assert(registers->PC == 0xe144);
	memory->setFlag(C);
	registers->PC = 0xe122;
	commands->JR(JP_NC_nn, 0x44, 0xe1, &skip);
	assert(registers->PC == 0xe122);

	registers->PC = 0xe122;
	commands->JR(JR_n, 0x50, 0, &skip);
	assert(registers->PC == 0xe172);

	clearFlags();
	registers->PC = 0xe122;
	commands->JR(JR_C_n, 0x50, 0, &skip);
	assert(registers->PC == 0xe122);
	commands->JR(JR_NC_n, 0x50, 0, &skip);
	assert(registers->PC == 0xe172);
	registers->PC = 0xe122;
	memory->setFlag(C);
	commands->JR(JR_NC_n, 0x50, 0, &skip);
	assert(registers->PC == 0xe122);
	commands->JR(JR_C_n, 0x50, 0, &skip);
	assert(registers->PC == 0xe172);

	clearFlags();
	registers->PC = 0xe122;
	commands->JR(JR_Z_n, 0x50, 0, &skip);
	assert(registers->PC == 0xe122);
	commands->JR(JR_NZ_n, 0x50, 0, &skip);
	assert(registers->PC == 0xe172);
	registers->PC = 0xe122;
	memory->setFlag(Z);
	commands->JR(JR_NZ_n, 0x50, 0, &skip);
	assert(registers->PC == 0xe122);
	commands->JR(JR_Z_n, 0x50, 0, &skip);
	assert(registers->PC == 0xe172);

	// LD

	// CPL / CCF / SCF
	// registers->AF.a = 0x38;
	

	// CALL / RET
	uint8_t skipPCIntTest = 0;
	registers->PC = 0x1000;
	registers->SP = 0xc789;
	commands->CALL(CALL_nn, 0x45, 0x7a);
	assert(registers->PC == 0x7a45);
	assert(registers->SP == 0xc787);
	assert(memory->get(registers->SP) == 0x03);
	assert(memory->get(registers->SP + 1) == 0x10);
	commands->RET_(RET, &skipPCIntTest);
	assert(registers->PC == 0x1003);
	assert(registers->SP == 0xc789);


	registers->PC = 0x000;

	// Test sprite data
	clearFlags();
	uint8_t spriteNum = 2;
	uint8_t spriteData = 0xae; // 10101110
	uint8_t _x = 0x90;
	uint8_t _y = 0xf1;
	uint8_t tileNum = 0x45;
	uint16_t address = 0xfe08;
	memory->set(address, _y);
	memory->set(address+1, _x);
	memory->set(address+2, tileNum);
	memory->set(address+3, spriteData);
	Sprite *sprite = new Sprite();
	GBCDraw *draw = new GBCDraw(memory, registers);
	draw->GetSpriteByNumber(spriteNum, sprite);
	assert(sprite->X == _x);
	assert(sprite->Y == _y);
	assert(sprite->TileNumber == tileNum);
	assert(sprite->PaletteBank == 0);
	assert(sprite->CGBPalette == 0x6);
	assert(sprite->XFlip == 1);
	assert(sprite->YFlip == 0);
	assert(sprite->SpritePriority == 1);

}

void Test::clearFlags() {
	memory->resetFlag(Z);
	memory->resetFlag(N);
	memory->resetFlag(C);
	memory->resetFlag(H);
}
