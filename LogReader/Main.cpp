#include <iostream>
#include "opcodes.h"
#include <iomanip>
#include <sstream>
#include <conio.h>

using namespace std;

short GetParameters(uint8_t opcode);
string CodeToString(uint8_t opcode, uint16_t PC, uint16_t param1, uint16_t param2);
string CBCodeToString(uint8_t opcode);

int main(int argc, char* argv[]) {
	std::stringstream st;

	if (argc < 2) {
		cout << "Usage: %s [logfile input]\n" << argv[0] << endl;
		return 1;
	}

	int i = 0;
	FILE* f;
	int err = fopen_s(&f, argv[1], "rb");
	if (err > 0 || f == 0) {
		cout << "Couldn't open file '" << argv[1] << "'" << endl;
		return 2;
	}

	int params = 0;
	int skipUntil = 0;
	bool silentMode = false;
	string regcheck = "";
	uint16_t regval = 0;

	int c = 0, p1 = 0, p2 = 0, pc = 0, reg = 0;
	uint16_t af = 0, bc = 0, de = 0, hl = 0, sp = 0;
	do {
		c = (int)fgetc(f);
		pc = c;
		c = (int)fgetc(f);
		pc += c << 8;

		c = (int)fgetc(f);
		params = GetParameters(c);
		if (params > 0) {
			p1 = (int)fgetc(f);
		}
		if (params > 1) {
			p2 = (int)fgetc(f);
		}

		if (skipUntil == pc) {
			skipUntil = 0;
			silentMode = false;
		}

		if (!silentMode)
			cout << CodeToString(c, pc, p1, p2);
		//for (i = 0; i < params; i++) {

		//}
		//cout << st.str();

		c = (int)fgetc(f);
		af = c;
		c = (int)fgetc(f);
		af += c << 8;
		if(!silentMode)
			cout << ", AF:" << hex << setw(4) << setfill('0') << af;
		c = (int)fgetc(f);
		bc = c;
		c = (int)fgetc(f);
		bc += c << 8;
		if (!silentMode)
			cout << ", BC:" << hex << setw(4) << setfill('0') << bc;
		c = (int)fgetc(f);
		de = c;
		c = (int)fgetc(f);
		de += c << 8;
		if (!silentMode)
			cout << ", DE:" << hex << setw(4) << setfill('0') << de;
		c = (int)fgetc(f);
		hl = c;
		c = (int)fgetc(f);
		hl += c << 8;
		if (!silentMode)
			cout << ", HL:" << hex << setw(4) << setfill('0') << hl;
		c = (int)fgetc(f);
		sp = c;
		c = (int)fgetc(f);
		sp += c << 8;
		if (!silentMode)
			cout << ", SP:" << hex << setw(4) << setfill('0') << sp << endl;

		if (regcheck != "") {
			if ((regcheck.compare("hl") == 0 && hl == regval) || (regcheck.compare("af") == 0 && af == regval) || (regcheck.compare("bc") == 0 && bc == regval) 
					|| (regcheck.compare("de") == 0 && de == regval) || (regcheck.compare("sp") == 0 && sp == regval) ||
				(regcheck.compare("h") == 0 && ((hl >> 8) & 0xff) == (regval & 0xff)) || (regcheck.compare("l") == 0 && (hl & 0xff) == (regval & 0xff)) ||
				(regcheck.compare("a") == 0 && ((af >> 8) & 0xff) == (regval & 0xff)) || (regcheck.compare("f") == 0 && (af & 0xff) == (regval & 0xff)) ||
				(regcheck.compare("b") == 0 && ((bc >> 8) & 0xff) == (regval & 0xff)) || (regcheck.compare("c") == 0 && (bc & 0xff) == (regval & 0xff)) ||
				(regcheck.compare("d") == 0 && ((de >> 8) & 0xff) == (regval & 0xff)) || (regcheck.compare("e") == 0 && (de & 0xff) == (regval & 0xff))
				) {
					skipUntil = 0; regcheck = "";
			}
		}

		if (skipUntil == 0) {
			char v = _getch();
			switch (v) {
				case 's':
					cin >> hex >> skipUntil; break;
				case 'S':
					cout << "Skip silent until: ";
					cin >> hex >> skipUntil; silentMode = true; break;
				case 'r':
					skipUntil = -1;
					cout << "Enter register: ";
					cin >> regcheck;
					cout << "Break when " << reg << " hits: ";
					cin >> hex >> regval; break;
			}
		}
	} while (c != EOF);
	fclose(f);

	return 0;
}


// Figure out how many paramters the given instruction requires
short GetParameters(uint8_t opcode) {
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
		//printf("param1=%02x, param2=%02x, PC=%04x, PC+1=%04x, PC+2=%04x\n", *param1, *param2, PC, PC+1, PC+2);
		numParams = 2; break;

	}

	return numParams;
}

string CodeToString(uint8_t opcode, uint16_t PC, uint16_t param1, uint16_t param2) {
	stringstream ss;

	ss << hex << setw(4) << setfill('0') << PC;

	switch (opcode) {
	case CB:
		ss << " CB "; break;
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
		ss << " AND %x"; break;
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
string CBCodeToString(uint8_t opcode) {
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