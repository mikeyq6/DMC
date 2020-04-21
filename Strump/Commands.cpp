#include "Commands.h"
#include "opcodes.h"
#include <iomanip>

Commands::Commands(Memory* _memory, Registers* _registers) {
	memory = _memory;
	registers = _registers;
}
Commands::~Commands() {}


bool Commands::IsHalfCarry(uint8_t val1, uint8_t val2) {
	if ((((val1 & 0xf) + (val2 & 0xf)) & 0x10) == 0x10)
		return true;
	else
		return false;
}
bool Commands::IsHalfCarry(uint8_t val1, int8_t val2) {
	if ((((val1 & 0xf) + (val2 & 0xf)) & 0x10) == 0x10)
		return true;
	else
		return false;
}
bool Commands::IsHalfCarry(uint16_t val1, uint8_t val2) {
	if ((((val1 & 0xf) + (val2 & 0xf)) & 0x10) == 0x10)
		return true;
	else
		return false;
}
bool Commands::IsHalfCarry(uint16_t val1, int8_t val2) {
	if ((((val1 & 0xf) + (val2 & 0xf)) & 0x10) == 0x10)
		return true;
	else
		return false;
}
bool Commands::IsHalfCarry(uint16_t val1, uint16_t val2) {
	if ((((val1 & 0xfff) + (val2 & 0xfff)) & 0x1000) == 0x1000)
		return true;
	else
		return false;
}

#pragma region ALU methods

void Commands::ADD(uint8_t opcode) {
	return ADD(opcode, 0);
}
void Commands::ADD(uint8_t opcode, uint8_t param) {
	uint8_t a, val = 0;
	uint16_t val16 = 0;
	uint16_t oldHL = registers->HL.hl;
	uint16_t oldSP = registers->SP;
	uint32_t sum = 0;
	int8_t paramS = (int8_t)param;

	a = registers->AF.a;
	memory->resetFlag(N);

	switch (opcode) {
		case ADD_A_A:
			val = registers->AF.a; registers->AF.a += registers->AF.a; break;
		case ADD_A_B:
			registers->AF.a += registers->BC.b; val = registers->BC.b; break;
		case ADD_A_C:
			registers->AF.a += registers->BC.c; val = registers->BC.c; break;
		case ADD_A_D:
			registers->AF.a += registers->DE.d; val = registers->DE.d; break;
		case ADD_A_E:
			registers->AF.a += registers->DE.e; val = registers->DE.e; break;
		case ADD_A_H:
			registers->AF.a += registers->HL.h; val = registers->HL.h; break;
		case ADD_A_L:
			registers->AF.a += registers->HL.l; val = registers->HL.l; break;
		case ADD_A_HL:
			val = memory->ReadMem(registers->HL.hl); registers->AF.a += val; break;
		case ADD_SP_n:
			if(IsHalfCarry(registers->SP, paramS))
				memory->setFlag(H);
			else
				memory->resetFlag(H);
			if ((((registers->SP & 0xff) + (paramS & 0xff)) & 0x100) == 0x100)
				memory->setFlag(C);
			else
				memory->resetFlag(C);
			registers->SP += paramS;
			memory->resetFlag(Z); break; // convert to signed
		case ADD_HL_BC:
			registers->HL.hl += registers->BC.bc; val16 = registers->BC.bc; break;
		case ADD_HL_DE:
			registers->HL.hl += registers->DE.de; val16 = registers->DE.de; break;
		case ADD_HL_HL:
			val16 = registers->HL.hl; registers->HL.hl += registers->HL.hl; break; // Get the val before we do the addition
		case ADD_HL_SP:
			registers->HL.hl += registers->SP; val16 = registers->SP; break;
		case ADD_A_n:
			registers->AF.a += param; val = param; break;
	}

	if (opcode == ADD_HL_BC || opcode == ADD_HL_DE || opcode == ADD_HL_HL || opcode == ADD_HL_SP) {
		if (IsHalfCarry(oldHL, val16))
		//if (((oldHL & 0xfff) + (val16 & 0xfff)) > 0xfff)
			memory->setFlag(H);
		else
			memory->resetFlag(H);

		sum = oldHL + val16;
		if (sum > 0xffff)
			memory->setFlag(C);
		else
			memory->resetFlag(C);
	}
	else if(opcode != ADD_SP_n){
		if (IsHalfCarry(a, val))
		//if ((((a & 0xf) + (val & 0xf)) & 0x10) == 0x10)
			memory->setFlag(H);
		else
			memory->resetFlag(H);

		sum = a + val;
		if (sum > 0xff)
			memory->setFlag(C);
		else
			memory->resetFlag(C);

		if (registers->AF.a == 0) {
			memory->setFlag(Z);
		}
		else {
			memory->resetFlag(Z);
		}
	}
}
void Commands::ADC(uint8_t opcode) {
	return ADC(opcode, 0);
}
void Commands::ADC(uint8_t opcode, uint8_t param) {
	uint8_t a, val = 0;
	uint32_t sum = 0;

	a = registers->AF.a;
	short flag = memory->getFlag(C);

	switch (opcode) {
		case ADC_A_A:
			val = registers->AF.a; registers->AF.a += registers->AF.a + flag; break;
		case ADC_A_B:
			registers->AF.a += registers->BC.b + flag; val = registers->BC.b; break;
		case ADC_A_C:
			registers->AF.a += registers->BC.c + flag; val = registers->BC.c; break;
		case ADC_A_D:
			registers->AF.a += registers->DE.d + flag; val = registers->DE.d; break;
		case ADC_A_E:
			registers->AF.a += registers->DE.e + flag; val = registers->DE.e; break;
		case ADC_A_H:
			registers->AF.a += registers->HL.h + flag; val = registers->HL.h; break;
		case ADC_A_L:
			registers->AF.a += registers->HL.l + flag; val = registers->HL.l; break;
		case ADC_A_HL:
			val = memory->ReadMem(registers->HL.hl); registers->AF.a += val + flag; break;
		case ADC_A_n:
			registers->AF.a += param + flag; val = param; break;
	}

	// if (IsHalfCarry(a, (uint8_t)(val + flag)))
	if ((((a & 0xf) + (val & 0xf) + flag) & 0x10) == 0x10)
		memory->setFlag(H);
	else
		memory->resetFlag(H);

	sum = a + val + flag;
	if (sum > 0xff)
		memory->setFlag(C);
	else
		memory->resetFlag(C);

	if (registers->AF.a == 0) {
		memory->setFlag(Z);
	}
	else {
		memory->resetFlag(Z);
	}
	memory->resetFlag(N);
}

void Commands::SUB(uint8_t opcode) {
	return SUB(opcode, 0);
}
void Commands::SUB(uint8_t opcode, uint8_t param) {
	memory->resetFlag(Z);
	memory->setFlag(N);
	memory->resetFlag(H);
	memory->resetFlag(C);

	uint8_t val = 0;
	uint8_t oldA = registers->AF.a;

	switch (opcode) {
	case SUB_A:
		val = registers->AF.a; registers->AF.a -= registers->AF.a; break;
	case SUB_B:
		registers->AF.a -= registers->BC.b; val = registers->BC.b; break;
	case SUB_C:
		registers->AF.a -= registers->BC.c; val = registers->BC.c; break;
	case SUB_D:
		registers->AF.a -= registers->DE.d; val = registers->DE.d; break;
	case SUB_E:
		registers->AF.a -= registers->DE.e; val = registers->DE.e; break;
	case SUB_H:
		registers->AF.a -= registers->HL.h; val = registers->HL.h; break;
	case SUB_L:
		registers->AF.a -= registers->HL.l; val = registers->HL.l; break;
	case SUB_n:
		registers->AF.a -= param; val = param; break;
	case SUB_HL:
		val = memory->ReadMem(registers->HL.hl); registers->AF.a -= val; break;
	}

	if (registers->AF.a == 0) { memory->setFlag(Z); }

	// Half carry and full carry
	//printf("registers->AF.a = 0x%x, val = 0x%x, (registers->AF.a & 0xf) - (val & 0xf) = 0x%x\n", registers->AF.a, val, (registers->AF.a & 0xf) - (val & 0xf));
	if ((oldA & 0xf) < (val & 0xf)) {
		memory->setFlag(H);
	}
	if (oldA < val) {
		memory->setFlag(C);
	}
}

void Commands::SBC(uint8_t opcode) {
	return SBC(opcode, 0);
}
void Commands::SBC(uint8_t opcode, uint8_t param) {
	uint8_t cFlag = memory->getFlag(C);
	memory->resetFlag(Z);
	memory->setFlag(N);
	memory->resetFlag(H);
	memory->resetFlag(C);

	uint8_t oldA = registers->AF.a;
	uint8_t res = 0;
	uint8_t valToSub = 0;

	switch (opcode) {
		case SBC_A_A:
			valToSub = registers->AF.a; break;
		case SBC_A_B:
			valToSub = registers->BC.b; break;
		case SBC_A_C:
			valToSub = registers->BC.c; break;
		case SBC_A_D:
			valToSub = registers->DE.d; break;
		case SBC_A_E:
			valToSub = registers->DE.e; break;
		case SBC_A_H:
			valToSub = registers->HL.h; break;
		case SBC_A_L:
			valToSub = registers->HL.l; break;
		case SBC_A_HL:
			valToSub = memory->ReadMem(registers->HL.hl); break;
		case SBC_A_n:
			valToSub = param; break;
	}

	res = registers->AF.a - valToSub;
	res -= cFlag;
	registers->AF.a = res;

	if (registers->AF.a == 0) { memory->setFlag(Z); }

	// Half carry and full carry
	if (((oldA & 0xf) - (valToSub & 0xf) - cFlag) < 0) {
		memory->setFlag(H);
	}
	if ((oldA - valToSub - cFlag) < 0) {
		memory->setFlag(C);
	}
}

#pragma endregion

#pragma region Loads

