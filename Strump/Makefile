CC=g++ -std=c++17
OBJS=Commands.cpp CPU.cpp JoypadState.cpp Draw.cpp GBDraw.cpp GBCDraw.cpp DrawFactory.cpp Emulator.cpp Log.cpp Memory.cpp MBC1Memory.cpp MBC2Memory.cpp MBC3Memory.cpp MBC5Memory.cpp NoRamNoRomMemory.cpp RamNoRomMemory.cpp MemoryFactory.cpp ROMInfo.cpp Sprite.cpp test.cpp testMemory.cpp testMBC5.cpp Timer.cpp Main.cpp
LFLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf
SUPPRESS=-Wno-unused-variable -Wno-unused-private-field
EXENAME=strump

debug:
	${CC} -g -Wall ${SUPPRESS} -DRUNTESTS=1 ${LFLAGS} -o ${EXENAME} ${OBJS}

debug-notests:
	${CC} -g -Wall ${SUPPRESS} ${LFLAGS} -o ${EXENAME} ${OBJS}

log:
	${CC} -g -Wall ${SUPPRESS} -DLOG_COMMANDS=1 -DRUNTESTS=1 ${LFLAGS} -o ${EXENAME} ${OBJS}

release:
	${CC} -Wall ${SUPPRESS} -DREAL_TIME_CPU=1 ${LFLAGS} -o ${EXENAME} ${OBJS}

drelease:
	${CC} -g -Wall ${SUPPRESS} -DREAL_TIME_CPU=1 ${LFLAGS} -o ${EXENAME} ${OBJS}

logreader:
	${CC} -Wall ${SUPPRESS} -o ../LogReader/LogReader ../LogReader/Main.cpp

clean:
	rm -rf ${EXENAME} *.o commands
	rm -rf ../LogReader/*.o ../LogReader/LogReader
