CC=g++ -std=c++17
OBJS=Commands.cpp CPU.cpp JoypadState.cpp Draw.cpp Emulator.cpp Log.cpp Memory.cpp MBC1Memory.cpp MBC2Memory.cpp MBC3Memory.cpp MBC5Memory.cpp NoRamNoRomMemory.cpp RamNoRomMemory.cpp MemoryFactory.cpp ROMInfo.cpp Sprite.cpp test.cpp Timer.cpp Main.cpp
#OBJS=Main.cpp
SDL2dir=~/Library/Frameworks/SDL2.framework/Headers

LFLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf
SUPPRESS=-Wno-unused-variable -Wno-unused-private-field

debug:
	${CC} -Wall ${SUPPRESS} -I${SDL2dir} ${LFLAGS} -o strump ${OBJS}

log:
	${CC} -Wall ${SUPPRESS} -DLOG_COMMANDS=1 -I${SDL2dir} ${LFLAGS} -o strump ${OBJS}

release:
	${CC} -Wall ${SUPPRESS} -DREAL_TIME_CPU=1 -I${SDL2dir} ${LFLAGS} -o strump ${OBJS}

logreader:
	${CC} -Wall ${SUPPRESS} -o ../LogReader/LogReader ../LogReader/Main.cpp

clean:
	rm -rf strump *.o commands
	rm -rf ../LogReader/*.o ../LogReader/LogReader
