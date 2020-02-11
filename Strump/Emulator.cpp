#include "Emulator.h"
#include <conio.h>

Emulator::Emulator(const char* _cartridgeFileName) {
	cartridgeFileName = _cartridgeFileName;

	cpu = new CPU();
}
Emulator::~Emulator() {
	delete cpu;
}

void Emulator::End() {
	//cpu->Quit();
}

bool Emulator::Init() {
	// Try to open ROM file
	int err = fopen_s(&fp, cartridgeFileName, "r");
	if (err > 0) {
		cout << "Coulnd't open file '%s'\n" << cartridgeFileName << endl;
		return false;
	}

	isRunning = true;

	return true;
}
void Emulator::Start() {

	uint16_t size = (uint16_t)fread(cpu->GetRomInfo()->cartridge, sizeof(uint8_t), CARTRIDGE_SIZE, fp);
	cpu->initCPU();
	cpu->DisplayCartridgeInfo();

	auto run = [](CPU *cpu) {
		cpu->Start();
	};
	std::thread cpu_thread(run, cpu);
	cpu_thread.detach();

	Draw* draw;
	draw = new Draw(cpu->GetMemory(), cpu->GetRegisters());
	draw->drawInit("Illuminati", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, S_WIDTH, S_HEIGHT, false, false, true, true);

	while (running()) {
		handleEvents();
		update();
		draw->render(cpu->IsStopped());
	}
	draw->clean();

	cpu->Stop();


	//printf("\nAny key to quit");
	//_getch();
}

void Emulator::handleEvents() {
	SDL_Event event;
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
			default:
				break;
		}
	}
}

void Emulator::processKeyEvent(SDL_Event* event) {
	int alter = event->type == SDL_KEYUP ? 8 : 0;

	switch (event->key.keysym.sym) {
		case SDLK_LEFT:
			cpu->InputProcess(INPUT_LEFT_DOWN + alter);
			break;
		case SDLK_RIGHT:
			cpu->InputProcess(INPUT_RIGHT_DOWN + alter);
			break;
		case SDLK_UP:
			cpu->InputProcess(INPUT_UP_DOWN + alter);
			break;
		case SDLK_DOWN:
			cpu->InputProcess(INPUT_DOWN_DOWN + alter);
			break;
		case SDLK_a:
			cpu->InputProcess(A_BUTTON_DOWN + alter);
			break;
		case SDLK_b:
			cpu->InputProcess(B_BUTTON_DOWN + alter);
			break;
		case SDLK_1:
			cpu->InputProcess(SELECT_BUTTON_DOWN + alter);
			break;
		case SDLK_2:
			cpu->InputProcess(START_BUTTON_DOWN + alter);
			break;
		case SDLK_r:
			SDL_Log("Restart");  // doesn't work
			//cpu->Stop();
			cpu->initCPU();
			//u->Start();
			break;
		case SDLK_q:
			SDL_Log("Quit");
			isRunning = false;
			break;
		case SDLK_s: // Start/end step mode
			SDL_Log("Step mode");
			if (cpu->StepModeActive())
				cpu->DeactivateStepMode();
			else
				cpu->ActivateStepMode();
			break;
		case SDLK_SPACE: // Step
			if (cpu->StepModeActive())
				cpu->NextStep();

		default:
			break;
	}
}

void Emulator::update() {
	//
}