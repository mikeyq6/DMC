#include "Log.h"
#include "opcodes.h"


Log::Log(Memory* _memory, Registers* _registers) {
	memory = _memory;
	registers = _registers;
}
Log::~Log() {}

short Log::GetParameters(uint8_t opcode, uint8_t* param1, uint8_t* param2) {
	short numParams = 0;

	switch (opcode) {
		case XOR_n:
		case LD_A_n:
		case LD_B_n:
		case LD_C_n:
		case LD_D_n:
		case LD_E_n:
		case LD_H_n:
		case LD_L_n:
		case LD__HL__n:
		case LD_HL_SP_n:
		case CB:
		case JR_NZ_n:
		case JR_NC_n:
		case JR_Z_n:
		case JR_C_n:
		case JR_n:
		case ADD_SP_n:
		case ADC_A_n:
		case SUB_n:
		case SBC_A_n:
		case AND_n:
		case OR_n:
		case CP_n:
		case ADD_A_n:
		case LDH_n_A:
		case LDH_A_n:
			*param1 = memory->ReadMem(registers->PC + 1);
			numParams = 1; break;
		case LD_BC_nn:
		case LD_DE_nn:
		case LD_HL_nn:
		case LD_SP_nn:
		case LD_nn_SP:
		case LD_nn_A:
		case LD_A_nn:
		case JP_NZ_nn:
		case JP_NC_nn:
		case JP_Z_nn:
		case JP_C_nn:
		case JP_nn:
		case CALL_NC_nn:
		case CALL_NZ_nn:
		case CALL_Z_nn:
		case CALL_C_nn:
		case CALL_nn:
			*param1 = memory->ReadMem(registers->PC + 1);
			*param2 = memory->ReadMem(registers->PC + 2);
			numParams = 2; break;

	}

	return numParams;
}