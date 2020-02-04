#include "SDL.h"
#include <iostream>


int main(int argc, char** argv) {

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		std::cout << "Initialised!";
	}

	return 0;
}