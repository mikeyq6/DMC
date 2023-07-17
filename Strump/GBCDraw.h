#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "Memory.h"
#include "Draw.h"
#include "registers.h"
#include "Sprite.h"
#include "GBCTile.h"

class GBCDraw :
	public Draw
{
public:
	GBCDraw(Memory* _memory, Registers* _registers);
	~GBCDraw() override;

	GBCTile* background[BACKGROUNDTILES];
	GBCTile* windowX[BACKGROUNDTILES];
	uint32_t screenPixels[NUMPIXELS];
	uint32_t tilePixels[NUM_TILE_PIXELS];

	void drawInit(const char* title, int xpos, int ypos, uint8_t width, uint8_t height, 
		bool fullscreen, bool _showCommandOutput, bool _showBackgroundMap, bool _showTileMap, bool _showPaletteMap,
		bool _showOAMMap) override;
	void loadBackground();
	void loadWindow();
	void render(bool CPUIsStopped) override;
	void clean() override;
	void printTileData(int tileNum);

	void SetColourMode(uint8_t mode);
	void ToggleColourMode() override;

	FILE* out;
private:
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

	SDL_Window* paletteWindow;
	SDL_Texture* paletteTexture;
	SDL_Renderer* paletteRenderer;

	SDL_Window* oamWindow;
	SDL_Texture* oamTexture;
	SDL_Renderer* oamRenderer;

	TTF_Font *font;
	
	uint8_t Width, Height;

	void displayMe(void);
	void getTileAt(uint16_t address, tile* t, uint8_t vramBank);
	void setBackgroundPixels();
	void setFullBackgroundPixels();
	void setTilePixels();
	void setSpritePixels();
	void GetPaletteByNumber(bool isSprite, uint8_t paletteNumber, Palette *palette);
	void DrawPalette(SDL_Renderer* r, uint8_t pType, uint8_t pNum);
	uint32_t GetColourForPixel(bool isSprite, uint8_t pixel, uint8_t paletteNumber);
	uint32_t GetColourFor(uint8_t pixel, tile *t) override;
	uint32_t GetColourForPaletteNumber(uint8_t paletteNumber) override;
	string registerInfo;

	int text_width, text_height;
	uint32_t fullBackgroundPixels[FULL_BACKGROUND_PIXELS];
	uint8_t colourMode;


};