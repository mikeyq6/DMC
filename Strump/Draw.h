#pragma once

#include <cstdint>
#include "alias.h"
#include "registers.h"
#include "Sprite.h"
#include "Memory.h"
#include "registers.h"

constexpr auto BACKGROUNDTILES = 0x400;
constexpr auto NUMPIXELS = 0x5a00;
constexpr auto NUM_TILE_PIXELS = 0xc000;

constexpr auto WHITE = 0x00ffffff;
constexpr auto LT_GRAY = 0x00555555;
constexpr auto DK_GRAY = 0x00cccccc;
constexpr auto BLACK = 0x00000000;
constexpr auto TRANSPARENT = -1;

constexpr auto CLASSIC_WHITE = 0x00e0f8d0;
constexpr auto CLASSIC_LT_GRAY = 0x0088c070;
constexpr auto CLASSIC_DK_GRAY = 0x00346856;
constexpr auto CLASSIC_BLACK = 0x00081820;

constexpr auto MODE_CLASSIC = 1;
constexpr auto MODE_CLEAR = 0;

constexpr auto FULL_BACKGROUND_WIDTH = 256;
constexpr auto FULL_BACKGROUND_HEIGHT = 256;
constexpr auto FULL_BACKGROUND_PIXELS = 65536;

class Draw {
    public:

    virtual ~Draw() = 0;

    virtual void drawInit(const char* title, int xpos, int ypos, uint8_t width, uint8_t height, bool fullscreen, bool _showCommandOutput, bool _showBackgroundMap, bool _showTileMa) = 0;
	virtual void render(bool CPUIsStopped) = 0;
	virtual void clean() = 0;
	virtual void ToggleColourMode() = 0;

	virtual uint32_t GetColourFor(uint8_t number) = 0;
	virtual uint32_t GetColourForPaletteNumber(uint8_t pNumber) = 0;

    protected:

	Memory *memory;
	Registers* registers;

    bool compareSpriteX(Sprite* s1, Sprite* s2);
	void getPixel(tile* t, uint8_t col, uint8_t row, uint8_t* val);
	void getPixel(tile* t, uint8_t col, uint8_t row, uint8_t* val, bool xFlip, bool yFlip);
    uint16_t GetBackgroundTileMapLocation();
	uint16_t GetWindowTileMapLocation();
	uint16_t BGWindowTileLocation();
	bool GetWindowEnabled();
	bool tileIsNotEmpty(tile* t);
	bool SpritesEnabled();
};