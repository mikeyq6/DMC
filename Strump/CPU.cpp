#include "CPU.h"

#include <stdio.h>
#include <string.h>
#include <sstream>
#include <stdlib.h>
#include <iomanip>




CPU::CPU() {
	rominfo = new ROMInfo();
	registers = new Registers();

	// Setup memory
	factory = new MemoryFactory();
	joypadState = new JoypadState();
	Halted = Stopped = WillEnableInterrupts = WillDisableInterrupts = IME = 0;
	memset(InstructionStats, 0, sizeof(InstructionStats));
	doHaltBug = false;

#ifdef LOG_COMMANDS
	clog = fopen("commands", "wb");
	if (clog == NULL) {
		cout << "Couldn't open file 'commands'" << endl;
	}
#endif
#ifdef LOG_STATS
	statlog = fopen("stats", "wb");
	if (statlog == NULL) {
		cout << "Couldn't open file 'stats'" << endl;
	}
#endif

}
CPU::~CPU() {
	delete rominfo;
	delete commands;
	delete memory;
	delete registers;
	delete factory;
	delete joypadState;
}

ROMInfo* CPU::GetRomInfo() {
	return rominfo;
}

void CPU::initCPU() {
	std::lock_guard<mutex> locker(cpu_mutex);

	rominfo->SetCartridgeInfo();
	memory = factory->GetMemoryByType(rominfo->CartInfo);
	memory->init(rominfo, &registers->AF.f, joypadState);

	memory->Startup = false;
	param1 = 0;
	param2 = 0;
	isPaused = false;
	stepModeActive = false;
	runNextStep = false;

	timer = new Timer(memory->GetPointerTo(TAC), memory->GetPointerTo(TIMA), memory->GetPointerTo(TMA), memory->GetPointerTo(DIV));
	commands = new Commands(memory, registers);

#ifdef RUNTESTS
	test = new Test(commands, memory, registers);
	test->TestInstructions();	
	delete test;

	Memory* mem5 = new MBC5Memory(false, false, true, true);
	mem5->init(rominfo, &registers->AF.f, joypadState);
	testMemory = new TestMemory(commands, mem5, registers);
	testMemory->RunTests();
	delete testMemory;

	testMBC5 = new TestMBC5(commands, mem5, registers);
	testMBC5->RunTests();
	delete mem5;
	delete testMBC5;
#endif

	setDefaults();
}

void CPU::setDefaults() {
	registers->AF.af = 0x01f0;
	registers->BC.bc = 0x0013;
	registers->DE.de = 0x00d8;
	registers->HL.hl = 0x014d;
	registers->SP = 0xfffe;
	registers->rDiv = 0;
	registers->PC = 0x100;
	memory->WriteMem(TIMA, 0x00); // TIMA
	memory->WriteMem(TMA, 0x00); // TMA
	memory->WriteMem(TAC, 0x00); // TAC
	memory->WriteMem(NR10, 0x80); // NR10
	memory->WriteMem(NR11, 0xbf); // NR11
	memory->WriteMem(NR12, 0xf3); // NR12
	memory->WriteMem(NR14, 0xbf); // NR14
	memory->WriteMem(NR21, 0x3f); // NR21
	memory->WriteMem(NR22, 0x00); // NR22
	memory->WriteMem(NR24, 0xbf); // NR24
	memory->WriteMem(NR30, 0x7f); // NR30
	memory->WriteMem(NR31, 0xff); // NR31
	memory->WriteMem(NR32, 0x9f); // NR32
	memory->WriteMem(NR33, 0xbf); // NR33
	memory->WriteMem(NR41, 0xff); // NR41
	memory->WriteMem(NR42, 0x00); // NR42
	memory->WriteMem(NR43, 0x00); // NR43
	memory->WriteMem(NR44, 0xbf); // NR44
	memory->WriteMem(NR50, 0x77); // NR50
	memory->WriteMem(NR51, 0xf3); // NR51
	memory->WriteMem(NR52, 0xf1); // NR52 GB
	memory->WriteMem(LCDC, 0x91); // LCDC
	memory->WriteMem(SCY, 0x00); // SCY
	memory->WriteMem(SCX, 0x00); // SCX
	memory->WriteMem(LYC, 0x00); // LYC
	memory->WriteMem(BGP, 0xfc); // BGP
	memory->WriteMem(OBP0, 0xff); // OBP0
	memory->WriteMem(OBP1, 0xff); // OBP1
	memory->WriteMem(WY, 0x00); // WY
	memory->WriteMem(WX, 0x00); // WX
	memory->WriteMem(IE, 0x00); // IE
	memory->WriteMem(IF, 0xe1); // IE
	//memory->WriteMem(STAT, 0x85); // IE
}


string CPU::GetLastInstruction() {
	return lastInst;
}