void Commands::LD(uint8_t opcode) {
	return LD(opcode, 0, 0);
}
void Commands::LD(uint8_t opcode, uint8_t param1) {
	return LD(opcode, param1, 0);
}
void Commands::LD(uint8_t opcode, uint8_t param1, uint8_t param2) {
	uint16_t address = (param2 << 8) + param1;
	uint16_t temp = address;
	uint32_t t = 0;
	uint8_t tempVal = 0;

	switch (opcode) {
	case LD_A_A:
		break; // No effect
	case LD_A_B:
		registers->AF.a = registers->BC.b; break;
	case LD_A_C:
		registers->AF.a = registers->BC.c; break;
	case LD_A_D:
		registers->AF.a = registers->DE.d; break;
	case LD_A_E:
		registers->AF.a = registers->DE.e; break;
	case LD_A_H:
		registers->AF.a = registers->HL.h; break;
	case LD_A_L:
		registers->AF.a = registers->HL.l; break;
	case LD_A_HL:
		registers->AF.a = memory->ReadMem(registers->HL.hl); break;
	case LD_A_n:
		registers->AF.a = param1; break;
	case LD_B_B:
		break;
	case LD_B_A:
		registers->BC.b = registers->AF.a; break;
	case LD_B_C:
		registers->BC.b = registers->BC.c; break;
	case LD_B_D:
		registers->BC.b = registers->DE.d; break;
	case LD_B_E:
		registers->BC.b = registers->DE.e; break;
	case LD_B_H:
		registers->BC.b = registers->HL.h; break;
	case LD_B_L:
		registers->BC.b = registers->HL.l; break;
	case LD_B_HL:
		registers->BC.b = memory->ReadMem(registers->HL.hl); break;
	case LD_B_n:
		registers->BC.b = param1; break;
	case LD_C_C:
		break;
	case LD_C_A:
		registers->BC.c = registers->AF.a; break;
	case LD_C_B:
		registers->BC.c = registers->BC.b; break;
	case LD_C_D:
		registers->BC.c = registers->DE.d; break;
	case LD_C_E:
		registers->BC.c = registers->DE.e; break;
	case LD_C_H:
		registers->BC.c = registers->HL.h; break;
	case LD_C_L:
		registers->BC.c = registers->HL.l; break;
	case LD_C_HL:
		registers->BC.c = memory->ReadMem(registers->HL.hl); break;
	case LD_C_n:
		registers->BC.c = param1; break;
	case LD_D_D:
		break;
	case LD_D_A:
		registers->DE.d = registers->AF.a; break;
	case LD_D_B:
		registers->DE.d = registers->BC.b; break;
	case LD_D_C:
		registers->DE.d = registers->BC.c; break;
	case LD_D_E:
		registers->DE.d = registers->DE.e; break;
	case LD_D_H:
		registers->DE.d = registers->HL.h; break;
	case LD_D_L:
		registers->DE.d = registers->HL.l; break;
	case LD_D_HL:
		registers->DE.d = memory->ReadMem(registers->HL.hl); break;
	case LD_D_n:
		registers->DE.d = param1; break;
	case LD_E_E:
		break;
	case LD_E_A:
		registers->DE.e = registers->AF.a; break;
	case LD_E_B:
		registers->DE.e = registers->BC.b; break;
	case LD_E_C:
		registers->DE.e = registers->BC.c; break;
	case LD_E_D:
		registers->DE.e = registers->DE.d; break;
	case LD_E_H:
		registers->DE.e = registers->HL.h; break;
	case LD_E_L:
		registers->DE.e = registers->HL.l; break;
	case LD_E_HL:
		registers->DE.e = memory->ReadMem(registers->HL.hl); break;
	case LD_E_n:
		registers->DE.e = param1; break;
	case LD_H_H:
		break;
	case LD_H_A:
		registers->HL.h = registers->AF.a; break;
	case LD_H_B:
		registers->HL.h = registers->BC.b; break;
	case LD_H_C:
		registers->HL.h = registers->BC.c; break;
	case LD_H_D:
		registers->HL.h = registers->DE.d; break;
	case LD_H_E:
		registers->HL.h = registers->DE.e; break;
	case LD_H_L:
		registers->HL.h = registers->HL.l; break;
	case LD_H_HL:
		registers->HL.h = memory->ReadMem(registers->HL.hl); break;
	case LD_H_n:
		registers->HL.h = param1; break;
	case LD_L_L:
		break;
	case LD_L_A:
		registers->HL.l = registers->AF.a; break;
	case LD_L_B:
		registers->HL.l = registers->BC.b; break;
	case LD_L_C:
		registers->HL.l = registers->BC.c; break;
	case LD_L_D:
		registers->HL.l = registers->DE.d; break;
	case LD_L_E:
		registers->HL.l = registers->DE.e; break;
	case LD_L_H:
		registers->HL.l = registers->HL.h; break;
	case LD_L_HL:
		registers->HL.l = memory->ReadMem(registers->HL.hl); break;
	case LD_L_n:
		registers->HL.l = param1; break;
	case LD_HL_A:
		memory->WriteMem(registers->HL.hl, registers->AF.a); break;
	case LD_HL_B:
		memory->WriteMem(registers->HL.hl, registers->BC.b); break;
	case LD_HL_C:
		memory->WriteMem(registers->HL.hl, registers->BC.c); break;
	case LD_HL_D:
		memory->WriteMem(registers->HL.hl, registers->DE.d); break;
	case LD_HL_E:
		memory->WriteMem(registers->HL.hl, registers->DE.e); break;
	case LD_HL_H:
		memory->WriteMem(registers->HL.hl, registers->HL.h); break;
	case LD_HL_L:
		memory->WriteMem(registers->HL.hl, registers->HL.l); break;
	case LD__HL__n:
		memory->WriteMem(registers->HL.hl, param1); break;
	case LD_BC_nn:
		registers->BC.bc = address; break;
	case LD_DE_nn:
		registers->DE.de = address; break;
	case LD_HL_nn:
		registers->HL.hl = address; break;
	case LD_SP_nn:
		registers->SP = address; break;
	case LD_nn_SP:
		tempVal = registers->SP & 0xFF;
		memory->WriteMem(address, tempVal);
		tempVal = (registers->SP >> 8);
		memory->WriteMem(address+1, tempVal);
		break;
	case LD_BC_A:
		memory->WriteMem(registers->BC.bc, registers->AF.a); break;
	case LD_DE_A:
		memory->WriteMem(registers->DE.de, registers->AF.a); break;
	case LD_HLP_A:
		memory->WriteMem(registers->HL.hl, registers->AF.a); registers->HL.hl++; break;
	case LD_HLM_A:
		memory->WriteMem(registers->HL.hl, registers->AF.a); registers->HL.hl--; break;
	case LD_A_HLP:
		registers->AF.a = memory->ReadMem(registers->HL.hl); registers->HL.hl++; break;
	case LD_A_HLM:
		registers->AF.a = memory->ReadMem(registers->HL.hl); registers->HL.hl--; break;
	case LD_A_BC:
		registers->AF.a = memory->ReadMem(registers->BC.bc); break;
	case LD_A_DE:
		registers->AF.a = memory->ReadMem(registers->DE.de); break;
	case LD__C__A:
		memory->WriteMem(0xff00 + registers->BC.c, registers->AF.a); break;
	case LD_A__C_:
		registers->AF.a = memory->ReadMem(0xff00 + registers->BC.c); break;
	case LDH_n_A:
		memory->WriteMem(0xff00 + param1, registers->AF.a); break;
	case LDH_A_n:
		registers->AF.a = memory->ReadMem(0xff00 + param1); break;
	case LD_nn_A:
		memory->WriteMem(address, registers->AF.a); break;
	case LD_A_nn:
		registers->AF.a = memory->ReadMem(address); break;
	case LD_HL_SP_n:
		memory->resetFlag(Z);
		memory->resetFlag(N);
		t = registers->SP + (int8_t)param1;
		if ((((registers->SP & 0xf) + ((int8_t)param1 & 0xf)) & 0x10) == 0x10)
			memory->setFlag(H);
		else
			memory->resetFlag(H);

		if ((((registers->SP & 0xff) + ((int8_t)param1 & 0xff)) & 0x100) == 0x100)
			memory->setFlag(C);
		else
			memory->resetFlag(C);
		memory->resetFlag(Z);
		registers->HL.hl = registers->SP + (int8_t)param1;
		break;
	case LD_SP_HL:
		registers->SP = registers->HL.hl; break;
	}
}

#pragma endregion

#pragma region Increment / Decrement

void Commands::INC(uint8_t opcode) {
	uint8_t flagsAffected = 1;
	if (opcode == INC_BC || opcode == INC_DE || opcode == INC_HL || opcode == INC_SP) {
		flagsAffected = 0;
	}

	if (flagsAffected) {
		memory->resetFlag(Z);
		memory->resetFlag(N);
		memory->resetFlag(H);
	}
	uint8_t val = 0;

	switch (opcode) {
	case INC_A:
		val = registers->AF.a; registers->AF.a++; if (registers->AF.a == 0) { memory->setFlag(Z); } break;
	case INC_B:
		val = registers->BC.b; registers->BC.b++; if (registers->BC.b == 0) { memory->setFlag(Z); }  break;
	case INC_C:
		val = registers->BC.c; registers->BC.c++; if (registers->BC.c == 0) { memory->setFlag(Z); }  break;
	case INC_D:
		val = registers->DE.d; registers->DE.d++; if (registers->DE.d == 0) { memory->setFlag(Z); }  break;
	case INC_E:
		val = registers->DE.e; registers->DE.e++; if (registers->DE.e == 0) { memory->setFlag(Z); }  break;
	case INC_H:
		val = registers->HL.h; registers->HL.h++; if (registers->HL.h == 0) { memory->setFlag(Z); }  break;
	case INC_L:
		val = registers->HL.l; registers->HL.l++; if (registers->HL.l == 0) { memory->setFlag(Z); }  break;
	case INC__HL_:
		val = memory->ReadMem(registers->HL.hl); memory->WriteMem(registers->HL.hl, val + 1); if ((val + 1) == 0) { memory->setFlag(Z); }  break;
	case INC_BC:
		registers->BC.bc++; break;
	case INC_DE:
		registers->DE.de++; break;
	case INC_HL:
		registers->HL.hl++; break;
	case INC_SP:
		registers->SP++; break;
	}

	if (flagsAffected) {
		// Half carry - can only happen if previous value was bin_1111
		if ((((val & 0xf) + 0x1) & 0x10) == 0x10)
			memory->setFlag(H);
	}
}
void Commands::DEC(uint8_t opcode) {
	uint8_t flagsAffected = 1;
	if (opcode == DEC_BC || opcode == DEC_DE || opcode == DEC_HL || opcode == DEC_SP) {
		flagsAffected = 0;
	}

	if (flagsAffected) {
		memory->resetFlag(Z);
		memory->resetFlag(H);
		memory->setFlag(N);
	}
	uint8_t val = 0;


	switch (opcode) {
	case DEC_A:
		val = registers->AF.a; registers->AF.a--; if (registers->AF.a == 0) { memory->setFlag(Z); } break;
	case DEC_B:
		val = registers->BC.b; registers->BC.b--; if (registers->BC.b == 0) { memory->setFlag(Z); }  break;
	case DEC_C:
		val = registers->BC.c; registers->BC.c--; if (registers->BC.c == 0) { memory->setFlag(Z); }  break;
	case DEC_D:
		val = registers->DE.d; registers->DE.d--; if (registers->DE.d == 0) { memory->setFlag(Z); }  break;
	case DEC_E:
		val = registers->DE.e; registers->DE.e--; if (registers->DE.e == 0) { memory->setFlag(Z); }  break;
	case DEC_H:
		val = registers->HL.h; registers->HL.h--; if (registers->HL.h == 0) { memory->setFlag(Z); }  break;
	case DEC_L:
		val = registers->HL.l; registers->HL.l--; if (registers->HL.l == 0) { memory->setFlag(Z); }  break;
	case DEC__HL_:
		val = memory->ReadMem(registers->HL.hl); memory->WriteMem(registers->HL.hl, val - 1); if ((val - 1) == 0) { memory->setFlag(Z); }  break;
	case DEC_BC:
		registers->BC.bc--; break;
	case DEC_DE:
		registers->DE.de--; break;
	case DEC_HL:
		registers->HL.hl--; break;
	case DEC_SP:
		registers->SP--; break;
	}

	if (flagsAffected) {
		// Half carry, easy as can only happen when the previous value was bin_xxx10000
		if ((val & 0xf) == 0)  {
			memory->setFlag(H);
		}
		memory->setFlag(N);
	}
}

#pragma endregion

#pragma region Jump, Call and Returns

