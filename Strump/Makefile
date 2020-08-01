CC=g++ -std=c++17
OBJS=Commands.cpp CPU.cpp JoypadState.cpp Draw.cpp Emulator.cpp Log.cpp Memory.cpp MBC1Memory.cpp MBC2Memory.cpp MBC3Memory.cpp MBC5Memory.cpp NoRamNoRomMemory.cpp RamNoRomMemory.cpp MemoryFactory.cpp ROMInfo.cpp Sprite.cpp test.cpp Timer.cpp Main.cpp
#OBJS=Main.cpp
#SDL2dir=~/Library/Frameworks/SDL2.framework/Headers

SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LDFLAGS= $(shell sdl2-config --libs) -lSDL2_image -lSDL2 -lSDL2_ttf

#LFLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf
SUPPRESS=-Wno-unused-variable -Wno-unused-private-field

debug:
	${CC} -Wall -o strump ${OBJS} ${SUPPRESS} -DRUNTESTS=1 ${SDL_CFLAGS} ${SDL_LDFLAGS} -lpthread

log:
	${CC} -Wall -o strump ${OBJS} ${SUPPRESS} -DLOG_COMMANDS=1 -DRUNTESTS=1 ${SDL_CFLAGS} ${SDL_LDFLAGS} -lpthread

release:
	${CC} -Wall -o strump ${OBJS} ${SUPPRESS} -DREAL_TIME_CPU=1 ${SDL_CFLAGS} ${SDL_LDFLAGS} -lpthread

logreader:
	${CC} -Wall ${SUPPRESS} -o ../LogReader/LogReader ../LogReader/Main.cpp

clean:
	rm -rf strump *.o commands
	rm -rf ../LogReader/*.o ../LogReader/LogReader