void CPU::Start() {

	stringstream strcmd;
	isRunning = true;
	uint16_t oldPC;
	chrono::system_clock::time_point tp;

	while(isRunning) {
		if (!(isPaused)) {

			if (Halted) {
				UpdateTimer(inst);

				if (IME) {
					CheckInterrupts();
				} else {
					if(memory->get(IE) & memory->get(IF)) {
						Halted = false;
					}
				}
			} else {
				tp = chrono::system_clock::now();
				oldPC = registers->PC;
				inst = GetNextInstruction();
				short params = GetParameters(inst, &param1, &param2);
 				if (WillEnableInterrupts) { 
					WillEnableInterrupts = 0; 
					IME = 1;
				}
				if (WillDisableInterrupts) {
					WillDisableInterrupts = 0;
					IME = 0;
				}

				//printf("%x ", InternalRom[i]);
				if (params == 0) {
					if (ShouldPrint()) {
						cout << commands->CodeToString(inst, registers->PC, static_cast<uint16_t>(param1), static_cast<uint16_t>(param2));
					}
					Run(inst, 0, 0);
				}
				else if (params == 1) {
					if (ShouldPrint()) {
						if (inst == CB) {
							cout << commands->CodeToString(inst, registers->PC, static_cast<uint16_t>(param1), static_cast<uint16_t>(param2)) << commands->CBCodeToString(param1);
						}
						else {
							cout << commands->CodeToString(inst, registers->PC, static_cast<uint16_t>(param1), static_cast<uint16_t>(param2));
						}
					}

					Run(inst, param1, 0);
				}
				else if (params == 2) {
					if (ShouldPrint()) {
						cout << commands->CodeToString(static_cast<uint16_t>(inst), registers->PC, static_cast<uint16_t>(param1), static_cast<uint16_t>(param2));
					}
					Run(inst, param1, param2);
				}
				if (ShouldPrint()) {
					cout << endl;
				}
				if(isHalting) {
					uint8_t inst1 = GetNextInstruction();
				}

#ifdef LOG_COMMANDS
				uint8_t _ie = memory->get(IE);
				uint8_t _if = memory->get(IF);
				LogCommand(&oldPC, &inst, &param1, &param2, &params, &IME, &_if, &_ie);
#endif
				#ifdef REAL_TIME_CPU
				DoCPUWait(&tp, inst);
				#endif
				UpdateTimer(inst);
				UpdateGraphics(inst);
				SetLCDStatus();

				if (IME) {
					CheckInterrupts();
				}
				if(doHaltBug && !isHalting) {
					doHaltBug = false;
					registers->PC = oldPC;
				}
				if(isHalting) {
					isHalting = false;
					Halted = true;
				}
			}
		}
	}

#ifdef LOG_STATS
	LogStats();
#endif
}

void CPU::Stop() {
	std::lock_guard<mutex> locker(cpu_mutex);

	isRunning = false;
}
bool CPU::IsRunning() {
	return isRunning;
}
bool CPU::IsHalted() {
	return Halted;
}
bool CPU::IsStopped() {
	return Stopped;
}
void CPU::Pause() {
	std::lock_guard<mutex> locker(cpu_mutex);

	isPaused = true;
}
void CPU::Unpause() {
	std::lock_guard<mutex> locker(cpu_mutex);

	isPaused = false;
}
bool CPU::IsPaused() {
	return isPaused;
}
void CPU::NextStep() {
	runNextStep = true;
}
void CPU::ActivateStepMode() {
	std::lock_guard<mutex> locker(cpu_mutex);

	stepModeActive = true;
	runNextStep = true;
}
void CPU::DeactivateStepMode() {
	std::lock_guard<mutex> locker(cpu_mutex);

	stepModeActive = false;
	runNextStep = false;
}
bool CPU::StepModeActive() {
	return stepModeActive;
}

short CPU::ShouldPrint() {
#ifdef PRINT_COMMANDS
	return 1;
#else
	return 0;
#endif
}

void CPU::DoCPUWait(chrono::system_clock::time_point* tp, uint8_t inst) {
	uint8_t cycles = GetCycles(inst);
	
	// Determine time instruction should take in nanoseconds
	long long duration = (cycles / (double)FREQ) * 1000000000;
	auto finish = *tp + chrono::nanoseconds(duration);
	this_thread::sleep_until(finish);

}


// InterruptsEnabled
void CPU::CheckInterrupts() {
	uint8_t bit = 1;
	uint8_t ifFlag = memory->get(IF);
	uint8_t ieFlag = memory->get(IE);

	// Check VBlank
	if (ifFlag & ieFlag & bit) {
		if(!Halted) {
			PushPCOntoStack();
			registers->PC = I_VBlank;
		}
		ResetInterrupt(I_VBlank);
		IME = 0;
		Halted = false;
	}  
	// Check LCDC
	if (ifFlag & ieFlag & (bit << 1)) {
		if(!Halted) {
			PushPCOntoStack();
			registers->PC = I_LCDC;
		}
		ResetInterrupt(I_LCDC); 
		IME = 0;
		Halted = false;
	} 
	// Check Timer
	if (ifFlag & ieFlag & (bit << 2)) {
		if(!Halted) {
			PushPCOntoStack();
			registers->PC = I_Timer;
		}
		ResetInterrupt(I_Timer);
		IME = 0;
		Halted = false;
	} 
	// Check Serial
	if (ifFlag & ieFlag & (bit << 3)) {
		if(!Halted) {
			PushPCOntoStack();
			registers->PC = I_Serial;
		}
		ResetInterrupt(I_Serial);
		IME = 0;
		Halted = false;
	}  
	// Check Joypad
	if (ifFlag & ieFlag & (bit << 4)) {
		if(!Halted) {
			PushPCOntoStack();
			registers->PC = I_Joypad;
		}
		ResetInterrupt(I_Joypad);
		IME = 0;
		Halted = false;
	}
}

