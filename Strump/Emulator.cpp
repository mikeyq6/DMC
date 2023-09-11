#include "Emulator.h"

Emulator::Emulator(const char* _cartridgeFileName) {
	string s(_cartridgeFileName);
	cartridgeFileName = s;
	draw = NULL;

	#ifdef REAL_TIME_CPU
	bool useRealTimeCPU = true;
	#else
	bool useRealTimeCPU = false;
	#endif

	cpu = new CPU(useRealTimeCPU);

	#ifdef SHOW_OAM_MAP
	showOAMMap = true;
	#else
	showOAMMap = false;
	#endif

	#ifdef SHOW_TILE_MAP
	showTileMap = true;
	#else
	showTileMap = false;
	#endif

	#ifdef SHOW_PALETTES
	showPalettes = true;
	#else
	showPalettes = false;
	#endif

	#ifdef SHOW_FULL_BACKGROUND
	showFullBackground = true;
	#else
	showFullBackground = false;
	#endif
	
}
Emulator::~Emulator() {
	delete draw;
	delete cpu;
}

void Emulator::End() {
	//cpu->Quit();
}

bool Emulator::Init() {
	// Try to open ROM file
	fp = fopen(cartridgeFileName.c_str(), "r");
	if (fp == NULL) {
		cout << "Couldn't open file \n" << cartridgeFileName << endl;
		return false;
	}

	saveFileName = cartridgeFileName + ".sav";
	setWindowTitle();

	isRunning = true;
	return isRunning;
}
void Emulator::Start() {
	uint32_t size = (uint32_t)fread(cpu->GetRomInfo()->cartridge, sizeof(uint8_t), (size_t)CARTRIDGE_SIZE, fp);
	cpu->initCPU();
	cpu->DisplayCartridgeInfo();

	auto run = [](CPU *cpu) {
		cpu->Start();
	};
	std::thread cpu_thread(run, cpu);
	cpu_thread.detach();

	DrawFactory *drawFactory = new DrawFactory();

	draw = drawFactory->GetDrawByType(cpu->GetMemory(), cpu->GetRegisters(), cpu->GetRomInfo()->CartInfo);
	delete drawFactory;

	draw->drawInit(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, S_WIDTH, S_HEIGHT, 
		false, false, showFullBackground, showTileMap, showPalettes, showOAMMap);

	while (running()) {
		handleEvents();
		update();
		// if(cpu->GetMemory()->getStatMode() != 0b01) {
			draw->render(cpu->IsStopped());
		// }
	}
	draw->clean();
	cpu->Stop();
}

void Emulator::setWindowTitle() {
	windowTitle = cartridgeFileName;

	// Remove any path characters
	int pos;	
	while ((pos = windowTitle.find("/")) != string::npos) {
		windowTitle.erase(0, pos + 1);
	}
	if((pos = windowTitle.find(".")) != string::npos) {
		windowTitle.erase(pos, windowTitle.length() - pos);
	}
}

void Emulator::handleEvents() {
	SDL_Event event;
	bool resizeDone = false;
	while (SDL_PollEvent(&event)) {
		switch (event.type)
		{
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				processKeyEvent(&event);
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (cpu->IsPaused())
					cpu->Unpause();
				else
					cpu->Pause();
				break;
			case SDL_WINDOWEVENT:  {
				if(resizeDone) {
					resizeDone = false;
				} else {
					cpu->Pause();
					draw->HandleWindowResizeEvent(event);
					resizeDone = true;
					cpu->Unpause();
				}
				break;
			default:
				break;
        }

		}
	}
}

