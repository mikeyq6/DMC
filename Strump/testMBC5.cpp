#include "testMBC5.h"
#include <assert.h>

TestMBC5::TestMBC5(Commands* _commands, Memory* _memory, Registers* _registers) {
	commands = _commands;
	memory = _memory;
	registers = _registers;
}
TestMBC5::~TestMBC5() {}

void TestMBC5::RunTests() {
    // Test HDMA functions

    // Set value to copy
    uint8_t val1 = memory->ReadMem(0x4460);
    uint8_t val2 = memory->ReadMem(0x446f);
    uint8_t val3 = memory->ReadMem(0x4470); // This should not be copied
    // Write source 0x4466. Four lower bits should be ignored, so should work out to be 0x4460
    memory->WriteMem(HDMA1, 0x44);
    memory->WriteMem(HDMA2, 0x66);
    // Write destination 0x8014. Four lower bits should be ignored, so should work out to be 0x8010
    memory->WriteMem(HDMA3, 0x80);
    memory->WriteMem(HDMA4, 0x14);
    // Write exactly one byte from source to destination
    memory->WriteMem(HDMA5, 0);
    assert(memory->ReadMem(0x8010) == val1);
    assert(memory->ReadMem(0x801f) == val2);
    assert(memory->ReadMem(0x8020) != val3);

    val1 = memory->ReadMem(0x5560);
    val2 = memory->ReadMem(0x557f);
    val3 = memory->ReadMem(0x5580);
    // Write source 0x4466. Four lower bits should be ignored, so should work out to be 0x4460
    memory->WriteMem(HDMA1, 0x55);
    memory->WriteMem(HDMA2, 0x6f);
    // Write destination 0x8014. Four lower bits should be ignored, so should work out to be 0x8010
    memory->WriteMem(HDMA3, 0x82);
    memory->WriteMem(HDMA4, 0x55);
    // Write exactly one byte from source to destination
    memory->WriteMem(HDMA5, 1);
    assert(memory->ReadMem(0x8250) == val1);
    assert(memory->ReadMem(0x826f) == val2);
    assert(memory->ReadMem(0x8270) != val3);
}