#include "MemoryFactory.h"

Memory* MemoryFactory::GetMemoryByType(CartridgeInfo *cartInfo) {
	Memory* memory = NULL;

	switch (cartInfo->type) {
		case 0:
			memory = new NoRamNoRomMemory();
			break;
		case 1:
			return new MBC1Memory(false, false);
			break;
		case 2:
			return new MBC1Memory(true, false);
			break;
		case 3:
			return new MBC1Memory(true, true);
			break;
		case 5:
			return new MBC2Memory(false);
			break;
		case 6:
			return new MBC2Memory(true);
			break;
		case 8:
			return new RamNoRomMemory(false);
			break;
		case 9:
			return new RamNoRomMemory(true);
			break;
		case 0xf:
			return new MBC3Memory(false, true, true); break;
		case 0x10:
			return new MBC3Memory(true, true, true); break;
		case 0x11:
			return new MBC3Memory(false, false); break;
		case 0x12:
			return new MBC3Memory(true, false);
			break;
		case 0x13:
			return new MBC3Memory(true, true);
			break;
		case 0x19:
			return new MBC5Memory(false, false, false, false);
			break;
		case 0x1a:
			return new MBC5Memory(true, false, false, false);
			break;
		case 0x1b:
			return new MBC5Memory(true, true, false, false);
			break;
		case 0x1c:
			return new MBC5Memory(false, false, true, false);
			break;
		case 0x1d:
			return new MBC5Memory(false, false, true, true);
			break;
		case 0x1e:
			return new MBC5Memory(false, true, true, true);
			break;

	}
	return memory;
}