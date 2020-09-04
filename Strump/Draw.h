#pragma once

#include <SDL2/SDL.h>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "Memory.h"
#include "registers.h"
#include "Sprite.h"

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

bool compareSpriteX(Sprite* s1, Sprite* s2);

class Draw
{
private:
	bool showCommandOutput, showBackgroundMap, showTileMap;
	Memory *memory;
	Registers* registers;
	SDL_Event event;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;

	SDL_Window* debugWindow;
	SDL_Texture* debugTexture;
	SDL_Renderer* debugRenderer;
	SDL_Surface* textSurface;
	SDL_Rect renderQuad;
	SDL_Color textColor = { 255, 255, 255, 0 };

	SDL_Window* tileWindow;
	SDL_Texture* tileTexture;
	SDL_Renderer* tileRenderer;

	SDL_Window* fullBackgroundWindow;
	SDL_Texture* fullBackgroundTexture;
	SDL_Renderer* fullBackgroundRenderer;
	
	uint8_t Width, Height;

	void displayMe(void);
	void getPixel(tile* t, uint8_t col, uint8_t row, uint8_t* val);
	void getPixel(tile* t, uint8_t col, uint8_t row, uint8_t* val, bool xFlip, bool yFlip);
	void getTileAt(uint16_t address, tile* t);
	void setBackgroundPixels();
	void setFullBackgroundPixels();
	void setTilePixels();
	void setSpritePixels();
	uint16_t GetBackgroundTileMapLocation();
	uint16_t GetWindowTileMapLocation();
	uint16_t BGWindowTileLocation();
	bool GetWindowEnabled();
	string ToHexString(uint8_t val);
	string ToHexString(uint16_t val);
	string GetRegisterInfo();
	void GetSpriteByNumber(uint8_t spriteNum, Sprite* sprite);
	string registerInfo;
	bool SpritesEnabled();
	bool tileIsNotEmpty(tile* t);
	uint32_t GetSpriteColourFor(uint8_t number, uint8_t palette);

	int text_width, text_height;
	uint32_t fullBackgroundPixels[FULL_BACKGROUND_PIXELS];
	uint8_t colourMode;

public:
	Draw(Memory* _memory, Registers* _registers);
	~Draw();

	tile* background[BACKGROUNDTILES];
	tile* windowX[BACKGROUNDTILES];
	uint32_t screenPixels[NUMPIXELS];
	uint32_t tilePixels[NUM_TILE_PIXELS];

	void drawInit(const char* title, int xpos, int ypos, uint8_t width, uint8_t height, bool fullscreen, bool _showCommandOutput, bool _showBackgroundMap, bool _showTileMa);
	void loadBackground();
	void loadWindow();
	void render(bool CPUIsStopped);
	void clean();
	void printTileData(int tileNum);

	uint32_t GetColourFor(uint8_t number);
	uint32_t GetColourForPaletteNumber(uint8_t pNumber);

	void SetColourMode(uint8_t mode);
	void ToggleColourMode();

	FILE* out;

};