void CPU::SetInterrupt(uint8_t iRegister) {
	uint8_t iFlag = memory->ReadMem(IF);

	switch (iRegister) {
		case I_VBlank:
			memory->set(IF, (iFlag | 0x01)); break;
		case I_LCDC:
			memory->set(IF, (iFlag | 0x02)); break;
		case I_Timer:
			memory->set(IF, (iFlag | 0x04)); break;
		case I_Serial:
			memory->set(IF, (iFlag | 0x08)); break;
		case I_Joypad:
			memory->set(IF, (iFlag | 0x0f)); break;
	}
}
void CPU::ResetInterrupt(uint8_t iRegister) {
	uint8_t iFlag = memory->ReadMem(IF);

	switch (iRegister) {
		case I_VBlank:
			memory->set(IF, (iFlag & 0xfe)); break;
		case I_LCDC:
			memory->set(IF, (iFlag & 0xfd)); break;
		case I_Timer:
			memory->set(IF, (iFlag & 0xfb)); break;
		case I_Serial:
			memory->set(IF, (iFlag & 0xf7)); break;
		case I_Joypad:
			memory->set(IF, (iFlag & 0xef)); break;
	}
}

void CPU::UpdateTimer(uint8_t opcode) {

	uint8_t cycles = GetCycles(opcode);
	if (timer->UpdateWillInterrupt(cycles)) {
		SetInterrupt(I_Timer);
	}

}

void CPU::PushPCOntoStack() {
	memory->WriteMem(--registers->SP, (uint8_t)(registers->PC >> 8));
	memory->WriteMem(--registers->SP, (uint8_t)(registers->PC & 0xff));
}

