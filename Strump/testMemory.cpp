#include "testMemory.h"
#include <assert.h>

void TestVramBanking();

TestMemory::TestMemory(Commands* _commands, Memory* _memory, Registers* _registers) {
	commands = _commands;
	memory = _memory;
	registers = _registers;
}
TestMemory::~TestMemory() {}

void TestMemory::RunTests() {
    TestVramBanking();
    TestWramBanking();
    TestPalettes();

}

void TestMemory::TestVramBanking() {
    // test VRam banking
    memory->WriteMem(VBK, 0); // set bank 0
    memory->WriteMem(0x8101, 0xe6);
    memory->WriteMem(0x8102, 0x15);
    memory->WriteMem(VBK, 0xef); // set bank 1
    assert(memory->ReadMem(0x8101) != 0xe6);
    assert(memory->ReadMem(0x8102) != 0x15);

    memory->WriteMem(0x8101, 0xa6);
    memory->WriteMem(0x8102, 0xa5);
    memory->WriteMem(VBK, 0xee); // set bank 0
    assert(memory->ReadMem(0x8101) == 0xe6);
    assert(memory->ReadMem(0x8102) == 0x15);

    memory->WriteMem(VBK, 0xef); // set bank 1
    assert(memory->ReadMem(0x8101) == 0xa6);
    assert(memory->ReadMem(0x8102) == 0xa5);
}

void TestMemory::TestWramBanking() {
    // Test WRam banking
    memory->WriteMem(SVBK, 0); // set bank 0, will default to 1
    memory->WriteMem(0xc040, 0xad);
    memory->WriteMem(0xc041, 0x3c);
    memory->WriteMem(0xd040, 0x33);
    memory->WriteMem(0xd041, 0x44);

    memory->WriteMem(SVBK, 2); // set bank 2
    assert(memory->ReadMem(0xc040) == 0xad); // addresses from c000 - d000 are always available
    assert(memory->ReadMem(0xc041) == 0x3c);
    memory->WriteMem(0xd040, 0x55);
    memory->WriteMem(0xd041, 0x66);

    memory->WriteMem(SVBK, 7); // set bank 7
    assert(memory->ReadMem(0xc040) == 0xad);
    assert(memory->ReadMem(0xc041) == 0x3c);
    memory->WriteMem(0xd040, 0x77);
    memory->WriteMem(0xd041, 0xbb);

    memory->WriteMem(SVBK, 0xf9); // set bank 1, highest 5 bits ignored
    assert(memory->ReadMem(0xd040) == 0x33);
    assert(memory->ReadMem(0xd041) == 0x44);

    memory->WriteMem(SVBK, 0xfa); // set bank 2, highest 5 bits ignored
    assert(memory->ReadMem(0xd040) == 0x55);
    assert(memory->ReadMem(0xd041) == 0x66);

    memory->WriteMem(SVBK, 0xff); // set bank 7, highest 5 bits ignored
    assert(memory->ReadMem(0xd040) == 0x77);
    assert(memory->ReadMem(0xd041) == 0xbb);
}

void TestMemory::TestPalettes() {
    
}