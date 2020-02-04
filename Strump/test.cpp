#include "test.h"

#include <assert.h>


Test::Test(Commands* _commands, Memory* _memory, Registers* _registers) {
	commands = _commands;
	memory = _memory;
	registers = _registers;
}
Test::~Test() {}


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
	commands->OR(OR_C, 0);
	assert(registers->AF.a == 0x9b);
	registers->BC.c = 0x4;
	commands->OR(OR_C, 0);
	assert(registers->AF.a == 0x9f);

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
	assert(memory->get(registers->SP) == 0x24);
	assert(memory->get(registers->SP + 1) == 0x50);
	registers->AF.af = 0;
	commands->POP(POP_AF);
	assert(registers->SP == 0xe010);
	assert(registers->AF.af == 0x2450);

	registers->SP = 0xd000;
	registers->BC.bc = 0xabcd;
	commands->PUSH(PUSH_BC);
	assert(registers->SP == 0xcffe);
	assert(memory->get(registers->SP) == 0xab);
	assert(memory->get(registers->SP + 1) == 0xcd);
	registers->BC.bc = 0;
	commands->POP(POP_BC);
	assert(registers->SP == 0xd000);
	assert(registers->BC.bc == 0xabcd);

	registers->SP = 0xd000;
	registers->DE.de = 0xabcd;
	commands->PUSH(PUSH_DE);
	assert(registers->SP == 0xcffe);
	assert(memory->get(registers->SP) == 0xab);
	assert(memory->get(registers->SP + 1) == 0xcd);
	registers->DE.de = 0;
	commands->POP(POP_DE);
	assert(registers->SP == 0xd000);
	assert(registers->DE.de == 0xabcd);

	registers->SP = 0xd000;
	registers->HL.hl = 0xabcd;
	commands->PUSH(PUSH_HL);
	assert(registers->SP == 0xcffe);
	assert(memory->get(registers->SP) == 0xab);
	assert(memory->get(registers->SP + 1) == 0xcd);
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

	clearFlags();
	registers->AF.a = 0x25;
	registers->DE.e = 0x17;
	commands->ADD(ADD_A_E, 0);
	assert(registers->AF.a == 0x3c);
	assert(memory->getFlag(H) == 0);
	assert(memory->getFlag(C) == 0);
	commands->DAA_();
	assert(registers->AF.a == 0x42);

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
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	registers->AF.a = 0;
	commands->RRCA_();
	assert(registers->AF.a == 0);
	assert(memory->getFlag(C) == 0);
	assert(memory->getFlag(Z) == 1);

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
	assert(memory->getFlag(Z) == 1);

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
	assert(memory->getFlag(Z) == 0);

	clearFlags();
	registers->AF.a = 0;
	commands->RLCA_();
	assert(registers->AF.a == 0);
	assert(memory->getFlag(C) == 0);
	assert(memory->getFlag(Z) == 1);

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
	assert(memory->getFlag(Z) == 1);

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
	registers->HL.l = 0x1d;
	commands->SRA(SRA_L);
	assert(registers->HL.l == 0xe);
	assert(memory->getFlag(C) == 1);
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
	assert(memory->get(registers->SP) == 0x10);
	assert(memory->get(registers->SP + 1) == 0x03);
	commands->RET_(RET, &skipPCIntTest);
	assert(registers->PC == 0x1003);
	assert(registers->SP == 0xc789);


	registers->PC = 0x000;
}

void Test::clearFlags() {
	memory->resetFlag(Z);
	memory->resetFlag(N);
	memory->resetFlag(C);
	memory->resetFlag(H);
}