// Instructions 
void CPU::Run(uint8_t opcode, uint8_t param1, uint8_t param2) {
	uint8_t skipPCInc = 0;
	uint8_t p1, p2;
	InstructionStats[opcode]++;

	switch (opcode) {
		case CB:
			RunCBInstruction(param1); break;
		case NOP:
		case LD_A_A:
		case LD_B_B:
		case LD_C_C:
		case LD_E_E:
		case LD_H_H:
		case LD_L_L:
			break; // No effect
		case LD_A_B:
		case LD_A_C:
		case LD_A_D:
		case LD_A_E:
		case LD_A_H:
		case LD_A_L:
		case LD_A_HL:
		case LD_B_A:
		case LD_B_C:
		case LD_B_D:
		case LD_B_E:
		case LD_B_H:
		case LD_B_L:
		case LD_B_HL:
		case LD_C_A:
		case LD_C_B:
		case LD_C_D:
		case LD_C_E:
		case LD_C_H:
		case LD_C_L:
		case LD_C_HL:
		case LD_D_A:
		case LD_D_B:
		case LD_D_C:
		case LD_D_E:
		case LD_D_H:
		case LD_D_L:
		case LD_D_HL:
		case LD_E_A:
		case LD_E_B:
		case LD_E_C:
		case LD_E_D:
		case LD_E_H:
		case LD_E_L:
		case LD_E_HL:
		case LD_H_A:
		case LD_H_B:
		case LD_H_C:
		case LD_H_D:
		case LD_H_E:
		case LD_H_L:
		case LD_H_HL:
		case LD_L_A:
		case LD_L_B:
		case LD_L_C:
		case LD_L_D:
		case LD_L_E:
		case LD_L_H:
		case LD_L_HL:
		case LD_HL_A:
		case LD_HL_B:
		case LD_HL_C:
		case LD_HL_D:
		case LD_HL_E:
		case LD_HL_H:
		case LD_HL_L:
		case LD_BC_A:
		case LD_DE_A:
		case LD_HLP_A:
		case LD_HLM_A:
		case LD_A_HLP:
		case LD_A_HLM:
		case LD_A_BC:
		case LD_A_DE:
		case LD_A_n:
		case LD_B_n:
		case LD_C_n:
		case LD_D_n:
		case LD_E_n:
		case LD_H_n:
		case LD_L_n:
		case LD__HL__n:
		case LD_BC_nn:
		case LD_DE_nn:
		case LD_HL_nn:
		case LD_SP_nn:
		case LD_nn_SP:
		case LD__C__A:
		case LD_A__C_:
		case LD_nn_A:
		case LD_A_nn:
		case LD_HL_SP_n:
		case LD_SP_HL:
		case LDH_A_n:
		case LDH_n_A:
			commands->LD(opcode, param1, param2); break;
		case AND_A:
		case AND_B:
		case AND_C:
		case AND_D:
		case AND_E:
		case AND_H:
		case AND_L:
		case AND_HL:
			commands->AND(opcode); break;
		case AND_n:
			commands->AND(opcode, param1); break;
		case OR_A:
		case OR_B:
		case OR_C:
		case OR_D:
		case OR_E:
		case OR_H:
		case OR_L:
		case OR_HL:
			commands->OR(opcode); break;
		case OR_n:
			commands->OR(opcode, param1); break;
		case ADD_A_A:
		case ADD_A_B:
		case ADD_A_C:
		case ADD_A_D:
		case ADD_A_E:
		case ADD_A_H:
		case ADD_A_L:
		case ADD_A_HL:
		case ADD_HL_BC:
		case ADD_HL_DE:
		case ADD_HL_HL:
		case ADD_HL_SP:
			commands->ADD(opcode); break;
		case ADD_A_n:
		case ADD_SP_n:
			commands->ADD(opcode, param1); break;
		case ADC_A_A:
		case ADC_A_B:
		case ADC_A_C:
		case ADC_A_D:
		case ADC_A_E:
		case ADC_A_H:
		case ADC_A_L:
		case ADC_A_HL:
			commands->ADC(opcode); break;
		case ADC_A_n:
			commands->ADC(opcode, param1); break;
		case SUB_A:
		case SUB_B:
		case SUB_C:
		case SUB_D:
		case SUB_E:
		case SUB_H:
		case SUB_L:
		case SUB_HL:
			commands->SUB(opcode); break;
		case SUB_n:
			commands->SUB(opcode, param1); break;
		case SBC_A_A:
		case SBC_A_B:
		case SBC_A_C:
		case SBC_A_D:
		case SBC_A_E:
		case SBC_A_H:
		case SBC_A_L:
		case SBC_A_HL:
			commands->SBC(opcode); break;
		case SBC_A_n:
			commands->SBC(opcode, param1); break;
		case CP_A:
		case CP_B:
		case CP_C:
		case CP_D:
		case CP_E:
		case CP_H:
		case CP_L:
		case CP_HL:
			commands->CP(opcode); break;
		case CP_n:
			commands->CP(opcode, param1); break;
		case XOR_A:
		case XOR_B:
		case XOR_C:
		case XOR_D:
		case XOR_E:
		case XOR_H:
		case XOR_L:
		case XOR_HL:
			commands->XOR(opcode); break;
		case XOR_n:
			commands->XOR(opcode, param1); break;
		case INC_A:
		case INC_B:
		case INC_C:
		case INC_D:
		case INC_E:
		case INC_H:
		case INC_L:
		case INC__HL_:
		case INC_BC:
		case INC_DE:
		case INC_HL:
		case INC_SP:
			commands->INC(opcode); break;
		case DEC_A:
		case DEC_B:
		case DEC_C:
		case DEC_D:
		case DEC_E:
		case DEC_H:
		case DEC_L:
		case DEC__HL_:
		case DEC_BC:
		case DEC_DE:
		case DEC_HL:
		case DEC_SP:
			commands->DEC(opcode); break;
		case PUSH_AF:
		case PUSH_DE:
		case PUSH_BC:
		case PUSH_HL:
			commands->PUSH(opcode); break;
		case POP_AF:
		case POP_DE:
		case POP_BC:
		case POP_HL:
			commands->POP(opcode); break;
		case JR_NC_n:
		case JR_NZ_n:
		case JR_Z_n:
		case JR_C_n:
		case JR_n:
		case JP_NC_nn:
		case JP_NZ_nn:
		case JP_C_nn:
		case JP_Z_nn:
		case JP_nn:
		case JP_HL:
			commands->JR(opcode, param1, param2, &skipPCInc); break;
		case CALL_NZ_nn:
		case CALL_NC_nn:
		case CALL_C_nn:
		case CALL_Z_nn:
		case CALL_nn:
			skipPCInc = 1;
			commands->CALL(opcode, param1, param2); break;
		case RET_NC:
		case RET_NZ:
		case RET_Z:
		case RET_C:
		case RET:
			commands->RET_(opcode, &skipPCInc); break;
		case RETI:
			commands->RET_(opcode, &skipPCInc); IME = 1; break;
		case RST_00H:
		case RST_10H:
		case RST_20H:
		case RST_30H:
		case RST_08H:
		case RST_18H:
		case RST_28H:
		case RST_38H:
			skipPCInc = 1;
			commands->RST(opcode); break;
		case DAA:
			commands->DAA_(); break;
		case RRCA:
			commands->RRCA_(); break;
		case RRA_:
			commands->RRA(); break;
		case RLCA:
			commands->RLCA_(); break;
		case RLA:
			commands->RLA_(); break;
		case SCF:
			memory->setFlag(C); memory->resetFlag(N); memory->resetFlag(H); break;
		case CCF:
			if (memory->getFlag(C)) { memory->resetFlag(C); }
			else { memory->setFlag(C); }
			memory->resetFlag(N); memory->resetFlag(H); break;
		case CPL:
			registers->AF.a ^= 0xff; memory->setFlag(N); memory->setFlag(H); break;
		case HALT:
			if(!IME) {
				isHalting = true;
				if(memory->get(IE) & memory->get(IF)) {
					doHaltBug = true;
				}
			}
			break; 
		case STOP:
			Stopped = 1; break;
		case DI:
			WillDisableInterrupts = 1; break;
		case EI:
			WillEnableInterrupts = 1; break;
		default:
			break;
			//printf("instruction [%x] [%x] [%x] not implemented\n", opcode, param1, param2);
		}
		if (skipPCInc) { skipPCInc = 0; }
		else {
			registers->PC += (1 + GetParameters(opcode, &p1, &p2)); 
		}
	}
	void CPU::RunCBInstruction(uint8_t opcode) {
		InstructionStats[opcode + 0xff]++;

		switch (opcode) {
		case SWAP_A:
		case SWAP_B:
		case SWAP_C:
		case SWAP_D:
		case SWAP_E:
		case SWAP_H:
		case SWAP_L:
		case SWAP_HL:
			commands->SWAP(opcode); break;
		case BIT0_B:
		case BIT0_C:
		case BIT0_D:
		case BIT0_E:
		case BIT0_H:
		case BIT0_L:
		case BIT0_HL:
		case BIT0_A:
			commands->BIT(opcode, 0); break;
		case BIT1_B:
		case BIT1_C:
		case BIT1_D:
		case BIT1_E:
		case BIT1_H:
		case BIT1_L:
		case BIT1_HL:
		case BIT1_A:
			commands->BIT(opcode, 1); break;
		case BIT2_B:
		case BIT2_C:
		case BIT2_D:
		case BIT2_E:
		case BIT2_H:
		case BIT2_L:
		case BIT2_HL:
		case BIT2_A:
			commands->BIT(opcode, 2); break;
		case BIT3_B:
		case BIT3_C:
		case BIT3_D:
		case BIT3_E:
		case BIT3_H:
		case BIT3_L:
		case BIT3_HL:
		case BIT3_A:
			commands->BIT(opcode, 3); break;
		case BIT4_B:
		case BIT4_C:
		case BIT4_D:
		case BIT4_E:
		case BIT4_H:
		case BIT4_L:
		case BIT4_HL:
		case BIT4_A:
			commands->BIT(opcode, 4); break;
		case BIT5_B:
		case BIT5_C:
		case BIT5_D:
		case BIT5_E:
		case BIT5_H:
		case BIT5_L:
		case BIT5_HL:
		case BIT5_A:
			commands->BIT(opcode, 5); break;
		case BIT6_B:
		case BIT6_C:
		case BIT6_D:
		case BIT6_E:
		case BIT6_H:
		case BIT6_L:
		case BIT6_HL:
		case BIT6_A:
			commands->BIT(opcode, 6); break;
		case BIT7_B:
		case BIT7_C:
		case BIT7_D:
		case BIT7_E:
		case BIT7_H:
		case BIT7_L:
		case BIT7_HL:
		case BIT7_A:
			commands->BIT(opcode, 7); break;
		case RES0_B:
		case RES0_C:
		case RES0_D:
		case RES0_E:
		case RES0_H:
		case RES0_L:
		case RES0_HL:
		case RES0_A:
			commands->RES(opcode, 0); break;
		case RES1_B:
		case RES1_C:
		case RES1_D:
		case RES1_E:
		case RES1_H:
		case RES1_L:
		case RES1_HL:
		case RES1_A:
			commands->RES(opcode, 1); break;
		case RES2_B:
		case RES2_C:
		case RES2_D:
		case RES2_E:
		case RES2_H:
		case RES2_L:
		case RES2_HL:
		case RES2_A:
			commands->RES(opcode, 2); break;
		case RES3_B:
		case RES3_C:
		case RES3_D:
		case RES3_E:
		case RES3_H:
		case RES3_L:
		case RES3_HL:
		case RES3_A:
			commands->RES(opcode, 3); break;
		case RES4_B:
		case RES4_C:
		case RES4_D:
		case RES4_E:
		case RES4_H:
		case RES4_L:
		case RES4_HL:
		case RES4_A:
			commands->RES(opcode, 4); break;
		case RES5_B:
		case RES5_C:
		case RES5_D:
		case RES5_E:
		case RES5_H:
		case RES5_L:
		case RES5_HL:
		case RES5_A:
			commands->RES(opcode, 5); break;
		case RES6_B:
		case RES6_C:
		case RES6_D:
		case RES6_E:
		case RES6_H:
		case RES6_L:
		case RES6_HL:
		case RES6_A:
			commands->RES(opcode, 6); break;
		case RES7_B:
		case RES7_C:
		case RES7_D:
		case RES7_E:
		case RES7_H:
		case RES7_L:
		case RES7_HL:
		case RES7_A:
			commands->RES(opcode, 7); break;
		case SET0_B:
		case SET0_C:
		case SET0_D:
		case SET0_E:
		case SET0_H:
		case SET0_L:
		case SET0_HL:
		case SET0_A:
			commands->SET(opcode, 0); break;
		case SET1_B:
		case SET1_C:
		case SET1_D:
		case SET1_E:
		case SET1_H:
		case SET1_L:
		case SET1_HL:
		case SET1_A:
			commands->SET(opcode, 1); break;
		case SET2_B:
		case SET2_C:
		case SET2_D:
		case SET2_E:
		case SET2_H:
		case SET2_L:
		case SET2_HL:
		case SET2_A:
			commands->SET(opcode, 2); break;
		case SET3_B:
		case SET3_C:
		case SET3_D:
		case SET3_E:
		case SET3_H:
		case SET3_L:
		case SET3_HL:
		case SET3_A:
			commands->SET(opcode, 3); break;
		case SET4_B:
		case SET4_C:
		case SET4_D:
		case SET4_E:
		case SET4_H:
		case SET4_L:
		case SET4_HL:
		case SET4_A:
			commands->SET(opcode, 4); break;
		case SET5_B:
		case SET5_C:
		case SET5_D:
		case SET5_E:
		case SET5_H:
		case SET5_L:
		case SET5_HL:
		case SET5_A:
			commands->SET(opcode, 5); break;
		case SET6_B:
		case SET6_C:
		case SET6_D:
		case SET6_E:
		case SET6_H:
		case SET6_L:
		case SET6_HL:
		case SET6_A:
			commands->SET(opcode, 6); break;
		case SET7_B:
		case SET7_C:
		case SET7_D:
		case SET7_E:
		case SET7_H:
		case SET7_L:
		case SET7_HL:
		case SET7_A:
			commands->SET(opcode, 7); break;
		case RLC_B:
		case RLC_C:
		case RLC_D:
		case RLC_E:
		case RLC_H:
		case RLC_L:
		case RLC_HL:
		case RLC_A:
			commands->RLC(opcode); break;
		case RRC_B:
		case RRC_C:
		case RRC_D:
		case RRC_E:
		case RRC_H:
		case RRC_L:
		case RRC_HL:
		case RRC_A:
			commands->RRC(opcode); break;
		case RL_A:
		case RL_B:
		case RL_C:
		case RL_D:
		case RL_E:
		case RL_H:
		case RL_L:
		case RL_HL:
			commands->RL(opcode); break;
		case RR_A:
		case RR_B:
		case RR_C:
		case RR_D:
		case RR_E:
		case RR_H:
		case RR_L:
		case RR_HL:
			commands->RR(opcode); break;
		case SRA_A:
		case SRA_B:
		case SRA_C:
		case SRA_D:
		case SRA_E:
		case SRA_L:
		case SRA_H:
		case SRA_HL:
			commands->SRA(opcode); break;
		case SLA_A:
		case SLA_B:
		case SLA_C:
		case SLA_D:
		case SLA_E:
		case SLA_L:
		case SLA_H:
		case SLA_HL:
			commands->SLA(opcode); break;
		case SRL_A:
		case SRL_B:
		case SRL_C:
		case SRL_D:
		case SRL_E:
		case SRL_L:
		case SRL_H:
		case SRL_HL:
			commands->SRL(opcode); break;
	}
}
uint8_t CPU::GetNextInstruction() {
	return memory->ReadMem(registers->PC);
	/*return memory->GetValueAt(registers->PC);*/
}

 
// Figure out how many paramters the given instruction requires
short CPU::GetParameters(uint8_t opcode, uint8_t* param1, uint8_t* param2) {
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
			//printf("param1=%02x, param2=%02x, PC=%04x, PC+1=%04x, PC+2=%04x\n", *param1, *param2, PC, PC+1, PC+2);
			numParams = 2; break;

	}

	return numParams;
}
// Figure out how many CPU cycles the given instruction requires
uint8_t CPU::GetCycles(uint8_t opcode) {
	uint8_t cycles = 4;

	switch (opcode) {
		case RST_00H:
		case RST_08H:
		case RST_10H:
		case RST_18H:
		case RST_20H:
		case RST_28H:
		case RST_30H:
		case RST_38H:
			cycles = 32; break;
		case CALL_nn:
			cycles = 24; break;
		case LD_nn_SP:
			cycles = 20; break;
		case RET:
		case RETI:
		case PUSH_AF:
		case PUSH_BC:
		case PUSH_DE:
		case PUSH_HL:
		case JP_nn:
		case ADD_SP_n:
		case LD_nn_A:
		case LD_A_nn:
			cycles = 16; break;
		case LD_BC_nn:
		case LD_DE_nn:
		case LD_HL_nn:
		case LD_SP_nn:
		case JR_n:
		case LDH_A_n:
		case LDH_n_A:
		case POP_AF:
		case POP_BC:
		case POP_DE:
		case POP_HL:
		case LD_HL_SP_n:
		case LD__HL__n:
		case CB:
			cycles = 12; break;
		case LD_BC_A:
		case LD_DE_A:
		case LD_HLM_A:
		case LD_HLP_A:
		case LD_A_BC:
		case LD_A_DE:
		case LD_A_HLP:
		case LD_A_HLM:
		case INC_BC:
		case INC_DE:
		case INC_HL:
		case INC_SP:
		case DEC_BC:
		case DEC_DE:
		case DEC_HL:
		case DEC_SP:
		case LD_A_n:
		case LD_B_n:
		case LD_C_n:
		case LD_D_n:
		case LD_E_n:
		case LD_H_n:
		case LD_L_n:
		case LD_A_HL:
		case LD_B_HL:
		case LD_C_HL:
		case LD_D_HL:
		case LD_E_HL:
		case LD_H_HL:
		case LD_L_HL:
		case LD_HL_A:
		case LD_HL_B:
		case LD_HL_C:
		case LD_HL_D:
		case LD_HL_E:
		case LD_HL_H:
		case LD_HL_L:
		case LD__C__A:
		case LD_A__C_:
		case LD_SP_HL:
		case ADD_HL_BC:
		case ADD_HL_DE:
		case ADD_HL_HL:
		case ADD_HL_SP:
		case ADD_A_n:
		case ADD_A_HL:
		case ADC_A_n:
		case ADC_A_HL:
		case SUB_n:
		case SUB_HL:
		case SBC_A_n:
		case SBC_A_HL:
		case AND_n:
		case AND_HL:
		case OR_n:
		case OR_HL:
		case XOR_n:
		case XOR_HL:
		case CP_n:
		case CP_HL:
			cycles = 8; break;
		case JR_Z_n:
			if (memory->getFlag(Z)) { cycles = 12; }
			else { cycles = 8; }; break;
		case JR_C_n:
			if (memory->getFlag(C)) { cycles = 12; }
			else { cycles = 8; }; break;
		case JR_NZ_n:
			if (!memory->getFlag(Z)) { cycles = 12; }
			else { cycles = 8; }; break;
		case JR_NC_n:
			if (!memory->getFlag(C)) { cycles = 12; }
			else { cycles = 8; }; break;
		case RET_Z:
			if (memory->getFlag(Z)) { cycles = 20; }
			else { cycles = 8; }; break;
		case RET_C:
			if (memory->getFlag(C)) { cycles = 20; }
			else { cycles = 8; }; break;
		case RET_NZ:
			if (!memory->getFlag(Z)) { cycles = 20; }
			else { cycles = 8; }; break;
		case RET_NC:
			if (!memory->getFlag(C)) { cycles = 20; }
			else { cycles = 8; }; break;
		case CALL_C_nn:
			if (memory->getFlag(C)) { cycles = 24; }
			else { cycles = 12; }; break;
		case CALL_Z_nn:
			if (memory->getFlag(Z)) { cycles = 24; }
			else { cycles = 12; }; break;
		case CALL_NC_nn:
			if (!memory->getFlag(C)) { cycles = 24; }
			else { cycles = 12; }; break;
		case CALL_NZ_nn:
			if (!memory->getFlag(Z)) { cycles = 24; }
			else { cycles = 12; }; break;
		default:
			cycles = 4; break;

	}

	return cycles;
}

