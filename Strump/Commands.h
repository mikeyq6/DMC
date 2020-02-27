#pragma once

#ifndef COMMANDS_H
#define COMMANDS_H

#include "alias.h"
#include "registers.h"
#include "SDL_Config.h"
#include "Memory.h"
#include "registers.h"
#include <sstream>

class Commands {

private:
	Memory* memory;
	Registers* registers;

	bool IsHalfCarry(uint8_t val1, uint8_t val2);
	bool IsHalfCarry(uint8_t val1, int8_t val2);
	bool IsHalfCarry(uint16_t val1, uint8_t val2);
	bool IsHalfCarry(uint16_t val1, int8_t val2);
	bool IsHalfCarry(uint16_t val1, uint16_t val2);

public:
	Commands(Memory* _memory, Registers* _registers);
	~Commands();
	
	const string CodeToString(uint8_t opcode, uint16_t PC, uint16_t param1, uint16_t param2);
	const string CBCodeToString(uint8_t opcode);

	void XOR(uint8_t opcode);
	void XOR(uint8_t opcode, uint8_t param);
	void OR(uint8_t opcode);
	void OR(uint8_t opcode, uint8_t param);
	void AND(uint8_t opcode);
	void AND(uint8_t opcode, uint8_t param);
	void ADD(uint8_t opcode);
	void ADD(uint8_t opcode, uint8_t param);
	void ADC(uint8_t opcode);
	void ADC(uint8_t opcode, uint8_t param);
	void SUB(uint8_t opcode);
	void SUB(uint8_t opcode, uint8_t param);
	void SBC(uint8_t opcode);
	void SBC(uint8_t opcode, uint8_t param);
	void CP(uint8_t opcode);
	void CP(uint8_t opcode, uint8_t param);
	void LD(uint8_t opcode);
	void LD(uint8_t opcode, uint8_t param);
	void LD(uint8_t opcode, uint8_t param, uint8_t param2);
	void DEC(uint8_t opcode);
	void INC(uint8_t opcode);
	void JR(uint8_t opcode, uint8_t param1, uint8_t param2, uint8_t* skipPCInc);
	void CALL(uint8_t opcode, uint8_t param1, uint8_t param2);
	void RET_(uint8_t opcode, uint8_t* skipPCInc);
	void DAA_();
	void RRCA_();
	void RRA();
	void RLCA_();
	void RLA_();

	void PUSH(uint8_t opcode);
	void POP(uint8_t opcode);
	void RRC(uint8_t opcode);
	void RLC(uint8_t opcode);
	void RL(uint8_t opcode);
	void RR(uint8_t opcode);
	void RES(uint8_t opcode, uint8_t bit_n);
	void SET(uint8_t opcode, uint8_t bit_n);
	void BIT(uint8_t opcode, int bit_n);
	void RST(uint8_t opcode);
	void SWAP(uint8_t opcode);
	void SRL(uint8_t opcode);
	void SRA(uint8_t opcode);
	void SLA(uint8_t opcode);

};

#endif