void Commands::JR(uint8_t opcode, uint8_t param1, uint8_t param2, uint8_t* skipPCInc) {
	uint16_t address = param1 + (param2 << 8);

	switch (opcode) {
		case JR_NC_n:
			if (!memory->getFlag(C)) { registers->PC += (int8_t)param1; }; break;
		case JR_NZ_n:
			if (!memory->getFlag(Z)) { registers->PC += (int8_t)param1; }; break;
		case JR_Z_n:
			if (memory->getFlag(Z)) { registers->PC += (int8_t)param1; }; break;
		case JR_C_n:
			if (memory->getFlag(C)) { registers->PC += (int8_t)param1; }; break;
		case JR_n:
			//printf("before: PC = %04x, param1=%02x\n", PC, param1);
			registers->PC += (int8_t)param1; break;
		case JP_NC_nn:
			if (!memory->getFlag(C)) { registers->PC = address; *skipPCInc = 1; }; break;
		case JP_NZ_nn:
			if (!memory->getFlag(Z)) { registers->PC = address; *skipPCInc = 1; }; break;
		case JP_C_nn:
			//printf("JP C: address=%04x\n", address);
			if (memory->getFlag(C)) { registers->PC = address; *skipPCInc = 1; }; break;
		case JP_Z_nn:
			if (memory->getFlag(Z)) { registers->PC = address; *skipPCInc = 1; }; break;
		case JP_nn:
			//printf("param1=%02x, param2=%02x, address=%04x\n", param1, param2, address);
			registers->PC = address; *skipPCInc = 1; break;
		case JP_HL:
			registers->PC = registers->HL.hl; *skipPCInc = 1; break;
	}
}
void Commands::CALL(uint8_t opcode, uint8_t param1, uint8_t param2) {
	uint16_t address = (param2 << 8) + param1;
	uint8_t doCall = 0;
	short i = 0;
	//printf("\nparam1=%02x, param2=%02x, address=%04x\n", param1, param2, address);

	switch (opcode) {
	case CALL_NC_nn:
		if (!memory->getFlag(C)) { doCall = 1; }; break;
	case CALL_NZ_nn:
		if (!memory->getFlag(Z)) { doCall = 1; }; break;
	case CALL_Z_nn:
		if (memory->getFlag(Z)) { doCall = 1; }; break;
	case CALL_C_nn:
		if (memory->getFlag(C)) { doCall = 1; }; break;
	case CALL_nn:
		doCall = 1; break;
	}

	uint16_t next = registers->PC + 3;
	if (doCall) {
		memory->WriteMem(--registers->SP, (uint8_t)(next >> 8));
		memory->WriteMem(--registers->SP, (uint8_t)(next & 0xff));
		registers->PC = address;
	}
	else {
		registers->PC = next;
	}
}
void Commands::RET_(uint8_t opcode, uint8_t* skipPCInc) {
	uint16_t address;
	bool doReturn = false;

	address = (memory->ReadMem(registers->SP + 1) << 8) + memory->ReadMem(registers->SP);

	switch (opcode) {
		case RET_NC:
			if (!memory->getFlag(C)) { doReturn = true; }; break;
		case RET_NZ:
			if (!memory->getFlag(Z)) { doReturn = true; }; break;
		case RET_Z:
			if (memory->getFlag(Z)) { doReturn = true; }; break;
		case RET_C:
			if (memory->getFlag(C)) { doReturn = true; }; break;
		case RET:
		case RETI: // In the case of RETI, the interrupt flag should be set in the CPU calling code
			doReturn = true; break;
	}
	if (doReturn) {
		registers->PC = address;
		registers->SP += 2; 
		*skipPCInc = 1;
	}
}

#pragma endregion

#pragma region Bit Operations

void Commands::SWAP(uint8_t opcode) {
	uint8_t tmp = 0, val = 0;
	memory->resetFlag(Z);
	memory->resetFlag(N);
	memory->resetFlag(H);
	memory->resetFlag(C);

	switch (opcode) {
	case SWAP_A:
		tmp = registers->AF.a << 4; registers->AF.a = (registers->AF.a >> 4) | tmp; tmp = registers->AF.a; break;
	case SWAP_B:
		tmp = registers->BC.b << 4; registers->BC.b = (registers->BC.b >> 4) | tmp; tmp = registers->BC.b; break;
	case SWAP_C:
		tmp = registers->BC.c << 4; registers->BC.c = (registers->BC.c >> 4) | tmp; tmp = registers->BC.c; break;
	case SWAP_D:
		tmp = registers->DE.d << 4; registers->DE.d = (registers->DE.d >> 4) | tmp; tmp = registers->DE.d; break;
	case SWAP_E:
		tmp = registers->DE.e << 4; registers->DE.e = (registers->DE.e >> 4) | tmp; tmp = registers->DE.e; break;
	case SWAP_H:
		tmp = registers->HL.h << 4; registers->HL.h = (registers->HL.h >> 4) | tmp; tmp = registers->HL.h; break;
	case SWAP_L:
		tmp = registers->HL.l << 4; registers->HL.l = (registers->HL.l >> 4) | tmp; tmp = registers->HL.l; break;
	case SWAP_HL:
		val = memory->ReadMem(registers->HL.hl);
		tmp = val << 4; val = (val >> 4) | tmp; tmp = val;
		memory->WriteMem(registers->HL.hl, val); break;
	}

	if (tmp == 0) { memory->setFlag(Z); }

}
void Commands::BIT(uint8_t opcode, int bit_n) {
	uint8_t set = 0;
	uint8_t bit = 0;

	bit = 1 << bit_n;

	switch (opcode) {
	case BIT0_A:
	case BIT1_A:
	case BIT2_A:
	case BIT3_A:
	case BIT4_A:
	case BIT5_A:
	case BIT6_A:
	case BIT7_A:
		bit = registers->AF.a & bit; break;
	case BIT0_B:
	case BIT1_B:
	case BIT2_B:
	case BIT3_B:
	case BIT4_B:
	case BIT5_B:
	case BIT6_B:
	case BIT7_B:
		bit = registers->BC.b & bit; break;
	case BIT0_C:
	case BIT1_C:
	case BIT2_C:
	case BIT3_C:
	case BIT4_C:
	case BIT5_C:
	case BIT6_C:
	case BIT7_C:
		bit = registers->BC.c & bit; break;
	case BIT0_D:
	case BIT1_D:
	case BIT2_D:
	case BIT3_D:
	case BIT4_D:
	case BIT5_D:
	case BIT6_D:
	case BIT7_D:
		bit = registers->DE.d & bit; break;
	case BIT0_E:
	case BIT1_E:
	case BIT2_E:
	case BIT3_E:
	case BIT4_E:
	case BIT5_E:
	case BIT6_E:
	case BIT7_E:
		bit = registers->DE.e & bit; break;
	case BIT0_H:
	case BIT1_H:
	case BIT2_H:
	case BIT3_H:
	case BIT4_H:
	case BIT5_H:
	case BIT6_H:
	case BIT7_H:
		bit = registers->HL.h & bit; break;
	case BIT0_L:
	case BIT1_L:
	case BIT2_L:
	case BIT3_L:
	case BIT4_L:
	case BIT5_L:
	case BIT6_L:
	case BIT7_L:
		bit = registers->HL.l & bit; break;
	case BIT0_HL:
	case BIT1_HL:
	case BIT2_HL:
	case BIT3_HL:
	case BIT4_HL:
	case BIT5_HL:
	case BIT6_HL:
	case BIT7_HL:
		bit = memory->ReadMem(registers->HL.hl) & bit; break;

	}

	if (bit) {
		memory->resetFlag(Z);
	}
	else {
		memory->setFlag(Z);
	}
	memory->resetFlag(N);
	memory->setFlag(H);
}
void Commands::SET(uint8_t opcode, uint8_t bit_n) {
	uint8_t bit = 0;

	bit = 1 << bit_n;

	switch (opcode) {
	case SET0_A:
	case SET1_A:
	case SET2_A:
	case SET3_A:
	case SET4_A:
	case SET5_A:
	case SET6_A:
	case SET7_A:
		registers->AF.a |= bit; break;
	case SET0_B:
	case SET1_B:
	case SET2_B:
	case SET3_B:
	case SET4_B:
	case SET5_B:
	case SET6_B:
	case SET7_B:
		registers->BC.b |= bit; break;
	case SET0_C:
	case SET1_C:
	case SET2_C:
	case SET3_C:
	case SET4_C:
	case SET5_C:
	case SET6_C:
	case SET7_C:
		registers->BC.c |= bit; break;
	case SET0_D:
	case SET1_D:
	case SET2_D:
	case SET3_D:
	case SET4_D:
	case SET5_D:
	case SET6_D:
	case SET7_D:
		registers->DE.d |= bit; break;
	case SET0_E:
	case SET1_E:
	case SET2_E:
	case SET3_E:
	case SET4_E:
	case SET5_E:
	case SET6_E:
	case SET7_E:
		registers->DE.e |= bit; break;
	case SET0_H:
	case SET1_H:
	case SET2_H:
	case SET3_H:
	case SET4_H:
	case SET5_H:
	case SET6_H:
	case SET7_H:
		registers->HL.h |= bit; break;
	case SET0_L:
	case SET1_L:
	case SET2_L:
	case SET3_L:
	case SET4_L:
	case SET5_L:
	case SET6_L:
	case SET7_L:
		registers->HL.l |= bit; break;
	case SET0_HL:
	case SET1_HL:
	case SET2_HL:
	case SET3_HL:
	case SET4_HL:
	case SET5_HL:
	case SET6_HL:
	case SET7_HL:
		memory->WriteMem(registers->HL.hl, memory->ReadMem(registers->HL.hl) | bit); break;
	}
}
void Commands::RES(uint8_t opcode, uint8_t bit_n) {
	uint8_t bit = 0;

	bit = (1 << bit_n) ^ 0xff;

	switch (opcode) {
	case RES0_A:
	case RES1_A:
	case RES2_A:
	case RES3_A:
	case RES4_A:
	case RES5_A:
	case RES6_A:
	case RES7_A:
		registers->AF.a &= bit; break;
	case RES0_B:
	case RES1_B:
	case RES2_B:
	case RES3_B:
	case RES4_B:
	case RES5_B:
	case RES6_B:
	case RES7_B:
		registers->BC.b &= bit; break;
	case RES0_C:
	case RES1_C:
	case RES2_C:
	case RES3_C:
	case RES4_C:
	case RES5_C:
	case RES6_C:
	case RES7_C:
		registers->BC.c &= bit; break;
	case RES0_D:
	case RES1_D:
	case RES2_D:
	case RES3_D:
	case RES4_D:
	case RES5_D:
	case RES6_D:
	case RES7_D:
		registers->DE.d &= bit; break;
	case RES0_E:
	case RES1_E:
	case RES2_E:
	case RES3_E:
	case RES4_E:
	case RES5_E:
	case RES6_E:
	case RES7_E:
		registers->DE.e &= bit; break;
	case RES0_H:
	case RES1_H:
	case RES2_H:
	case RES3_H:
	case RES4_H:
	case RES5_H:
	case RES6_H:
	case RES7_H:
		registers->HL.h &= bit; break;
	case RES0_L:
	case RES1_L:
	case RES2_L:
	case RES3_L:
	case RES4_L:
	case RES5_L:
	case RES6_L:
	case RES7_L:
		registers->HL.l &= bit; break;
	case RES0_HL:
	case RES1_HL:
	case RES2_HL:
	case RES3_HL:
	case RES4_HL:
	case RES5_HL:
	case RES6_HL:
	case RES7_HL:
		memory->WriteMem(registers->HL.hl, memory->ReadMem(registers->HL.hl) & bit); break;
	}
}

#pragma endregion

#pragma region Shift Operations