// LCDC
void CPU::UpdateGraphics(uint8_t opcode) {

	uint8_t cycles;

	if (IsLCDEnabled()) {
		cycles = GetCycles(opcode) * 2;
		registers->sCounter -= cycles;
	}
	else return;

	//printf("sCounter=%04x\n", sCounter);
	if (registers->sCounter <= 0) { 
		memory->increment(LY);
		uint8_t cLine = memory->get(LY);

		registers->sCounter = 0x1c8;

		if (cLine == 0x90) {
			SetInterrupt(I_VBlank);
		}
		else if (cLine > 0x99) {
			memory->set(LY, 0);
		}
		else if (cLine == 143) {
			//printf("cLine=%02x, callRefresh();\n", cLine);
			//draw->callRefresh();
		}

		CheckLYCAndSetInterruptIfValid();
	}
	//callRefresh();
}

void CPU::SetLCDStatus() {
	uint8_t status = memory->get(STAT);

	if (!IsLCDEnabled()) {
		registers->sCounter = 0x1c8;
		memory->set(LY, 0);
		status &= 0xfc; // Clear the lower 2 bits
		memory->set(STAT, (status | 0x01)); // Set the lower 2 bits to '01'
		return;
	}

	uint8_t cLine = memory->get(LY);
	uint8_t cMode = status & 0x03; // Get lower 2 bits

	uint8_t mode = 0;
	uint8_t reqInt = 0;

	if (cLine >= 0x90) {
		mode = 1;
		status &= 0xfc; // Clear the lower 2 bits
		status |= 0x01; // Set lower 2 bits to '01'
		reqInt = (status & BIT4);
	}
	else {
		uint8_t mode2bounds = (uint8_t)(0x1c8 - 80);
		uint8_t mode3bounds = mode2bounds - 172;

		// mode 2
		if (registers->sCounter >= mode2bounds) {
			mode = 2;
			status &= 0xfc; // Clear the lower 2 bits
			status |= 0x02; // Set lower 2 bits to '10'
			reqInt = (status & BIT5);
			// mode 3
		}
		else if (registers->sCounter >= mode3bounds) {
			mode = 3;
			status |= 0x03; // Set lower 2 bits to '11'		
		// mode 0
		}
		else {
			mode = 0;
			status &= 0xfc; // Clear the lower 2 bits
			reqInt = (status & BIT3);
		}
	}

	// just entered a new mode so request interupt
	if (reqInt && (mode != cMode)) {
		SetInterrupt(I_LCDC);
	}

	// check the conincidence flag
	if (memory->get(LYC) == memory->get(LY)) {
		status |= 0x04;
		if (status & BIT6) {
			SetInterrupt(I_LCDC);
		}
	}
	else {
		status &= 0xfb;
	}

	memory->set(STAT, status);
}
void CPU::CheckLYCAndSetInterruptIfValid() {
	uint8_t status = memory->get(STAT);
	if (memory->get(LYC) == memory->get(LY)) { 
		if (status & BIT6) {
			SetInterrupt(I_LCDC);
		}
	}
}
uint8_t CPU::IsLCDEnabled() {
	//printf("LCDC=%02x, enabled=%s\n", Memory[LCDC], (Memory[LCDC] & BIT7 ? "true" : "false"));
	return (memory->get(LCDC) & BIT7);
}