void Emulator::processKeyEvent(SDL_Event* event) {
	int alter = event->type == SDL_KEYUP ? 8 : 0;
	uint16_t mod = SDL_GetModState(); 

	if (event->type == SDL_KEYUP) {
		switch (event->key.keysym.sym) {
			case SDLK_LEFT:
				cpu->InputProcess(INPUT_LEFT_UP);
				break;
			case SDLK_RIGHT:
				cpu->InputProcess(INPUT_RIGHT_UP);
				break;
			case SDLK_UP:
				cpu->InputProcess(INPUT_UP_UP);
				break;
			case SDLK_DOWN:
				cpu->InputProcess(INPUT_DOWN_UP);
				break;
			case SDLK_a:
			case SDLK_z:
				cpu->InputProcess(A_BUTTON_UP);
				break;
			case SDLK_b:
			case SDLK_x:
				cpu->InputProcess(B_BUTTON_UP);
				break;
			case SDLK_1:
				cpu->InputProcess(SELECT_BUTTON_UP);
				break;
			case SDLK_2:
				cpu->InputProcess(START_BUTTON_UP);
				break;
		}
	}
	else if (event->type == SDL_KEYDOWN) {
		switch (event->key.keysym.sym) {
			case SDLK_LEFT:
				cpu->InputProcess(INPUT_LEFT_DOWN);
				break;
			case SDLK_RIGHT:
				cpu->InputProcess(INPUT_RIGHT_DOWN);
				break;
			case SDLK_UP:
				cpu->InputProcess(INPUT_UP_DOWN);
				break;
			case SDLK_DOWN:
				cpu->InputProcess(INPUT_DOWN_DOWN);
				break;
			case SDLK_a:
			case SDLK_z:
				cpu->InputProcess(A_BUTTON_DOWN);
				break;
			case SDLK_b:
			case SDLK_x:
				cpu->InputProcess(B_BUTTON_DOWN);
				break;
			case SDLK_1:
				cpu->InputProcess(SELECT_BUTTON_DOWN);
				break;
			case SDLK_2:
				cpu->InputProcess(START_BUTTON_DOWN);
				break;
			case SDLK_c:
				if(SDL_GetModState() & KMOD_SHIFT) {
					for(int i=0; i<CARTRIDGE_SIZE; i++) {
						printf("%x ", cpu->GetRomInfo()->GetCardridgeVal(i));
						if(i>0 && (i % 0xff) == 0) {
							printf("\n");
						}
					}
				}
				break;
			case SDLK_l: // Load state
				if(SDL_GetModState() & KMOD_SHIFT) {
					loadGameState();
				}
				break;
			case SDLK_m: // edit memory
				if(SDL_GetModState() & KMOD_SHIFT) {
					editMemory();
				} else {
					viewMemory();
				}
				break;
			case SDLK_q:
				SDL_Log("Quit");
				isRunning = false;
				break;
			case SDLK_r:
				// SDL_Log("Restart");  // doesn't work
				//cpu->Stop();
				// cpu->initCPU();
				//u->Start();
				printf("Cartridge ROM bank: %x\n", cpu->memory->GetRomBank());
				break;
			case SDLK_s: // Save state
				if(SDL_GetModState() & KMOD_SHIFT) {
					saveGameState();
				}
				break;
			case SDLK_t: // Toggle colour mode
				draw->ToggleColourMode();
				break;
			case SDLK_o: // Toggle VRAM Location
				draw->ToggleVRAMLocation();
				break;
			case SDLK_g: // View/Edit register
				if(SDL_GetModState() & KMOD_SHIFT) {
					editRegister();
				} else {
					viewRegisterState();
				}
				break;
			case SDLK_p: // Toggle Real Time CPU mode
				cpu->ToggleUseRealTimeCPU();
				break;
			case SDLK_3: // Show/hide full background window
				draw->toggleBackgroundMap();
				break;
			case SDLK_4: // Show/hide tile map window
				draw->toggleTileMap();
				break;
			case SDLK_5: // Show/hide Palette window (BGC only)
				draw->togglePaletteMap();
				break;
			case SDLK_6: // Show/hide OAM window
				draw->toggleOAMMap();
				break;
			default:
				break;
		}
	}
}

void Emulator::update() {
	//
}

