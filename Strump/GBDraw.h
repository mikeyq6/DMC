#pragma once

#include <SDL2/SDL.h>
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

class GBDraw :
	public Draw
{
public:
	GBDraw(Memory* _memory, Registers* _registers);
	~GBDraw() override;

	tile* background[BACKGROUNDTILES];
	tile* windowX[BACKGROUNDTILES];
	uint32_t screenPixels[NUMPIXELS];
	uint32_t tilePixels[NUM_TILE_PIXELS];

	void drawInit(const char* title, int xpos, int ypos, uint8_t width, uint8_t height, bool fullscreen, 
		bool _showCommandOutput, bool _showBackgroundMap, bool _showTileMap, bool _showPaletteMap,
		bool _showOAMMap) override;
	void loadBackground();
	void loadWindow();
	void render(bool CPUIsStopped) override;
	void clean() override;
	void printTileData(int tileNum);

	uint32_t GetColourFor(uint8_t number, tile *t) override;
	uint32_t GetColourForPaletteNumber(uint8_t pNumber) override;

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
	
	uint8_t Width, Height;

	void displayMe(void);
	void getTileAt(uint16_t address, tile* t);
	void setBackgroundPixels();
	void setFullBackgroundPixels();
	void setTilePixels();
	void setSpritePixels();
	string ToHexString(uint8_t val);
	string ToHexString(uint16_t val);
	string GetRegisterInfo();
	void GetSpriteByNumber(uint8_t spriteNum, Sprite* sprite);
	string registerInfo;
	uint32_t GetSpriteColourFor(uint8_t number, uint8_t palette);

	int text_width, text_height;
	uint32_t fullBackgroundPixels[FULL_BACKGROUND_PIXELS];
	uint8_t colourMode;


};