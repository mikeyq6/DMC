# Segmentation Fault Fixes

## Issues Found and Fixed

### GBCDraw.cpp

#### 1. Buffer Overflow in OAM Rendering (Line 161)
**Issue**: `char *hexString = (char*)malloc(sizeof(char) * 4)` allocates only 4 bytes, but `sprintf(hexString, "%X", val)` can write up to 8+ bytes for a uint8_t.

**Fix**: Use stack buffer `char hexString[8]` instead of malloc.

**Impact**: Prevents heap corruption and segfault.

#### 2. Memory Leak - Sprite Allocation (Line 155)
**Issue**: `Sprite *sprite = new Sprite()` allocated but never freed.

**Fix**: Removed unused sprite allocation.

**Impact**: Prevents memory leak.

#### 3. Memory Leak in DrawPalette (Line 195)
**Issue**: `Palette *palette = (Palette*)malloc(sizeof(Palette))` allocated but never freed.

**Fix**: Use stack allocation `Palette palette` instead of malloc.

**Impact**: Prevents memory leak and potential segfault from dangling pointers.

#### 4. Memory Leak in GetColourForPixel (Line 265)
**Issue**: `Palette *palette = (Palette*)malloc(sizeof(Palette))` allocated but never freed.

**Fix**: Use stack allocation `Palette palette` instead of malloc.

**Impact**: Prevents memory leak and potential segfault.

### MBC5Memory.cpp

#### 5. Array Out of Bounds in RAM Read (Line 60)
**Issue**: `CartRamBankData[RAMB][location]` - RAMB is not validated and can exceed MAX_RAM_BANKS.

**Fix**: Mask RAMB with `(MAX_RAM_BANKS - 1)` to ensure valid index.

**Impact**: Prevents out-of-bounds array access and segfault.

#### 6. Array Out of Bounds in RAM Write (Line 120)
**Issue**: `CartRamBankData[RAMB][location] = value` - RAMB not validated.

**Fix**: Mask RAMB with `(MAX_RAM_BANKS - 1)` to ensure valid index.

**Impact**: Prevents out-of-bounds array access and segfault.

## Summary

- **3 memory leaks fixed** (malloc without free)
- **1 buffer overflow fixed** (sprintf overflow)
- **2 array bounds violations fixed** (out-of-bounds access)

All fixes use minimal code changes and stack allocation where possible for better performance and safety.