void CPU::DisplayCartridgeInfo() {
	rominfo->DisplayCartridgeInfo();
}


Memory* CPU::GetMemory() {
	return memory;
}
Registers* CPU::GetRegisters() {
	return registers;
}

string CPU::ToHexString(uint8_t val) {
	std::stringstream st;
	if (val == 0)
		st << hex << setw(2) << "00";
	else
		st << hex << setw(2) << setfill('0') << (uint16_t)val;
	return st.str();

}
string CPU::ToHexString(uint16_t val) {
	stringstream st;
	if (val == 0)
		st << hex << setw(4) << "0000";
	else
		st << hex << setw(4) << setfill('0') << val;
	return st.str();
}


void CPU::InputProcess(uint8_t type) {
	std::lock_guard<mutex> locker(cpu_mutex);

	switch (type) {
		case A_BUTTON_DOWN:
			joypadState->ButtonDown(joypadState->A_BUTTON); break;
		case INPUT_RIGHT_DOWN:
			joypadState->ButtonDown(joypadState->RIGHT_BUTTON); break;
		case A_BUTTON_UP:
			joypadState->ButtonUp(joypadState->A_BUTTON); break;
		case INPUT_RIGHT_UP:
			joypadState->ButtonUp(joypadState->RIGHT_BUTTON); break;
		case B_BUTTON_DOWN:
			joypadState->ButtonDown(joypadState->B_BUTTON); break;
		case INPUT_LEFT_DOWN:
			joypadState->ButtonDown(joypadState->LEFT_BUTTON); break;
		case B_BUTTON_UP:
			joypadState->ButtonUp(joypadState->B_BUTTON); break;
		case INPUT_LEFT_UP:
			joypadState->ButtonUp(joypadState->LEFT_BUTTON); break;
		case SELECT_BUTTON_DOWN:
			joypadState->ButtonDown(joypadState->SELECT_BUTTON); break;
		case INPUT_UP_DOWN:
			joypadState->ButtonDown(joypadState->UP_BUTTON); break;
		case SELECT_BUTTON_UP:
			joypadState->ButtonUp(joypadState->SELECT_BUTTON); break;
		case INPUT_UP_UP:
			joypadState->ButtonUp(joypadState->UP_BUTTON); break;
		case START_BUTTON_DOWN:
			joypadState->ButtonDown(joypadState->START_BUTTON); break;
		case INPUT_DOWN_DOWN:
			joypadState->ButtonDown(joypadState->DOWN_BUTTON); break;
		case START_BUTTON_UP:
			joypadState->ButtonUp(joypadState->START_BUTTON); break;
		case INPUT_DOWN_UP:
			joypadState->ButtonUp(joypadState->DOWN_BUTTON); break;
	}
}
void CPU::GetState(uint8_t *state) {
	uint32_t index = 0;
	uint32_t index2 = REGISTERS_STATE_SIZE + TIMER_STATE_SIZE;
	*(state+index++) = registers->AF.a; 
	*(state+index++) = registers->AF.f;
	*(state+index++) = registers->BC.b;
	*(state+index++) = registers->BC.c;
	*(state+index++) = registers->DE.d;
	*(state+index++) = registers->DE.e;
	*(state+index++) = registers->HL.h;
	*(state+index++) = registers->HL.l;
	*(state+index++) = registers->SP & 0xff;
	*(state+index++) = (registers->SP >> 8) & 0xff;
	*(state+index++) = registers->PC & 0xff;
	*(state+index++) = (registers->PC >> 8) & 0xff;
	*(state+index++) = registers->rDiv & 0xff;
	*(state+index++) = (registers->rDiv >> 8) & 0xff;
	*(state+index++) = registers->sCounter & 0xff;
	*(state+index++) = (registers->sCounter >> 8) & 0xff;
	*(state+index++) = (registers->sCounter >> 16) & 0xff;
	*(state+index++) = (registers->sCounter >> 24) & 0xff;

	timer->GetState(state, REGISTERS_STATE_SIZE);
	memory->GetState(state, &index2);
}