void Commands::RLC(uint8_t opcode) {
	memory->resetFlag(Z);
	memory->resetFlag(N);
	memory->resetFlag(H);
	memory->resetFlag(C);
	uint8_t outbit = 1 << 7;
	uint8_t val = 0;

	switch (opcode) {
	case RLC_A:
		outbit &= registers->AF.a; registers->AF.a <<= 1; outbit >>= 7; registers->AF.a += outbit; val = registers->AF.a; break;
	case RLC_B:
		outbit &= registers->BC.b; registers->BC.b <<= 1; outbit >>= 7; registers->BC.b += outbit; val = registers->BC.b; break;
	case RLC_C:
		outbit &= registers->BC.c; registers->BC.c <<= 1; outbit >>= 7; registers->BC.c += outbit; val = registers->BC.c; break;
	case RLC_D:
		outbit &= registers->DE.d; registers->DE.d <<= 1; outbit >>= 7; registers->DE.d += outbit; val = registers->DE.d; break;
	case RLC_E:
		outbit &= registers->DE.e; registers->DE.e <<= 1; outbit >>= 7; registers->DE.e += outbit; val = registers->DE.e; break;
	case RLC_H:
		outbit &= registers->HL.h; registers->HL.h <<= 1; outbit >>= 7; registers->HL.h += outbit; val = registers->HL.h; break;
	case RLC_L:
		outbit &= registers->HL.l; registers->HL.l <<= 1; outbit >>= 7; registers->HL.l += outbit; val = registers->HL.l; break;
	case RLC_HL:
		val = memory->ReadMem(registers->HL.hl);
		outbit &= val; val <<= 1; outbit >>= 7; val += outbit;
		memory->WriteMem(registers->HL.hl, val); break;
	}

	if (outbit) {
		memory->setFlag(C);
	}
	if (val == 0) {
		memory->setFlag(Z);
	}
}
void Commands::RL(uint8_t opcode) {
	uint8_t oldC = memory->getFlag(C);
	memory->resetFlag(Z);
	memory->resetFlag(N);
	memory->resetFlag(H);
	memory->resetFlag(C);
	uint8_t outbit = 1 << 7;
	uint8_t val = 0;

	switch (opcode) {
	case RL_A:
		outbit &= registers->AF.a; registers->AF.a <<= 1; registers->AF.a |= oldC; val = registers->AF.a; break;
	case RL_B:
		outbit &= registers->BC.b; registers->BC.b <<= 1; registers->BC.b |= oldC; val = registers->BC.b; break;
	case RL_C:
		outbit &= registers->BC.c; registers->BC.c <<= 1; registers->BC.c |= oldC; val = registers->BC.c; break;
	case RL_D:
		outbit &= registers->DE.d; registers->DE.d <<= 1; registers->DE.d |= oldC; val = registers->DE.d; break;
	case RL_E:
		outbit &= registers->DE.e; registers->DE.e <<= 1; registers->DE.e |= oldC; val = registers->DE.e; break;
	case RL_H:
		outbit &= registers->HL.h; registers->HL.h <<= 1; registers->HL.h |= oldC; val = registers->HL.h; break;
	case RL_L:
		outbit &= registers->HL.l; registers->HL.l <<= 1; registers->HL.l |= oldC; val = registers->HL.l; break;
	case RL_HL:
		val = memory->ReadMem(registers->HL.hl);
		outbit &= val; val <<= 1; val |= oldC;
		memory->WriteMem(registers->HL.hl, val); break;
	}

	if (outbit) {
		memory->setFlag(C);
	}
	if (val == 0) {
		memory->setFlag(Z);
	}
}
void Commands::RRC(uint8_t opcode) {
	memory->resetFlag(Z);
	memory->resetFlag(N);
	memory->resetFlag(H);
	memory->resetFlag(C);
	uint8_t outbit = 1;
	uint8_t val = 0;

	switch (opcode) {
	case RRC_A:
		outbit &= registers->AF.a; registers->AF.a >>= 1; outbit <<= 7; registers->AF.a += outbit; val = registers->AF.a; break;
	case RRC_B:
		outbit &= registers->BC.b; registers->BC.b >>= 1; outbit <<= 7; registers->BC.b += outbit; val = registers->BC.b; break;
	case RRC_C:
		outbit &= registers->BC.c; registers->BC.c >>= 1; outbit <<= 7; registers->BC.c += outbit; val = registers->BC.c; break;
	case RRC_D:
		outbit &= registers->DE.d; registers->DE.d >>= 1; outbit <<= 7; registers->DE.d += outbit; val = registers->DE.d; break;
	case RRC_E:
		outbit &= registers->DE.e; registers->DE.e >>= 1; outbit <<= 7; registers->DE.e += outbit; val = registers->DE.e; break;
	case RRC_H:
		outbit &= registers->HL.h; registers->HL.h >>= 1; outbit <<= 7; registers->HL.h += outbit; val = registers->HL.h; break;
	case RRC_L:
		outbit &= registers->HL.l; registers->HL.l >>= 1; outbit <<= 7; registers->HL.l += outbit; val = registers->HL.l; break;
	case RRC_HL:
		val = memory->ReadMem(registers->HL.hl);
		outbit &= val; val >>= 1; outbit <<= 7; val += outbit;
		memory->WriteMem(registers->HL.hl, val); break;
	}

	if (outbit) {
		memory->setFlag(C);
	}
	if (val == 0) {
		memory->setFlag(Z);
	}
}
void Commands::RR(uint8_t opcode) {
	uint8_t oldC = memory->getFlag(C) << 7;
	memory->resetFlag(Z);
	memory->resetFlag(N);
	memory->resetFlag(H);
	memory->resetFlag(C);
	uint8_t outbit = 1;
	uint8_t val = 0;

	switch (opcode) {
	case RR_A:
		outbit &= registers->AF.a; registers->AF.a >>= 1; registers->AF.a |= oldC; val = registers->AF.a; break;
	case RR_B:
		outbit &= registers->BC.b; registers->BC.b >>= 1; registers->BC.b |= oldC; val = registers->BC.b; break;
	case RR_C:
		outbit &= registers->BC.c; registers->BC.c >>= 1; registers->BC.c |= oldC; val = registers->BC.c; break;
	case RR_D:
		outbit &= registers->DE.d; registers->DE.d >>= 1; registers->DE.d |= oldC; val = registers->DE.d; break;
	case RR_E:
		outbit &= registers->DE.e; registers->DE.e >>= 1; registers->DE.e |= oldC; val = registers->DE.e; break;
	case RR_H:
		outbit &= registers->HL.h; registers->HL.h >>= 1; registers->HL.h |= oldC; val = registers->HL.h; break;
	case RR_L:
		outbit &= registers->HL.l; registers->HL.l >>= 1; registers->HL.l |= oldC; val = registers->HL.l; break;
	case RR_HL:
		val = memory->ReadMem(registers->HL.hl);
		outbit &= val; val >>= 1; val |= oldC;
		memory->WriteMem(registers->HL.hl, val); break;
	}

	if (outbit) {
		memory->setFlag(C);
	}
	if (val == 0) {
		memory->setFlag(Z);
	}
}
void Commands::SRL(uint8_t opcode) {
	memory->resetFlag(N);
	memory->resetFlag(H);
	memory->resetFlag(C);
	memory->resetFlag(Z);
	uint8_t val = 0;

	switch (opcode) {
	case SRL_A:
		if (registers->AF.a & 1) { memory->setFlag(C); }; registers->AF.a >>= 1; val = registers->AF.a; break;
	case SRL_B:
		if (registers->BC.b & 1) { memory->setFlag(C); }; registers->BC.b >>= 1; val = registers->BC.b; break;
	case SRL_C:
		if (registers->BC.c & 1) { memory->setFlag(C); }; registers->BC.c >>= 1; val = registers->BC.c; break;
	case SRL_D:
		if (registers->DE.d & 1) { memory->setFlag(C); }; registers->DE.d >>= 1; val = registers->DE.d; break;
	case SRL_E:
		if (registers->DE.e & 1) { memory->setFlag(C); }; registers->DE.e >>= 1; val = registers->DE.e; break;
	case SRL_H:
		if (registers->HL.h & 1) { memory->setFlag(C); }; registers->HL.h >>= 1; val = registers->HL.h; break;
	case SRL_L:
		if (registers->HL.l & 1) { memory->setFlag(C); }; registers->HL.l >>= 1; val = registers->HL.l; break;
	case SRL_HL:
		val = memory->ReadMem(registers->HL.hl); if (val & 1) { memory->setFlag(C); }; val >>= 1; memory->WriteMem(registers->HL.hl, val); break;
	}

	if (val == 0) {
		memory->setFlag(Z);
	}

}
void Commands::SRA(uint8_t opcode) {
	memory->resetFlag(N);
	memory->resetFlag(H);
	memory->resetFlag(C);
	memory->resetFlag(Z);
	uint8_t val = 0;
	uint8_t bit7 = 0;

	switch (opcode) {
	case SRA_A:
		if (registers->AF.a & 1) { memory->setFlag(C); }; bit7 = (registers->AF.a & 0x80); registers->AF.a >>= 1; registers->AF.a ^= bit7; val = registers->AF.a; break;
	case SRA_B:
		if (registers->BC.b & 1) { memory->setFlag(C); }; bit7 = (registers->BC.b & 0x80); registers->BC.b >>= 1; registers->BC.b ^= bit7; val = registers->BC.b; break;
	case SRA_C:
		if (registers->BC.c & 1) { memory->setFlag(C); }; bit7 = (registers->BC.c & 0x80); registers->BC.c >>= 1; registers->BC.c ^= bit7; val = registers->BC.c; break;
	case SRA_D:
		if (registers->DE.d & 1) { memory->setFlag(C); }; bit7 = (registers->DE.d & 0x80); registers->DE.d >>= 1; registers->DE.d ^= bit7; val = registers->DE.d; break;
	case SRA_E:
		if (registers->DE.e & 1) { memory->setFlag(C); }; bit7 = (registers->DE.e & 0x80); registers->DE.e >>= 1; registers->DE.e ^= bit7; val = registers->DE.e; break;
	case SRA_H:
		if (registers->HL.h & 1) { memory->setFlag(C); }; bit7 = (registers->HL.h & 0x80); registers->HL.h >>= 1; registers->HL.h ^= bit7; val = registers->HL.h; break;
	case SRA_L:
		if (registers->HL.l & 1) { memory->setFlag(C); }; bit7 = (registers->HL.l & 0x80);  registers->HL.l >>= 1; registers->HL.l ^= bit7; val = registers->HL.l; break;
	case SRA_HL:
		val = memory->ReadMem(registers->HL.hl); if (val & 1) { memory->setFlag(C); }; bit7 = (val & 0x80); val >>= 1; val ^= bit7; memory->WriteMem(registers->HL.hl, val); break;
	default:
		return;
	}

	if (val == 0) {
		memory->setFlag(Z);
	}
}
void Commands::SLA(uint8_t opcode) {
	memory->resetFlag(N);
	memory->resetFlag(H);
	memory->resetFlag(C);
	memory->resetFlag(Z);
	uint8_t val = 0;

	switch (opcode) {
	case SLA_A:
		if (registers->AF.a & 0x80) { memory->setFlag(C); }; registers->AF.a <<= 1; val = registers->AF.a; break;
	case SLA_B:
		if (registers->BC.b & 0x80) { memory->setFlag(C); }; registers->BC.b <<= 1; val = registers->BC.b; break;
	case SLA_C:
		if (registers->BC.c & 0x80) { memory->setFlag(C); }; registers->BC.c <<= 1; val = registers->BC.c; break;
	case SLA_D:
		if (registers->DE.d & 0x80) { memory->setFlag(C); }; registers->DE.d <<= 1; val = registers->DE.d; break;
	case SLA_E:
		if (registers->DE.e & 0x80) { memory->setFlag(C); }; registers->DE.e <<= 1; val = registers->DE.e; break;
	case SLA_H:
		if (registers->HL.h & 0x80) { memory->setFlag(C); }; registers->HL.h <<= 1; val = registers->HL.h; break;
	case SLA_L:
		if (registers->HL.l & 0x80) { memory->setFlag(C); }; registers->HL.l <<= 1; val = registers->HL.l; break;
	case SLA_HL:
		val = memory->ReadMem(registers->HL.hl); if (val & 0x80) { memory->setFlag(C); }; val <<= 1; memory->WriteMem(registers->HL.hl, val); break;
	}

	if (val == 0) {
		memory->setFlag(Z);
	}
}
void Commands::RRCA_() {
	memory->resetFlag(Z);
	memory->resetFlag(H);
	memory->resetFlag(N);
	memory->resetFlag(C);

	uint8_t c = registers->AF.a & 1;
	registers->AF.a >>= 1;
	registers->AF.a += c << 7;
	if (c) {
		memory->setFlag(C);
	}
}
void Commands::RRA() {
	uint8_t oldC = memory->getFlag(C);
	memory->resetFlag(Z);
	memory->resetFlag(H);
	memory->resetFlag(N);
	memory->resetFlag(C);

	uint8_t c = registers->AF.a & 1;
	registers->AF.a >>= 1;
	registers->AF.a += oldC << 7;
	if (c) {
		memory->setFlag(C);
	}
	if (registers->AF.a == 0) {
		memory->setFlag(Z);
	}
}
void Commands::RLCA_() {
	memory->resetFlag(Z);
	memory->resetFlag(H);
	memory->resetFlag(N);
	memory->resetFlag(C);

	uint8_t c = registers->AF.a & 0x80;
	registers->AF.a <<= 1;
	registers->AF.a += c >> 7;
	if (c) {
		memory->setFlag(C);
	}
}
void Commands::RLA_() {
	uint8_t oldC = memory->getFlag(C);
	memory->resetFlag(Z);
	memory->resetFlag(H);
	memory->resetFlag(N);
	memory->resetFlag(C);

	uint8_t c = registers->AF.a & 0x80;
	registers->AF.a <<= 1;
	registers->AF.a += oldC;
	if (c) {
		memory->setFlag(C);
	}
	if (registers->AF.a == 0) {
		memory->setFlag(Z);
	}
}

