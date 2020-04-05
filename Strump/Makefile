CC=g++ -std=c++17
OBJS=Commands.cpp CPU.cpp JoypadState.cpp Draw.cpp Emulator.cpp Log.cpp Memory.cpp MBC1Memory.cpp MBC2Memory.cpp MBC3Memory.cpp MBC5Memory.cpp NoRamNoRomMemory.cpp RamNoRomMemory.cpp MemoryFactory.cpp ROMInfo.cpp Sprite.cpp test.cpp Timer.cpp Main.cpp
#OBJS=Main.cpp
SDL2dir=~/Library/Frameworks/SDL2.framework/Headers

LFLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf -v
SUPPRESS=-Wno-unused-variable -Wno-unused-private-field

debug:
	${CC} -Wall ${SUPPRESS} -I${SDL2dir} ${LFLAGS} -o strump ${OBJS}

clean:
	rm -rf strump *.o commands