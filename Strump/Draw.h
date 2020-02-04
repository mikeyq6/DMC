#pragma once

#include <SDL.h>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "SDL_ttf.h"
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


class Draw
{
private:
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
	TTF_Font* font;
	SDL_Color textColor = { 255, 255, 255, 0 };

	SDL_Window* tileWindow;
	SDL_Texture* tileTexture;
	SDL_Renderer* tileRenderer;
	
	uint8_t Width, Height;

	void displayMe(void);
	void getPixel(tile* t, uint8_t col, uint8_t row, uint8_t* val);
	void getTileAt(uint16_t address, tile* t);
	void setBackgroundPixels();
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


	int text_width, text_height;

public:
	Draw(Memory* _memory, Registers* _registers);
	~Draw();

	tile* background[BACKGROUNDTILES];
	tile* windowX[BACKGROUNDTILES];
	uint32_t screenPixels[NUMPIXELS];
	uint32_t tilePixels[NUM_TILE_PIXELS];

	void drawInit(const char* title, uint8_t xpos, uint8_t ypos, uint8_t width, uint8_t height, bool fullscreen);
	void loadBackground();
	void loadWindow();
	void render(bool CPUIsStopped);
	void clean();
	void printTileData(int tileNum);

	uint32_t GetColourFor(uint8_t number);
	uint32_t GetColourForPaletteNumber(uint8_t pNumber);

	FILE* out;

};