#pragma endregion

#pragma region Logical Commands

void Commands::XOR(uint8_t opcode) {
	return XOR(opcode, 0);
}

void Commands::XOR(uint8_t opcode, uint8_t param) {
	memory->resetFlag(Z);
	memory->resetFlag(N);
	memory->resetFlag(H);
	memory->resetFlag(C);

	switch (opcode) {
	case XOR_A:
		registers->AF.a ^= registers->AF.a; break;
	case XOR_B:
		registers->AF.a ^= registers->BC.b; break;
	case XOR_C:
		registers->AF.a ^= registers->BC.c; break;
	case XOR_D:
		registers->AF.a ^= registers->DE.d; break;
	case XOR_E:
		registers->AF.a ^= registers->DE.e; break;
	case XOR_H:
		registers->AF.a ^= registers->HL.h; break;
	case XOR_L:
		registers->AF.a ^= registers->HL.l; break;
	case XOR_HL:
		registers->AF.a ^= memory->ReadMem(registers->HL.hl); break;
	case XOR_n:
		registers->AF.a ^= param; break;
	}

	if (registers->AF.a == 0) {
		memory->setFlag(Z);
	}
}

void Commands::AND(uint8_t opcode) {
	return AND(opcode, 0);
}
void Commands::AND(uint8_t opcode, uint8_t param) {
	switch (opcode) {
		case AND_A:
			registers->AF.a = registers->AF.a; break;
		case AND_B:
			registers->AF.a &= registers->BC.b; break;
		case AND_C:
			registers->AF.a &= registers->BC.c; break;
		case AND_D:
			registers->AF.a &= registers->DE.d; break;
		case AND_E:
			registers->AF.a &= registers->DE.e; break;
		case AND_H:
			registers->AF.a &= registers->HL.h; break;
		case AND_L:
			registers->AF.a &= registers->HL.l; break;
		case AND_HL:
			registers->AF.a &= memory->ReadMem(registers->HL.hl); break;
		case AND_n:
			registers->AF.a &= param; break;
	}

	memory->setFlag(H);
	memory->resetFlag(C);
	memory->resetFlag(N);

	if (registers->AF.a == 0) {
		memory->setFlag(Z);
	}
	else {
		memory->resetFlag(Z);
	}
}

void Commands::OR(uint8_t opcode) {
	return OR(opcode, 0);
}
void Commands::OR(uint8_t opcode, uint8_t param) {
	memory->resetFlag(Z);
	memory->resetFlag(N);
	memory->resetFlag(H);
	memory->resetFlag(C);

	switch (opcode) {
		case OR_A:
			break;
		case OR_B:
			registers->AF.a |= registers->BC.b; break;
		case OR_C:
			registers->AF.a |= registers->BC.c; break;
		case OR_D:
			registers->AF.a |= registers->DE.d; break;
		case OR_E:
			registers->AF.a |= registers->DE.e; break;
		case OR_H:
			registers->AF.a |= registers->HL.h; break;
		case OR_L:
			registers->AF.a |= registers->HL.l; break;
		case OR_HL:
			registers->AF.a |= memory->ReadMem(registers->HL.hl); break;
		case OR_n:
			registers->AF.a |= param; break;
	}

	if (registers->AF.a == 0) { memory->setFlag(Z); }
}

#pragma endregion


void Commands::CP(uint8_t opcode) {
	return CP(opcode, 0);
}
void Commands::CP(uint8_t opcode, uint8_t param) {
	memory->resetFlag(Z);
	memory->resetFlag(C);
	memory->setFlag(N);
	memory->resetFlag(H);
	uint8_t val = 0, res = 0;
	uint8_t oldA = registers->AF.a;

	switch (opcode) {
		case CP_A:
			val = registers->AF.a; res = registers->AF.a - val; break;
		case CP_B:
			res = registers->AF.a - registers->BC.b; val = registers->BC.b; break;
		case CP_C:
			res = registers->AF.a - registers->BC.c; val = registers->BC.c; break;
		case CP_D:
			res = registers->AF.a - registers->DE.d; val = registers->DE.d; break;
		case CP_E:
			res = registers->AF.a - registers->DE.e; val = registers->DE.e; break;
		case CP_H:
			res = registers->AF.a - registers->HL.h; val = registers->HL.h; break;
		case CP_L:
			res = registers->AF.a - registers->HL.l; val = registers->HL.l; break;
		case CP_HL:
			val = memory->ReadMem(registers->HL.hl); res = registers->AF.a - val; break;
		case CP_n:
			res = registers->AF.a - param; val = param; break;
	}

	if (res == 0) { memory->setFlag(Z); }

	// Half carry and full carry
	if ((val & 0xf) > (oldA & 0xf)) {
		memory->setFlag(H);
	}
	if (val > oldA) {
		memory->setFlag(C);
	}
}
void Commands::RST(uint8_t opcode) {
	// Push the PC onto the stack
	uint16_t next = registers->PC + 1;
	memory->WriteMem(--registers->SP, (uint8_t)(next >> 8));
	memory->WriteMem(--registers->SP, (uint8_t)(next & 0xff));

	switch (opcode) {
		case RST_00H:
			registers->PC = 0; break;
		case RST_10H:
			registers->PC = 0x10; break;
		case RST_20H:
			registers->PC = 0x20; break;
		case RST_30H:
			registers->PC = 0x30; break;
		case RST_08H:
			registers->PC = 0x08; break;
		case RST_18H:
			registers->PC = 0x18; break;
		case RST_28H:
			registers->PC = 0x28; break;
		case RST_38H:
			registers->PC = 0x38; break;
	}
}

void Commands::PUSH(uint8_t opcode) {
	switch (opcode) {
		case PUSH_AF:
			memory->WriteMem(--registers->SP, registers->AF.a); memory->WriteMem(--registers->SP, registers->AF.f); break;
		case PUSH_DE:
			memory->WriteMem(--registers->SP, registers->DE.d); memory->WriteMem(--registers->SP, registers->DE.e); break;
		case PUSH_BC:
			memory->WriteMem(--registers->SP, registers->BC.b); memory->WriteMem(--registers->SP, registers->BC.c); break;
		case PUSH_HL:
			memory->WriteMem(--registers->SP, registers->HL.h); memory->WriteMem(--registers->SP, registers->HL.l); break;
	}
}
void Commands::POP(uint8_t opcode) {
	switch (opcode) {
		case POP_AF: // Need to clear the lower 4 bits of f register, due to some vagary of the hardware. Not documented in manual!
			registers->AF.f = memory->ReadMem(registers->SP++) & 0xf0; registers->AF.a = memory->ReadMem(registers->SP++); break;
		case POP_DE:
			registers->DE.e = memory->ReadMem(registers->SP++); registers->DE.d = memory->ReadMem(registers->SP++); break;
		case POP_BC:
			registers->BC.c = memory->ReadMem(registers->SP++); registers->BC.b = memory->ReadMem(registers->SP++); break;
		case POP_HL:
			registers->HL.l = memory->ReadMem(registers->SP++); registers->HL.h = memory->ReadMem(registers->SP++); break;
	}
}

void Commands::DAA_() {
	uint8_t hFlag = memory->getFlag(H);
	uint8_t cFlag = memory->getFlag(C);
	uint8_t nFlag = memory->getFlag(N);

	memory->resetFlag(C);
	memory->resetFlag(H);

	//uint32_t temp = registers->AF.a;

	//if (!nFlag) {  // after an addition, adjust if (half-)carry occurred or if result is out of bounds
	//	if (hFlag || (registers->AF.a & 0xf) > 0x09) { registers->AF.a += 0x6; temp = registers->AF.a + 0x6; }
	//	if (cFlag || (registers->AF.a > 0x9f)) {
	//		registers->AF.a += 0x60; 
	//		temp = registers->AF.a + 0x60;
	//	}
	//}
	//else {  // after a subtraction, only adjust if (half-)carry occurredq  
	//	if (hFlag) { registers->AF.a -= 0x6; }
	//	if (cFlag) { registers->AF.a -= 0x60; }
	//}

	//if (registers->AF.a == 0) {
	//	memory->setFlag(Z);
	//}
	//if (temp > 0xff) {
 	//	memory->setFlag(C);
	//}

	uint8_t correction = 0;
	if (hFlag || (!nFlag && (registers->AF.a & 0xf) > 9)) {
		correction |= 0x6;
	}
	if (cFlag || (!nFlag && registers->AF.a > 0x99)) {
		correction |= 0x60;
		memory->setFlag(C);
	}
	registers->AF.a += nFlag ? -correction : correction;

	if (registers->AF.a == 0) {
		memory->setFlag(Z);
	}


	//let correction = 0;

	//let setFlagC = 0;
	//if (flagH || (!flagN && (value & 0xf) > 9)) {
	//	correction |= 0x6;
	//}

	//if (flagC || (!flagN && value > 0x99)) {
	//	correction |= 0x60;
	//	setFlagC = FLAG_C;
	//}

	//value += flagN ? -correction : correction;

	//value &= 0xff;

	//const setFlagZ = value == = 0 ? FLAG_Z : 0;

	//regF &= ~(FLAG_H | FLAG_Z | FLAG_C);
	//regF |= setFlagC | setFlagZ;

	//return { output, carry, zero };

}

