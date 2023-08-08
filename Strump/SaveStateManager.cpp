#include "SaveStateManager.h"

SaveStateManager::SaveStateManager() {

}

void SaveStateManager::LoadState(string *saveFileName, CPU *cpu) {
    cpu->Pause();

	Registers *registers = cpu->GetRegisters();
    Memory *memory = cpu->GetMemory();
    Timer *timer = cpu->GetTimer();

	// uint32_t size = RAM_SIZE + RAM_BANK_SIZE + (VRAM_BANK_SIZE * 2) + PALETTE_SIZE + TIMER_STATE_SIZE + REGISTERS_STATE_SIZE + RAM_STATE_SIZE;
	uint32_t size = memory->GetMemorySize() + RAM_SIZE_v0 + RAM_BANK_SIZE_v0 + (VRAM_BANK_SIZE_v0 * 2) + (WRAM_BANK_SIZE_v0 * 8) +
        (PALETTE_SIZE_v0 * 2) + TIMER_STATE_SIZE_v0 + REGISTERS_STATE_SIZE_v0 + RAM_STATE_SIZE_v0;
	uint32_t index = 0;
	uint32_t *pIndex = &index;

	uint8_t* gameState = (uint8_t*)malloc(sizeof(uint8_t) * size);
	for(int i=0; i<size; i++) {
		*(gameState + i) = 0;
	}

	// Try to open ROM file
	cout << "save file: " << saveFileName << endl;
	FILE *sfp = fopen((*saveFileName).c_str(), "rb");
	if (sfp == NULL) {
		cout << "Couldn't open save file \n" << saveFileName << endl;
	} else {
		fread(gameState, sizeof(uint8_t), sizeof(uint8_t) * size, sfp);
	}
	fclose(sfp);

	// int8_t version = gameState[0];
	
	// cpu->SetState(gameState);
    
	uint32_t index2 = REGISTERS_STATE_SIZE_v0 + TIMER_STATE_SIZE_v0;
	registers->AF.a = *(gameState+index++);
	registers->AF.f = *(gameState+index++);
	registers->BC.b = *(gameState+index++);
	registers->BC.c = *(gameState+index++);
	registers->DE.d = *(gameState+index++);
	registers->DE.e = *(gameState+index++);
	registers->HL.h = *(gameState+index++);
	registers->HL.l = *(gameState+index++);
	registers->SP = *(gameState+index) + (*(gameState+index+1) << 8); index += 2;
	registers->PC = *(gameState+index) + (*(gameState+index+1) << 8); index += 2;
	registers->rDiv = *(gameState+index) + (*(gameState+index+1) << 8); index += 2;
	registers->sCounter = *(gameState+index) + (*(gameState+index+1) << 8) + (*(gameState+index+2) << 16) + (*(gameState+index+3) << 24);

	timer->SetState(gameState, 18);
    *timer->tac = *(gameState+index++);
	*timer->tima = *(gameState+index++);
	*timer->tma = *(gameState+index++);
	*timer->div = *(gameState+index++);
	for(int i=0; i<4; i++) {
		timer->timerCounter |= (*(gameState + index + i) >> (i * 8));
		timer->divCounter |= (*(gameState + 4 + index + i) >> (i * 8));
	}

	memory->SetState(gameState, &index2);

    uint32_t val = index2;
	for(int i=0; i<RAM_SIZE_v0; i++) {
		memory->memory[i] = *(gameState+val+i);
	}
	val += RAM_SIZE_v0;
	for(int i=0; i<RAM_BANK_SIZE_v0; i++) {
		memory->RamBankData[i] = *(gameState+val+i);
	}
	val += RAM_BANK_SIZE_v0;
	for(int i=0; i<2; i++) {
		for(int j=0; j<VRAM_BANK_SIZE_v0; j++) {
			memory->VRamBankData[i][j] = *(gameState+val+j);
		}
		val += VRAM_BANK_SIZE_v0;
	}
	for(int i=0; i<8; i++) {
		for(int j=0; j<WRAM_BANK_SIZE_v0; j++) {
			memory->WRamBankData[i][j] = *(gameState+val+j);
		}
		val += WRAM_BANK_SIZE_v0;
	}
	for(int i=0; i<PALETTE_SIZE_v0; i++) {
		memory->PaletteData[i] = *(gameState+val+i);
	}
	val += PALETTE_SIZE_v0;
	memory->RamEnabled = *(gameState+val++);
	memory->RomBanking = *(gameState+val++);
	memory->RomBank = *(gameState+val++);
	memory->RamBank = *(gameState+val++);
	memory->VramBank = *(gameState+val++);
	memory->WRamBank = *(gameState+val++);

    memory->SetState(gameState, &val); // TODO: For the differenct MBCs figure out a way to do it here
    
    free(gameState);

	cpu->Unpause();
}

void SaveStateManager::SaveState(string *saveFileName, CPU *cpu) {

}