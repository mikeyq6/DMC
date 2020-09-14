CC=g++ -std=c++17
OBJS=Commands.cpp CPU.cpp JoypadState.cpp Draw.cpp GBDraw.cpp GBCDraw.cpp DrawFactory.cpp Emulator.cpp Log.cpp Memory.cpp MBC1Memory.cpp MBC2Memory.cpp MBC3Memory.cpp MBC5Memory.cpp NoRamNoRomMemory.cpp RamNoRomMemory.cpp MemoryFactory.cpp ROMInfo.cpp Sprite.cpp test.cpp Timer.cpp Main.cpp
LFLAGS=-lSDL2 -lSDL2_image
SUPPRESS=-Wno-unused-variable -Wno-unused-private-field

debug:
	${CC} -Wall ${SUPPRESS} -DRUNTESTS=1 ${LFLAGS} -o strump ${OBJS}

debug-notests:
	${CC} -Wall ${SUPPRESS} ${LFLAGS} -o strump ${OBJS}

log:
	${CC} -Wall ${SUPPRESS} -DLOG_COMMANDS=1 -DRUNTESTS=1 ${LFLAGS} -o strump ${OBJS}

release:
	${CC} -Wall ${SUPPRESS} -DREAL_TIME_CPU=1 ${LFLAGS} -o strump ${OBJS}

logreader:
	${CC} -Wall ${SUPPRESS} -o ../LogReader/LogReader ../LogReader/Main.cpp

clean:
	rm -rf strump *.o commands
	rm -rf ../LogReader/*.o ../LogReader/LogReader