const string Commands::CodeToString(uint8_t opcode, uint16_t PC, uint16_t param1, uint16_t param2) {
	stringstream ss;
	
	ss << hex << setw(4) << setfill('0') << PC;

	switch (opcode) {
		case CB:
			 ss << CBCodeToString(param1); break;
		case NOP:
			ss << " NOP "; break;
		case DAA:
			ss << " DAA"; break;
		case LD_A_A:
			ss << " LD A, A"; break;
		case LD_B_B:
			ss << " LD B, B"; break;
		case LD_C_C:
			ss << " LD C, C"; break;
		case LD_D_D:
			ss << " LD D, D"; break;
		case LD_E_E:
			ss << " LD E, E"; break;
		case LD_H_H:
			ss << " LD H, H"; break;
		case LD_L_L:
			ss << " LD L, L"; break;
		case LD_A_B:
			ss << " LD A, B"; break;
		case LD_A_C:
			ss << " LD A, C"; break;
		case LD_A_D:
			ss << " LD A, D"; break;
		case LD_A_E:
			ss << " LD A, E"; break;
		case LD_A_H:
			ss << " LD A, H"; break;
		case LD_A_L:
			ss << " LD A, L"; break;
		case LD_A_HL:
			ss << " LD A, (HL)"; break;
		case LD_B_A:
			ss << " LD B, A"; break;
		case LD_B_C:
			ss << " LD B, C"; break;
		case LD_B_D:
			ss << " LD B, D"; break;
		case LD_B_E:
			ss << " LD B, E"; break;
		case LD_B_H:
			ss << " LD B, H"; break;
		case LD_B_L:
			ss << " LD B, L"; break;
		case LD_B_HL:
			ss << " LD B, (HL)"; break;
		case LD_C_A:
			ss << " LD C, A"; break;
		case LD_C_B:
			ss << " LD C, B"; break;
		case LD_C_D:
			ss << " LD C, D"; break;
		case LD_C_E:
			ss << " LD C, E"; break;
		case LD_C_H:
			ss << " LD C, H"; break;
		case LD_C_L:
			ss << " LD C, L"; break;
		case LD_C_HL:
			ss << " LD C, (HL)"; break;
		case LD_D_A:
			ss << " LD D, A"; break;
		case LD_D_B:
			ss << " LD D, B"; break;
		case LD_D_C:
			ss << " LD D, C"; break;
		case LD_D_E:
			ss << " LD D, E"; break;
		case LD_D_H:
			ss << " LD D, H"; break;
		case LD_D_L:
			ss << " LD D, L"; break;
		case LD_D_HL:
			ss << " LD D, (HL)"; break;
		case LD_E_A:
			ss << " LD E, A"; break;
		case LD_E_B:
			ss << " LD E, B"; break;
		case LD_E_C:
			ss << " LD E, C"; break;
		case LD_E_D:
			ss << " LD E, D"; break;
		case LD_E_H:
			ss << " LD E, H"; break;
		case LD_E_L:
			ss << " LD E, L"; break;
		case LD_E_HL:
			ss << " LD E, (HL)"; break;
		case LD_H_A:
			ss << " LD H, A"; break;
		case LD_H_B:
			ss << " LD H, B"; break;
		case LD_H_C:
			ss << " LD H, C"; break;
		case LD_H_D:
			ss << " LD H, D"; break;
		case LD_H_E:
			ss << " LD H, E"; break;
		case LD_H_L:
			ss << " LD H, L"; break;
		case LD_H_HL:
			ss << " LD H, (HL)"; break;
		case LD_L_A:
			ss << " LD L, A"; break;
		case LD_L_B:
			ss << " LD L, B"; break;
		case LD_L_C:
			ss << " LD L, C"; break;
		case LD_L_D:
			ss << " LD L, D"; break;
		case LD_L_E:
			ss << " LD L, E"; break;
		case LD_L_H:
			ss << " LD L, H"; break;
		case LD_L_HL:
			ss << " LD L, (HL)"; break;
		case LD_HL_A:
			ss << " LD (HL), A"; break;
		case LD_HL_B:
			ss << " LD (HL), B"; break;
		case LD_HL_C:
			ss << " LD (HL), C"; break;
		case LD_HL_D:
			ss << " LD (HL), D"; break;
		case LD_HL_E:
			ss << " LD (HL), E"; break;
		case LD_HL_H:
			ss << " LD (HL), H"; break;
		case LD_HL_L:
			ss << " LD (HL), L"; break;
		case LD__HL__n:
			ss << " LD (HL), " << hex << setw(2) << setfill('0') << param1; break;
		case LD_BC_nn:
			ss << " LD BC, " << hex << setw(2) << setfill('0') << param1 << " " << hex << setw(2) << setfill('0') << param2; break;
		case LD_DE_nn:
			ss << " LD DE, " << hex << setw(2) << setfill('0') << param1 << " " << hex << setw(2) << setfill('0') << param2; break;
		case LD_HL_nn:
			ss << " LD HL, " << hex << setw(2) << setfill('0') << param1 << " " << hex << setw(2) << setfill('0') << param2; break;
		case LD_SP_nn:
			ss << " LD SP, " << hex << setw(2) << setfill('0') << param1 << " " << hex << setw(2) << setfill('0') << param2; break;
		case LD_nn_SP:
			ss << " LD " << hex << setw(2) << setfill('0') << param1 << " " << hex << setw(2) << setfill('0') << param2 << ", SP"; break;
		case LD_BC_A:
			ss << " LD (BC), A"; break;
		case LD_DE_A:
			ss << " LD (DE), A"; break;
		case LD_HLP_A:
			ss << " LD (HL+), A"; break;
		case LD_HLM_A:
			ss << " LD (HL-), A"; break;
		case LD_B_n:
			ss << " LD B, " << hex << setw(2) << setfill('0') << param1; break;
		case LD_C_n:
			ss << " LD C, " << hex << setw(2) << setfill('0') << param1; break;
		case LD_D_n:
			ss << " LD D, " << hex << setw(2) << setfill('0') << param1; break;
		case LD_E_n:
			ss << " LD E, " << hex << setw(2) << setfill('0') << param1; break;
		case LD_H_n:
			ss << " LD H, " << hex << setw(2) << setfill('0') << param1; break;
		case LD_L_n:
			ss << " LD L, " << hex << setw(2) << setfill('0') << param1; break;
		case LD_A_n:
			ss << " LD A, " << hex << setw(2) << setfill('0') << param1; break;
		case LD_A_BC:
			ss << " LD A, (BC)"; break;
		case LD_A_DE:
			ss << " LD A, (DE)"; break;
		case LD_A_HLP:
			ss << " LD A, (HL+)"; break;
		case LD_A_HLM:
			ss << " LD A, (HL-)"; break;
		case LD__C__A:
			ss << " LD (C), A"; break;
		case LD_A__C_:
			ss << " LD A, (C)"; break;
		case LDH_n_A:
			ss << " LDH (" << hex << setw(2) << setfill('0') << param1 << "), A"; break;
		case LDH_A_n:
			ss << " LDH A, (" << hex << setw(2) << setfill('0') << param1 << ")"; break;
		case LD_nn_A:
			ss << " LD (" << hex << setw(2) << setfill('0') << param2 << " " << hex << setw(2) << setfill('0') << param1 << "), A"; break;
		case LD_A_nn:
			ss << " LD A, " << hex << setw(2) << setfill('0') << param1 << " " << hex << setw(2) << setfill('0') << param2; break;
		case LD_HL_SP_n:
			ss << " LD HL, SP+" << hex << setw(2) << setfill('0') << param1; break;
		case LD_SP_HL:
			ss << " LD SP, HL"; break;
		case AND_A:
			ss << " AND A, A"; break;
		case AND_B:
			ss << " AND A, B"; break;
		case AND_C:
			ss << " AND A, C"; break;
		case AND_D:
			ss << " AND A, D"; break;
		case AND_E:
			ss << " AND A, E"; break;
		case AND_H:
			ss << " AND A, H"; break;
		case AND_L:
			ss << " AND A, L"; break;
		case AND_HL:
			ss << " AND A, (HL)"; break;
		case AND_n:
			ss << " AND " << hex << setw(2) << setfill('0') << param1; break;
		case XOR_A:
			ss << " XOR A"; break;
		case XOR_B:
			ss << " XOR B"; break;
		case XOR_C:
			ss << " XOR C"; break;
		case XOR_D:
			ss << " XOR D"; break;
		case XOR_E:
			ss << " XOR E"; break;
		case XOR_H:
			ss << " XOR H"; break;
		case XOR_L:
			ss << " XOR L"; break;
		case XOR_HL:
			ss << " XOR (HL)"; break;
		case XOR_n:
			ss << " XOR " << hex << setw(2) << setfill('0') << param1; break;
		case OR_A:
			ss << " OR A"; break;
		case OR_B:
			ss << " OR B"; break;
		case OR_C:
			ss << " OR C"; break;
		case OR_D:
			ss << " OR D"; break;
		case OR_E:
			ss << " OR E"; break;
		case OR_H:
			ss << " OR H"; break;
		case OR_L:
			ss << " OR L"; break;
		case OR_HL:
			ss << " OR (HL)"; break;
		case OR_n:
			ss << " OR " << hex << setw(2) << setfill('0') << param1; break;
		case ADD_A_A:
			ss << " ADD A, A"; break;
		case ADD_A_B:
			ss << " ADD A, B"; break;
		case ADD_A_C:
			ss << " ADD A, C"; break;
		case ADD_A_D:
			ss << " ADD A, D"; break;
		case ADD_A_E:
			ss << " ADD A, E"; break;
		case ADD_A_H:
			ss << " ADD A, H"; break;
		case ADD_A_L:
			ss << " ADD A, L"; break;
		case ADD_A_HL:
			ss << " ADD A, (HL)"; break;
		case ADD_SP_n:
			ss << " ADD SP, " << hex << setw(2) << setfill('0') << param1; break;
		case ADD_HL_BC:
			ss << " ADD HL, BC"; break;
		case ADD_HL_DE:
			ss << " %04x ADD HL, DE"; break;
		case ADD_HL_HL:
			ss << " ADD HL, HL"; break;
		case ADD_HL_SP:
			ss << " ADD HL, SP"; break;
		case ADD_A_n:
			ss << " ADD A, " << hex << setw(2) << setfill('0') << param1; break;
		case ADC_A_A:
			ss << " ADC A, A"; break;
		case ADC_A_B:
			ss << " ADC A, B"; break;
		case ADC_A_C:
			ss << " ADC A, C"; break;
		case ADC_A_D:
			ss << " ADC A, D"; break;
		case ADC_A_E:
			ss << " ADC A, E"; break;
		case ADC_A_H:
			ss << " ADC A, H"; break;
		case ADC_A_L:
			ss << " ADC A, L"; break;
		case ADC_A_HL:
			ss << " ADC A, (HL)"; break;
		case ADC_A_n:
			ss << " ADC A, " << hex << setw(2) << setfill('0') << param1; break;
		case SUB_A:
			ss << " SUB A"; break;
		case SUB_B:
			ss << " SUB B"; break;
		case SUB_C:
			ss << " SUB C"; break;
		case SUB_D:
			ss << " SUB D"; break;
		case SUB_E:
			ss << " SUB E"; break;
		case SUB_H:
			ss << " SUB H"; break;
		case SUB_L:
			ss << " SUB L"; break;
		case SUB_HL:
			ss << " SUB (HL)"; break;
		case SUB_n:
			ss << " SUB " << hex << setw(2) << setfill('0') << param1; break;
		case SBC_A_A:
			ss << " SBC A, A"; break;
		case SBC_A_B:
			ss << " SBC A, B"; break;
		case SBC_A_C:
			ss << " SBC A, C"; break;
		case SBC_A_D:
			ss << " SBC A, D"; break;
		case SBC_A_E:
			ss << " SBC A, E"; break;
		case SBC_A_H:
			ss << " SBC A, H"; break;
		case SBC_A_L:
			ss << " SBC A, L"; break;
		case SBC_A_HL:
			ss << " SBC A, (HL)"; break;
		case SBC_A_n:
			ss << " SBC A, " << hex << setw(2) << setfill('0') << param1; break;
		case CP_A:
			ss << " CP A"; break;
		case CP_B:
			ss << " CP B"; break;
		case CP_C:
			ss << " CP C"; break;
		case CP_D:
			ss << " CP D"; break;
		case CP_E:
			ss << " CP E"; break;
		case CP_H:
			ss << " CP H"; break;
		case CP_L:
			ss << " CP L"; break;
		case CP_HL:
			ss << " CP (HL)"; break;
		case CP_n:
			ss << " CP " << hex << setw(2) << setfill('0') << param1; break;
		case INC_A:
			ss << " INC A"; break;
		case INC_B:
			ss << " INC B";  break;
		case INC_C:
			ss << " INC C";  break;
		case INC_D:
			ss << " INC D"; break;
		case INC_E:
			ss << " INC E";  break;
		case INC_H:
			ss << " INC H"; break;
		case INC_L:
			ss << " INC L";  break;
		case INC__HL_:
			ss << " INC (HL)"; break;
		case INC_BC:
			ss << " INC BC"; break;
		case INC_DE:
			ss << " INC DE"; break;
		case INC_HL:
			ss << " INC HL"; break;
		case INC_SP:
			ss << " INC SP"; break;
		case DEC_A:
			ss << " DEC A"; break;
		case DEC_B:
			ss << " DEC B";  break;
		case DEC_C:
			ss << " DEC C";  break;
		case DEC_D:
			ss << " DEC D"; break;
		case DEC_E:
			ss << " DEC E";  break;
		case DEC_H:
			ss << " DEC H"; break;
		case DEC_L:
			ss << " DEC L";  break;
		case DEC__HL_:
			ss << " DEC (HL)"; break;
		case DEC_BC:
			ss << " DEC BC"; break;
		case DEC_DE:
			ss << " DEC DE"; break;
		case DEC_HL:
			ss << " DEC HL"; break;
		case DEC_SP:
			ss << " DEC SP"; break;
		case PUSH_AF:
			ss << " PUSH AF"; break;
		case PUSH_DE:
			ss << " PUSH DE"; break;
		case PUSH_BC:
			ss << " PUSH BC"; break;
		case PUSH_HL:
			ss << " PUSH HL"; break;
		case POP_AF:
			ss << " POP AF"; break;
		case POP_DE:
			ss << " POP DE"; break;
		case POP_BC:
			ss << " POP BC"; break;
		case POP_HL:
			ss << " POP HL"; break;
		case JR_NZ_n:
			ss << " JR NZ, " << hex << setw(2) << setfill('0') << param1; break;
		case JR_NC_n:
			ss << " JR NC, " << hex << setw(2) << setfill('0') << param1; break;
		case JR_Z_n:
			ss << " JR Z, " << hex << setw(2) << setfill('0') << param1; break;
		case JR_C_n:
			ss << " JR C, " << hex << setw(2) << setfill('0') << param1; break;
		case JR_n:
			ss << " JR " << hex << setw(2) << setfill('0') << param1; break;
		case JP_NC_nn:
			ss << " JP NC, " << hex << setw(2) << setfill('0') << param1 << " " << hex << setw(2) << setfill('0') << param2; break;
		case JP_NZ_nn:
			ss << " JP NZ, " << hex << setw(2) << setfill('0') << param1 << " " << hex << setw(2) << setfill('0') << param2; break;
		case JP_C_nn:
			ss << " JP C, " << hex << setw(2) << setfill('0') << param1 << " " << hex << setw(2) << setfill('0') << param2; break;
		case JP_Z_nn:
			ss << " JP Z, " << hex << setw(2) << setfill('0') << param1 << " " << hex << setw(2) << setfill('0') << param2; break;
		case JP_nn:
			ss << " JP " << hex << setw(2) << setfill('0') << param1 << " " << hex << setw(2) << setfill('0') << param2; break;
		case JP_HL:
			ss << " JP (HL)"; break;
		case CALL_NC_nn:
			ss << " CALL NC, " << hex << setw(2) << setfill('0') << param1 << " " << hex << setw(2) << setfill('0') << param2; break;
		case CALL_NZ_nn:
			ss << " CALL NZ, " << hex << setw(2) << setfill('0') << param1 << " " << hex << setw(2) << setfill('0') << param2; break;
		case CALL_Z_nn:
			ss << " CALL Z, " << hex << setw(2) << setfill('0') << param1 << " " << hex << setw(2) << setfill('0') << param2; break;
		case CALL_C_nn:
			ss << " CALL C, " << hex << setw(2) << setfill('0') << param1 << " " << hex << setw(2) << setfill('0') << param2; break;
		case CALL_nn:
			ss << " CALL " << hex << setw(2) << setfill('0') << param1 << " " << hex << setw(2) << setfill('0') << param2; break;
		case RET_NC:
			ss << " RET NC"; break;
		case RET_NZ:
			ss << " RET NZ"; break;
		case RET_Z:
			ss << " RET Z"; break;
		case RET_C:
			ss << " RET C"; break;
		case RET:
			ss << " RET"; break;
		case RETI:
			ss << " RETI"; break;
		case RST_00H:
			ss << " RST 00H"; break;
		case RST_10H:
			ss << " RST 10H"; break;
		case RST_20H:
			ss << " RST 20H"; break;
		case RST_30H:
			ss << " RST 30H"; break;
		case RST_08H:
			ss << " RST 08H"; break;
		case RST_18H:
			ss << " RST 18H"; break;
		case RST_28H:
			ss << " RST 28H"; break;
		case RST_38H:
			ss << " RST 38H"; break;
		case RRCA:
			ss << " RRCA"; break;
		case RRA_:
			ss << " RRA"; break;
		case RLCA:
			ss << " RLCA"; break;
		case RLA:
			ss << " RLA"; break;
		case SCF:
			ss << " SCF"; break;
		case CCF:
			ss << " CCF"; break;
		case CPL:
			ss << " CPL"; break;
		case HALT:
			ss << " HALT"; break;
		case STOP:
			ss << " STOP"; break;
		case DI:
			ss << " DI"; break;
		case EI:
			ss << " EI"; break;
		default:
			ss << " Undefined"; break;
	}

	return ss.str();
}
const string Commands::CBCodeToString(uint8_t opcode) {
	stringstream ss;

	switch (opcode) {
	case SWAP_A:
		ss << "SWAP A"; break;
	case SWAP_B:
		ss << "SWAP B"; break;
	case SWAP_C:
		ss << "SWAP C"; break;
	case SWAP_D:
		ss << "SWAP D"; break;
	case SWAP_E:
		ss << "SWAP E"; break;
	case SWAP_H:
		ss << "SWAP H"; break;
	case SWAP_L:
		ss << "SWAP L"; break;
	case SWAP_HL:
		ss << "SWAP (HL)"; break;
	case BIT0_B:
		ss << "BIT 0, B"; break;
	case BIT0_C:
		ss << "BIT 0, C"; break;
	case BIT0_D:
		ss << "BIT 0, D"; break;
	case BIT0_E:
		ss << "BIT 0, E"; break;
	case BIT0_H:
		ss << "BIT 0, H"; break;
	case BIT0_L:
		ss << "BIT 0, L"; break;
	case BIT0_HL:
		ss << "BIT 0, (HL)"; break;
	case BIT0_A:
		ss << "BIT 0, A"; break;
	case BIT1_B:
		ss << "BIT 1, B"; break;
	case BIT1_C:
		ss << "BIT 1, C"; break;
	case BIT1_D:
		ss << "BIT 1, D"; break;
	case BIT1_E:
		ss << "BIT 1, E"; break;
	case BIT1_H:
		ss << "BIT 1, H"; break;
	case BIT1_L:
		ss << "BIT 1, L"; break;
	case BIT1_HL:
		ss << "BIT 1, (HL)"; break;
	case BIT1_A:
		ss << "BIT 1, A"; break;
	case BIT2_B:
		ss << "BIT 2, B"; break;
	case BIT2_C:
		ss << "BIT 2, C"; break;
	case BIT2_D:
		ss << "BIT 2, D"; break;
	case BIT2_E:
		ss << "BIT 2, E"; break;
	case BIT2_H:
		ss << "BIT 2, H"; break;
	case BIT2_L:
		ss << "BIT 2, L"; break;
	case BIT2_HL:
		ss << "BIT 2, (HL)"; break;
	case BIT2_A:
		ss << "BIT 2, A"; break;
	case BIT3_B:
		ss << "BIT 3, B"; break;
	case BIT3_C:
		ss << "BIT 3, C"; break;
	case BIT3_D:
		ss << "BIT 3, D"; break;
	case BIT3_E:
		ss << "BIT 3, E"; break;
	case BIT3_H:
		ss << "BIT 3, H"; break;
	case BIT3_L:
		ss << "BIT 3, L"; break;
	case BIT3_HL:
		ss << "BIT 3, (HL)"; break;
	case BIT3_A:
		ss << "BIT 3, A"; break;
	case BIT4_B:
		ss << "BIT 4, B"; break;
	case BIT4_C:
		ss << "BIT 4, C"; break;
	case BIT4_D:
		ss << "BIT 4, D"; break;
	case BIT4_E:
		ss << "BIT 4, E"; break;
	case BIT4_H:
		ss << "BIT 4, H"; break;
	case BIT4_L:
		ss << "BIT 4, L"; break;
	case BIT4_HL:
		ss << "BIT 4, (HL)"; break;
	case BIT4_A:
		ss << "BIT 4, A"; break;
	case BIT5_B:
		ss << "BIT 5, B"; break;
	case BIT5_C:
		ss << "BIT 5, C"; break;
	case BIT5_D:
		ss << "BIT 5, D"; break;
	case BIT5_E:
		ss << "BIT 5, E"; break;
	case BIT5_H:
		ss << "BIT 5, H"; break;
	case BIT5_L:
		ss << "BIT 5, L"; break;
	case BIT5_HL:
		ss << "BIT 5, (HL)"; break;
	case BIT5_A:
		ss << "BIT 5, A"; break;
	case BIT6_B:
		ss << "BIT 6, B"; break;
	case BIT6_C:
		ss << "BIT 6, C"; break;
	case BIT6_D:
		ss << "BIT 6, D"; break;
	case BIT6_E:
		ss << "BIT 6, E"; break;
	case BIT6_H:
		ss << "BIT 6, H"; break;
	case BIT6_L:
		ss << "BIT 6, L"; break;
	case BIT6_HL:
		ss << "BIT 6, (HL)"; break;
	case BIT6_A:
		ss << "BIT 6, A"; break;
	case BIT7_B:
		ss << "BIT 7, B"; break;
	case BIT7_C:
		ss << "BIT 7, C"; break;
	case BIT7_D:
		ss << "BIT 7, D"; break;
	case BIT7_E:
		ss << "BIT 7, E"; break;
	case BIT7_H:
		ss << "BIT 7, H"; break;
	case BIT7_L:
		ss << "BIT 7, L"; break;
	case BIT7_HL:
		ss << "BIT 7, (HL)"; break;
	case BIT7_A:
		ss << "BIT 7, A"; break;
	case RES0_B:
		ss << "RES 0, B"; break;
	case RES0_C:
		ss << "RES 0, C"; break;
	case RES0_D:
		ss << "RES 0, D"; break;
	case RES0_E:
		ss << "RES 0, E"; break;
	case RES0_H:
		ss << "RES 0, H"; break;
	case RES0_L:
		ss << "RES 0, L"; break;
	case RES0_HL:
		ss << "RES 0, (HL)"; break;
	case RES0_A:
		ss << "RES 0, A"; break;
	case RES1_B:
		ss << "RES 1, B"; break;
	case RES1_C:
		ss << "RES 1, C"; break;
	case RES1_D:
		ss << "RES 1, D"; break;
	case RES1_E:
		ss << "RES 1, E"; break;
	case RES1_H:
		ss << "RES 1, H"; break;
	case RES1_L:
		ss << "RES 1, L"; break;
	case RES1_HL:
		ss << "RES 1, (HL)"; break;
	case RES1_A:
		ss << "RES 1, A"; break;
	case RES2_B:
		ss << "RES 2, B"; break;
	case RES2_C:
		ss << "RES 2, C"; break;
	case RES2_D:
		ss << "RES 2, D"; break;
	case RES2_E:
		ss << "RES 2, E"; break;
	case RES2_H:
		ss << "RES 2, H"; break;
	case RES2_L:
		ss << "RES 2, L"; break;
	case RES2_HL:
		ss << "RES 2, (HL)"; break;
	case RES2_A:
		ss << "RES 2, A"; break;
	case RES3_B:
		ss << "RES 3, B"; break;
	case RES3_C:
		ss << "RES 3, C"; break;
	case RES3_D:
		ss << "RES 3, D"; break;
	case RES3_E:
		ss << "RES 3, E"; break;
	case RES3_H:
		ss << "RES 3, H"; break;
	case RES3_L:
		ss << "RES 3, L"; break;
	case RES3_HL:
		ss << "RES 3, (HL)"; break;
	case RES3_A:
		ss << "RES 3, A"; break;
	case RES4_B:
		ss << "RES 4, B"; break;
	case RES4_C:
		ss << "RES 4, C"; break;
	case RES4_D:
		ss << "RES 4, D"; break;
	case RES4_E:
		ss << "RES 4, E"; break;
	case RES4_H:
		ss << "RES 4, H"; break;
	case RES4_L:
		ss << "RES 4, L"; break;
	case RES4_HL:
		ss << "RES 4, (HL)"; break;
	case RES4_A:
		ss << "RES 4, A"; break;
	case RES5_B:
		ss << "RES 5, B"; break;
	case RES5_C:
		ss << "RES 5, C"; break;
	case RES5_D:
		ss << "RES 5, D"; break;
	case RES5_E:
		ss << "RES 5, E"; break;
	case RES5_H:
		ss << "RES 5, H"; break;
	case RES5_L:
		ss << "RES 5, L"; break;
	case RES5_HL:
		ss << "RES 5, (HL)"; break;
	case RES5_A:
		ss << "RES 5, A"; break;
	case RES6_B:
		ss << "RES 6, B"; break;
	case RES6_C:
		ss << "RES 6, C"; break;
	case RES6_D:
		ss << "RES 6, D"; break;
	case RES6_E:
		ss << "RES 6, E"; break;
	case RES6_H:
		ss << "RES 6, H"; break;
	case RES6_L:
		ss << "RES 6, L"; break;
	case RES6_HL:
		ss << "RES 6, (HL)"; break;
	case RES6_A:
		ss << "RES 6, A"; break;
	case RES7_B:
		ss << "RES 7, B"; break;
	case RES7_C:
		ss << "RES 7, C"; break;
	case RES7_D:
		ss << "RES 7, D"; break;
	case RES7_E:
		ss << "RES 7, E"; break;
	case RES7_H:
		ss << "RES 7, H"; break;
	case RES7_L:
		ss << "RES 7, L"; break;
	case RES7_HL:
		ss << "RES 7, (HL)"; break;
	case RES7_A:
		ss << "RES 7, A"; break;
	case SET0_B:
		ss << "SET 0, B"; break;
	case SET0_C:
		ss << "SET 0, C"; break;
	case SET0_D:
		ss << "SET 0, D"; break;
	case SET0_E:
		ss << "SET 0, E"; break;
	case SET0_H:
		ss << "SET 0, H"; break;
	case SET0_L:
		ss << "SET 0, L"; break;
	case SET0_HL:
		ss << "SET 0, (HL)"; break;
	case SET0_A:
		ss << "SET 0, A"; break;
	case SET1_B:
		ss << "SET 1, B"; break;
	case SET1_C:
		ss << "SET 1, C"; break;
	case SET1_D:
		ss << "SET 1, D"; break;
	case SET1_E:
		ss << "SET 1, E"; break;
	case SET1_H:
		ss << "SET 1, H"; break;
	case SET1_L:
		ss << "SET 1, L"; break;
	case SET1_HL:
		ss << "SET 1, (HL)"; break;
	case SET1_A:
		ss << "SET 1, A"; break;
	case SET2_B:
		ss << "SET 2, B"; break;
	case SET2_C:
		ss << "SET 2, C"; break;
	case SET2_D:
		ss << "SET 2, D"; break;
	case SET2_E:
		ss << "SET 2, E"; break;
	case SET2_H:
		ss << "SET 2, H"; break;
	case SET2_L:
		ss << "SET 2, L"; break;
	case SET2_HL:
		ss << "SET 2, (HL)"; break;
	case SET2_A:
		ss << "SET 2, A"; break;
	case SET3_B:
		ss << "SET 3, B"; break;
	case SET3_C:
		ss << "SET 3, C"; break;
	case SET3_D:
		ss << "SET 3, D"; break;
	case SET3_E:
		ss << "SET 3, E"; break;
	case SET3_H:
		ss << "SET 3, H"; break;
	case SET3_L:
		ss << "SET 3, L"; break;
	case SET3_HL:
		ss << "SET 3, (HL)"; break;
	case SET3_A:
		ss << "SET 3, A"; break;
	case SET4_B:
		ss << "SET 4, B"; break;
	case SET4_C:
		ss << "SET 4, C"; break;
	case SET4_D:
		ss << "SET 4, D"; break;
	case SET4_E:
		ss << "SET 4, E"; break;
	case SET4_H:
		ss << "SET 4, H"; break;
	case SET4_L:
		ss << "SET 4, L"; break;
	case SET4_HL:
		ss << "SET 4, (HL)"; break;
	case SET4_A:
		ss << "SET 4, A"; break;
	case SET5_B:
		ss << "SET 5, B"; break;
	case SET5_C:
		ss << "SET 5, C"; break;
	case SET5_D:
		ss << "SET 5, D"; break;
	case SET5_E:
		ss << "SET 5, E"; break;
	case SET5_H:
		ss << "SET 5, H"; break;
	case SET5_L:
		ss << "SET 5, L"; break;
	case SET5_HL:
		ss << "SET 5, (HL)"; break;
	case SET5_A:
		ss << "SET 5, A"; break;
	case SET6_B:
		ss << "SET 6, B"; break;
	case SET6_C:
		ss << "SET 6, C"; break;
	case SET6_D:
		ss << "SET 6, D"; break;
	case SET6_E:
		ss << "SET 6, E"; break;
	case SET6_H:
		ss << "SET 6, H"; break;
	case SET6_L:
		ss << "SET 6, L"; break;
	case SET6_HL:
		ss << "SET 6, (HL)"; break;
	case SET6_A:
		ss << "SET 6, A"; break;
	case SET7_B:
		ss << "SET 7, B"; break;
	case SET7_C:
		ss << "SET 7, C"; break;
	case SET7_D:
		ss << "SET 7, D"; break;
	case SET7_E:
		ss << "SET 7, E"; break;
	case SET7_H:
		ss << "SET 7, H"; break;
	case SET7_L:
		ss << "SET 7, L"; break;
	case SET7_HL:
		ss << "SET 7, (HL)"; break;
	case SET7_A:
		ss << "SET 7, A"; break;
	case RLC_B:
		ss << "RLC B"; break;
	case RLC_C:
		ss << "RLC C"; break;
	case RLC_D:
		ss << "RLC D"; break;
	case RLC_E:
		ss << "RLC E"; break;
	case RLC_H:
		ss << "RLC H"; break;
	case RLC_L:
		ss << "RLC L"; break;
	case RLC_HL:
		ss << "RLC (HL)"; break;
	case RLC_A:
		ss << "RLC A"; break;
	case RRC_B:
		ss << "RRC B"; break;
	case RRC_C:
		ss << "RRC B"; break;
	case RRC_D:
		ss << "RRC C"; break;
	case RRC_E:
		ss << "RRC E"; break;
	case RRC_H:
		ss << "RRC H"; break;
	case RRC_L:
		ss << "RRC L"; break;
	case RRC_HL:
		ss << "RRC (HL)"; break;
	case RRC_A:
		ss << "RRC A"; break;
	case RL_A:
		ss << "RL A"; break;
	case RL_B:
		ss << "RL B"; break;
	case RL_C:
		ss << "RL C"; break;
	case RL_D:
		ss << "RL D"; break;
	case RL_E:
		ss << "RL E"; break;
	case RL_H:
		ss << "RL H"; break;
	case RL_L:
		ss << "RL L"; break;
	case RL_HL:
		ss << "RL (HL)"; break;
	case RR_A:
		ss << "RR A"; break;
	case RR_B:
		ss << "RR B"; break;
	case RR_C:
		ss << "RR C"; break;
	case RR_D:
		ss << "RR D"; break;
	case RR_E:
		ss << "RR E"; break;
	case RR_H:
		ss << "RR H"; break;
	case RR_L:
		ss << "RR L"; break;
	case RR_HL:
		ss << "RR (HL)"; break;
	case SRA_A:
		ss << "SRA A"; break;
	case SRA_B:
		ss << "SRA B"; break;
	case SRA_C:
		ss << "SRA C"; break;
	case SRA_D:
		ss << "SRA D"; break;
	case SRA_E:
		ss << "SRA E"; break;
	case SRA_L:
		ss << "SRA H"; break;
	case SRA_H:
		ss << "SRA L"; break;
	case SRA_HL:
		ss << "SRA (HL)"; break;
	case SLA_A:
		ss << "SLA A"; break;
	case SLA_B:
		ss << "SLA B"; break;
	case SLA_C:
		ss << "SLA C"; break;
	case SLA_D:
		ss << "SLA D"; break;
	case SLA_E:
		ss << "SLA E"; break;
	case SLA_L:
		ss << "SLA H"; break;
	case SLA_H:
		ss << "SLA L"; break;
	case SLA_HL:
		ss << "SLA (HL)"; break;
	case SRL_A:
		ss << "SRL A"; break;
	case SRL_B:
		ss << "SRL B"; break;
	case SRL_C:
		ss << "SRL C"; break;
	case SRL_D:
		ss << "SRL D"; break;
	case SRL_E:
		ss << "SRL E"; break;
	case SRL_L:
		ss << "SRL H"; break;
	case SRL_H:
		ss << "SRL L"; break;
	case SRL_HL:
		ss << "SRL (HL)"; break;
	default:
		ss << "Undefined"; break;

	}
	return ss.str();
}
