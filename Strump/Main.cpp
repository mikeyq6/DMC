#include <iostream>
#include <conio.h>
#include <thread>  
#include "CPU.h"
#include "Emulator.h"

using namespace std;

void CPUThreadMain(CPU* cpu);

int main(int argc, char* argv[]) {
	if (argc < 2) {
		cout << "Usage: %s [gameboy rom]\n" << argv[0] << endl;
		return 1;
	}

	Emulator* emulator = new Emulator(argv[1]);
	if (!emulator->Init())
		return 2;

	emulator->Start();
	emulator->End();


	return 0;
}

void CPUThreadMain(CPU* cpu) {
	cpu->Start();
}