void Emulator::saveGameState() {
	cpu->Pause();

	uint32_t size = cpu->GetMemory()->GetMemorySize() + TIMER_STATE_SIZE + REGISTERS_STATE_SIZE + RAM_STATE_SIZE;
	uint32_t index = 0;
	uint32_t *pIndex = &index;

	uint8_t* gameState = (uint8_t*)malloc(sizeof(uint8_t) * size);
	for(int i=0; i<size; i++) {
		*(gameState + i) = 0;
	}
	cpu->GetState(gameState);

	// Try to open ROM file
	cout << "save file: " << saveFileName << endl;
	FILE *sfp = fopen(saveFileName.c_str(), "wb");
	if (sfp == NULL) {
		cout << "Couldn't open save file \n" << saveFileName << endl;
	} else {
		fwrite(gameState, sizeof(uint8_t), sizeof(uint8_t) * size, sfp);
	}
	fclose(sfp);
	free(gameState);

	cpu->Unpause();
}
void Emulator::loadGameState() {
	cpu->Pause();

	// uint32_t size = RAM_SIZE + RAM_BANK_SIZE + (VRAM_BANK_SIZE * 2) + PALETTE_SIZE + TIMER_STATE_SIZE + REGISTERS_STATE_SIZE + RAM_STATE_SIZE;
	uint32_t size = cpu->GetMemory()->GetMemorySize() + PALETTE_SIZE + TIMER_STATE_SIZE + REGISTERS_STATE_SIZE + RAM_STATE_SIZE;
	uint32_t index = 0;
	uint32_t *pIndex = &index;

	uint8_t* gameState = (uint8_t*)malloc(sizeof(uint8_t) * size);
	for(int i=0; i<size; i++) {
		*(gameState + i) = 0;
	}

	cout << "save file: " << saveFileName << endl;
	FILE *sfp = fopen(saveFileName.c_str(), "rb");
	if (sfp == NULL) {
		cout << "Couldn't open save file \n" << saveFileName << endl;
		return;
	} else {
		fread(gameState, sizeof(uint8_t), sizeof(uint8_t) * size, sfp);
	}
	fclose(sfp);

	cpu->SetState(gameState);

	free(gameState);

	cpu->Unpause();
}

void Emulator::viewMemory() {
	cpu->Pause();

	uint16_t location;
	uint8_t value;

	cout << "View memory at location: ";
	cin >> hex >> location;
	value = cpu->GetMemory()->get(location);

	cout << "Value at location " << hex << setw(4) << setfill('0') << location;
	cout << ": ";
	cout << hex << setw(2) << setfill('0') << static_cast<int>(value) << endl;

	cpu->Unpause();
}

void Emulator::editMemory() {
	cpu->Pause();

	uint16_t location;
	int invalue;

	cout << "Edit memory location: ";
	cin >> hex >> location;
	cout << "Set value: ";
	cin >> hex >> invalue;
	uint8_t value = static_cast<uint8_t>(invalue);

	cout << "Will edit: " << hex << setw(4) << setfill('0') << location;
	cout << ", set to: " << hex << setw(2) << setfill('0') << static_cast<int>(value) << endl;
	cpu->GetMemory()->set(location, value);

	cpu->Unpause();
}

void Emulator::viewRegisterState() {
	cpu->Pause();

	uint16_t location;
	uint8_t value;
	Registers *registers = cpu->GetRegisters();

	cout << "Registers: " << endl;
	cout << "PC: " << hex << setw(4) << setfill('0') << registers->PC;
	cout << "  SP: " << hex << setw(4) << setfill('0') << registers->SP << endl;
	cout << "AF: " << hex << setw(4) << setfill('0') << registers->AF.af;
	cout << "  BC: " << hex << setw(4) << setfill('0') << registers->BC.bc << endl;
	cout << "DE: " << hex << setw(4) << setfill('0') << registers->DE.de;
	cout << "  HL: " << hex << setw(4) << setfill('0') << registers->HL.hl;
	cout << endl;

	cpu->Unpause();
}

void Emulator::editRegister() {
	// cpu->Pause();

	// uint16_t location;
	// int invalue;

	// cout << "Enter register to edit: ";
	// cin >> hex >> location;
	// cout << "Set value: ";
	// cin >> hex >> invalue;
	// uint8_t value = static_cast<uint8_t>(invalue);

	// cout << "Will edit: " << hex << setw(4) << setfill('0') << location;
	// cout << ", set to: " << hex << setw(2) << setfill('0') << static_cast<int>(value) << endl;
	// cpu->GetMemory()->set(location, value);

	// cpu->Unpause();
}