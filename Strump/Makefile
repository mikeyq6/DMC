CC=g++ -std=c++17
OBJS=Commands.cpp CPU.cpp JoypadState.cpp *Draw.cpp GBCTile.cpp DrawFactory.cpp Emulator.cpp Log.cpp *Memory.cpp MemoryFactory.cpp ROMInfo.cpp Sprite.cpp test.cpp testMBC5.cpp Timer.cpp Main.cpp SaveStateManager*.cpp
LFLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf
SUPPRESS=-Wno-unused-variable -Wno-unused-private-field
EXENAME=strump
# DEBUGVARIABLES=-DRUNTESTS=1 -DSHOW_TILE_INFO=1
DEBUGVARIABLES=-DRUNTESTS=1

debug:
	make clean
	${CC} -g -Wall ${SUPPRESS} ${DEBUGVARIABLES} ${LFLAGS} -o ${EXENAME} ${OBJS}

debug-notests:
	make clean
	${CC} -g -Wall ${SUPPRESS} ${LFLAGS} -o ${EXENAME} ${OBJS}

log:
	make clean
	${CC} -g -Wall ${SUPPRESS} -DLOG_COMMANDS=1 -DRUNTESTS=1 ${LFLAGS} -o ${EXENAME} ${OBJS}

release:
	make clean
	${CC} -Wall ${SUPPRESS} -DREAL_TIME_CPU=1 ${LFLAGS} -o ${EXENAME} ${OBJS}

drelease:
	make clean
	${CC} -g -Wall ${SUPPRESS} -DREAL_TIME_CPU=1 ${LFLAGS} -o ${EXENAME} ${OBJS}

logreader:
	make clean
	${CC} -Wall ${SUPPRESS} -o ../LogReader/LogReader ../LogReader/Main.cpp

clean:
	rm -rf ${EXENAME} *.o commands
	rm -rf ../LogReader/*.o ../LogReader/LogReader