void CPU::SetState(uint8_t *state) {
	uint32_t index = 0;
	uint32_t index2 = REGISTERS_STATE_SIZE + TIMER_STATE_SIZE;
	registers->AF.a = *(state+index++);
	registers->AF.f = *(state+index++);
	registers->BC.b = *(state+index++);
	registers->BC.c = *(state+index++);
	registers->DE.d = *(state+index++);
	registers->DE.e = *(state+index++);
	registers->HL.h = *(state+index++);
	registers->HL.l = *(state+index++);
	registers->SP = *(state+index) + (*(state+index+1) << 8); index += 2;
	registers->PC = *(state+index) + (*(state+index+1) << 8); index += 2;
	registers->rDiv = *(state+index) + (*(state+index+1) << 8); index += 2;
	registers->sCounter = *(state+index) + (*(state+index+1) << 8) + (*(state+index+2) << 16) + (*(state+index+3) << 24);

	timer->SetState(state, REGISTERS_STATE_SIZE);
	memory->SetState(state, &index2);
}

#ifdef LOG_COMMANDS
void CPU::LogCommand(uint16_t* pc, uint8_t* inst, uint8_t* param1, uint8_t* param2, short* params,
		uint8_t* ime, uint8_t* _if, uint8_t* ie) {

	// Write the spacer and the PC
	fwrite(pc, sizeof(uint16_t), sizeof(uint8_t), clog);

	switch (*params) {
		case (0):
			fwrite(inst, sizeof(uint8_t), sizeof(uint8_t), clog);
			break;
		case (1):
			fwrite(inst, sizeof(uint8_t), sizeof(uint8_t), clog);
			fwrite(param1, sizeof(uint8_t), sizeof(uint8_t), clog);
			break;
		case (2):
			fwrite(inst, sizeof(uint8_t), sizeof(uint8_t), clog);
			fwrite(param1, sizeof(uint8_t), sizeof(uint8_t), clog);
			fwrite(param2, sizeof(uint8_t), sizeof(uint8_t), clog);
			break;
	}

	// Write out state
	fwrite(&registers->AF.af, sizeof(uint16_t), sizeof(uint8_t), clog);
	fwrite(&registers->BC.bc, sizeof(uint16_t), sizeof(uint8_t), clog);
	fwrite(&registers->DE.de, sizeof(uint16_t), sizeof(uint8_t), clog);
	fwrite(&registers->HL.hl, sizeof(uint16_t), sizeof(uint8_t), clog);
	fwrite(&registers->SP, sizeof(uint16_t), sizeof(uint8_t), clog);
	fwrite(ime, sizeof(uint8_t), sizeof(uint8_t), clog);
	fwrite(ie, sizeof(uint8_t), sizeof(uint8_t), clog);
	fwrite(_if, sizeof(uint8_t), sizeof(uint8_t), clog);

	//fwrite(command, sizeof(char), sizeof(char) * strlen(command), clog);
}
#endif
#ifdef LOG_STATS
void CPU::LogStats() {
	stringstream st;

	for (int i = 0; i < 512; i++) {
		st << hex << setw(4) << setfill('0') << i << ", " << dec << InstructionStats[i] << endl;
	}
	string res = st.str();
	const char* c = res.c_str();
	fwrite(c, sizeof(char), strlen(c), statlog);
}
#endif