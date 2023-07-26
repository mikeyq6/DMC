#include "GBCDraw.h"

#include <stdlib.h>
#include "alias.h"

int oamWidth = 330;
int oamHeight = 260;
int tileInfoWidth = 830;
int tileInfoHeight = 660;

GBCDraw::GBCDraw(Memory* _memory, Registers* _registers) {
	memory = _memory;
	registers = _registers;

	Width = Height = 0;
}
GBCDraw::~GBCDraw() {

}

void GBCDraw::drawInit(const char* title, int xpos, int ypos, uint8_t width, uint8_t height, bool fullscreen, 
		bool _showCommandOutput, bool _showBackgroundMap, bool _showTileMap, bool _showPaletteMap, 
		bool _showOAMMap) {
	showCommandOutput = _showCommandOutput;
	showBackgroundMap = _showBackgroundMap;
	showTileMap = _showTileMap;
	showPaletteMap = _showPaletteMap;
	showOAMMap = _showOAMMap;

	int flags = 0;
	if (fullscreen) {
		flags = SDL_WINDOW_FULLSCREEN;
	}

	for (int i = 0; i < BACKGROUNDTILES; i++) {
		background[i] = new GBCTile();
		windowX[i] = new GBCTile();
	}

	int gogogo = 1;

	Width = width;
	Height = height;

	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow(title, xpos, ypos, Width * 3, Height * 3, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | flags);
	renderer = SDL_CreateRenderer(window, -1, 0);
	texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGB888,
		SDL_TEXTUREACCESS_STREAMING,
		Width, Height);

	if (showBackgroundMap) {
		fullBackgroundWindow = SDL_CreateWindow("Full Background", 50, 0, FULL_BACKGROUND_WIDTH, FULL_BACKGROUND_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		fullBackgroundRenderer = SDL_CreateRenderer(fullBackgroundWindow, -1, 0);
		fullBackgroundTexture = SDL_CreateTexture(fullBackgroundRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, FULL_BACKGROUND_WIDTH, FULL_BACKGROUND_HEIGHT);
	}

	if (showTileMap) {
		tileWindow = SDL_CreateWindow("Tile info", 50, 326, 256, 192, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		tileRenderer = SDL_CreateRenderer(tileWindow, -1, 0);
		tileTexture = SDL_CreateTexture(tileRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 256, 192);
	}

	if (showPaletteMap) {
		paletteWindow = SDL_CreateWindow("Palette Info", 50, 544, 300, 300, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		paletteRenderer = SDL_CreateRenderer(paletteWindow, -1, 0);
		paletteTexture = SDL_CreateTexture(paletteRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 300, 300);
	}

	if (showOAMMap) {
		TTF_Init();
		font = TTF_OpenFont("./arial.ttf", 9);
		if (!font) {
			cout << "TTF_OpenFont: " << TTF_GetError() << endl;
			// handle error
		}
		oamWindow = SDL_CreateWindow("OAM Sprite Info", 1050, 0, oamWidth, oamHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		oamRenderer = SDL_CreateRenderer(oamWindow, -1, 0);
		oamTexture = SDL_CreateTexture(oamRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 300, 300);

	}

	#ifdef SHOW_TILE_INFO
	TTF_Init();
	font = TTF_OpenFont("./arial.ttf", 9);
	tileInfoWindow = SDL_CreateWindow("Background Tile Info", 1080, 400, tileInfoWidth, tileInfoHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	tileInfoRenderer = SDL_CreateRenderer(tileInfoWindow, -1, 0);
	tileInfoTexture = SDL_CreateTexture(tileInfoRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 300, 300);
	#endif

	// 
	// if (showCommandOutput) {
	// 	debugWindow = SDL_CreateWindow("Output", 1400, 400, 300, 300, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	// 	debugRenderer = SDL_CreateRenderer(debugWindow, -1, 0);
	// 	
	// 	
	// }
}

void GBCDraw::render(bool CPUIsStopped) {

	loadBackground();
	loadWindow();

	if(!CPUIsStopped) {
		setBackgroundPixels();

		if (showBackgroundMap) {
			setFullBackgroundPixels();
		}
		if (showTileMap) {
			setTilePixels();
		}
		if (this->SpritesEnabled()) {
			setSpritePixels();
		}
	}
	SDL_UpdateTexture(texture, NULL, screenPixels, Width * sizeof(uint32_t));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	if (showTileMap) {
		SDL_UpdateTexture(tileTexture, NULL, tilePixels, 256 * sizeof(uint32_t));
		SDL_RenderClear(tileRenderer);
		SDL_RenderCopy(tileRenderer, tileTexture, NULL, NULL);
		SDL_RenderPresent(tileRenderer);
	}

	if (showBackgroundMap) {
		SDL_UpdateTexture(fullBackgroundTexture, NULL, fullBackgroundPixels, FULL_BACKGROUND_WIDTH * sizeof(uint32_t));
		SDL_RenderClear(fullBackgroundRenderer);
		SDL_RenderCopy(fullBackgroundRenderer, fullBackgroundTexture, NULL, NULL);
		SDL_RenderPresent(fullBackgroundRenderer);
	}

	if(showPaletteMap) {
		// SDL_Surface *pSurface = SDL_GetWindowSurface(paletteWindow);
		// SDL_FillRect(pSurface, &rect, SDL_MapRGB(pSurface->format, 0x30, 0x40, 0x00));
		// paletteTexture = SDL_CreateTextureFromSurface(paletteRenderer, pSurface);
		SDL_SetRenderDrawColor(paletteRenderer, 0xff, 0xff, 0xff, 0xff);
		SDL_RenderClear(paletteRenderer);
		// Draw palette colours
		for(uint8_t i=0; i<2; i++) {
			for(uint8_t j=0; j<8; j++) {
				DrawPalette(paletteRenderer, i, j);
			}
		}
		
		// SDL_UpdateTexture(paletteTexture, NULL, fullBackgroundPixels, FULL_BACKGROUND_WIDTH * sizeof(uint32_t));
		// SDL_RenderCopy(paletteRenderer, paletteTexture, NULL, NULL);
		SDL_RenderPresent(paletteRenderer);
	}

	// TODO: Migrate this code to Draw.cpp
	if (showOAMMap) {
		SDL_SetRenderDrawColor(oamRenderer, 0xff, 0xff, 0xff, 0xff);
		SDL_RenderClear(oamRenderer);

		SDL_Color colour = { 0, 0, 0 };

		SDL_Surface *surface;
		SDL_Texture *texture;
		int texW = 0;
		int texH = 0;
		int spacer = 9;
		int gspacer = 40;
		int vgspacer = 0;
		Sprite *sprite = new Sprite();
		char *hexString = (char*)malloc(sizeof(char) * 4);
		uint8_t val;
		uint16_t address;
		
		for(int i=0; i<40; i++) {
			if(i > 0 && i % 8 == 0) {
				vgspacer += 50;
			}
			address = 0xfe00 + (i * 4); // Start address of sprite data
			for(int j=0; j<4; j++) {
				val = memory->ReadMem(address + j);
				sprintf(hexString, "%X", val); //convert number to hex

				surface = TTF_RenderText_Solid(font, hexString, colour);
				texture = SDL_CreateTextureFromSurface(oamRenderer, surface);
				
				SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
				SDL_Rect dstrect = { 20 + ((i % 8) * gspacer), 10 + vgspacer + (j*spacer), texW, texH };
				SDL_RenderCopy(oamRenderer, texture, NULL, &dstrect);

				SDL_DestroyTexture(texture);
				SDL_FreeSurface(surface);
			}
		}

		SDL_RenderPresent(oamRenderer);
		free(hexString);
	}

	#ifdef SHOW_TILE_INFO
	SDL_Color colour = { 0, 0, 0 };

	SDL_Surface *surface;
	SDL_Texture *texture;
	int texW = 0;
	int texH = 0;
	int gspacer = 25;
	int vgspacer = 0;
	int labelGutter = 30;
	char *hexString = (char*)malloc(sizeof(char) * 4);
	uint16_t address, tileInfo;
	
	SDL_SetRenderDrawColor(tileInfoRenderer, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderClear(tileInfoRenderer);

	for(int i=0; i<1024; i++) {
		if(i > 0 && i % 0x20 == 0) {
			vgspacer += 20;
		} 
		address = vRAMLocation + i; // Start address of background tile data
		// for(int j=0; j<4; j++) {
			tileInfo = memory->GetVramForAddress(address, 0) << 8;
			tileInfo |= memory->GetVramForAddress(address, 1);
			
			if(i % 0x20 == 0) {
				sprintf(hexString, "%X", address); //convert address to hex
				surface = TTF_RenderText_Solid(font, hexString, colour);
				texture = SDL_CreateTextureFromSurface(tileInfoRenderer, surface);
				SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
				SDL_Rect labelrect = { 10, 10 + vgspacer, texW, texH };
				SDL_RenderCopy(tileInfoRenderer, texture, NULL, &labelrect);
			}

			sprintf(hexString, "%X", tileInfo); //convert number to hex
			surface = TTF_RenderText_Solid(font, hexString, colour);
			texture = SDL_CreateTextureFromSurface(tileInfoRenderer, surface);
			SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
			SDL_Rect dstrect = { labelGutter + 10 + ((i % 32) * gspacer), 10 + vgspacer, texW, texH };
			SDL_RenderCopy(tileInfoRenderer, texture, NULL, &dstrect);

			SDL_DestroyTexture(texture);
			SDL_FreeSurface(surface);

			// if(tileInfo > 0) {
			// 	printf("i: %x, tileInfo: %x, [x: %d, y: %d]\n", i, tileInfo, dstrect.x, dstrect.y);
			// }
		// }
	}
	SDL_RenderPresent(tileInfoRenderer);

	#endif
}

void GBCDraw::DrawPalette(SDL_Renderer* r, uint8_t pType, uint8_t pNum) {
	SDL_Rect rect;
	uint8_t width = 20;
	uint8_t height = 20;
	uint8_t left_gutter = 10;
	uint8_t x_offset = 120;
	uint8_t x_gap = 5;
	uint8_t y_offset = 30;
	Palette *palette = (Palette*)malloc(sizeof(Palette));
	GetPaletteByNumber(pType == 1, pNum, palette);
	for(uint8_t i=0; i<4; i++) {
		RGB rgb = PaletteColourToRGB(palette->Colours[i]);
		rect.x = left_gutter + (pType * x_offset) + (i * (x_gap + width));
		rect.y = left_gutter + (pNum * y_offset);
		rect.w = width, rect.h = height;
		SDL_SetRenderDrawColor(r, rgb.r, rgb.g, rgb.b, 0xff);
		SDL_RenderFillRect(r, &rect);
	}
}

void GBCDraw::displayMe(void)
{
	loadBackground();
}

void GBCDraw::loadBackground() {
	// Get wchich tile set to use
	uint16_t tileDataTableAddress = GetBackgroundTileMapLocation();
	uint16_t address = BGWindowTileLocation();
	uint16_t offset = 16;
	uint8_t tileNum, tileAttributes;

	for (int i = 0; i < BACKGROUNDTILES; i++) {
		tileNum = memory->GetVramForAddress(tileDataTableAddress + i, 0);
		tileAttributes = memory->GetVramForAddress(tileDataTableAddress + i, 1);
		background[i]->TileNumber = tileNum;
		GBCTile::GetBackgroundTile(tileAttributes, background[i]);

		if (address == 0x9000) { // allow for negative numbers
			getTileAt((offset * (int8_t)tileNum) + address, background[i]->t, background[i]->CGBVbank);
		}
		else {
			getTileAt((offset * tileNum) + address, background[i]->t, background[i]->CGBVbank);
		}
	}
}
void GBCDraw::loadWindow() {
	// Get wchich tile set to use
	uint16_t tileDataTableAddress = GetWindowTileMapLocation();
	uint16_t address = BGWindowTileLocation();
	uint16_t offset = 16;
	uint8_t tileNum, tileAttributes;

	for (int i = 0; i < BACKGROUNDTILES; i++) {
		tileNum = memory->GetVramForAddress(tileDataTableAddress + i, 0);
		tileAttributes = memory->GetVramForAddress(tileDataTableAddress + i, 1);
		windowX[i]->TileNumber = tileNum;
		GBCTile::GetBackgroundTile(tileAttributes, windowX[i]);

		if (address == 0x9000) { // allow for negative numbers
			getTileAt((offset * (int8_t)tileNum) + address, windowX[i]->t, windowX[i]->CGBVbank);
		}
		else {
			getTileAt((offset * tileNum) + address, windowX[i]->t, windowX[i]->CGBVbank);
		}
	}
}

void GBCDraw::printTileData(int tileNum) {

	//printf("address=%04x\n", (offset * tileNum) + address);
	printf("Tile data: ");
	for (int k = 0; k < 16; k++) {
		printf("%02x ", background[tileNum]->t->data[k]);
	}
	printf("\n");
}

void GBCDraw::setBackgroundPixels() {
	uint8_t sX = memory->get(SCX);
	uint8_t sY = memory->get(SCY);
	tile* cur;
	uint8_t pixel;
	int sPixelsIndex = 0;

	// Which tile to start with
	uint8_t pX = sX % 8;
	uint8_t pY = sY % 8;
	uint8_t wX, wY;
	uint16_t x, y, tY, tX, index;
	// printf("sX=%02x, sY=%02x\n", sX, sY);

	for (y = 0; y < Height; y++) {
		for (x = 0; x < Width; x++, sPixelsIndex++) {

			tY = (y + sY) % (32 * 8); // Should wrap around
			tX = (x + sX) % (32 * 8); // Should wrap around

			pX = tX % 8;
			pY = tY % 8;

			index = ((tY / 8) * 32) + (tX / 8);

			getPixel(background[index]->t, pX, pY, &pixel, background[index]->XFlip, background[index]->YFlip);

			screenPixels[sPixelsIndex] = GetColourForPixel(false, pixel, background[index]->CGBPalette);
		}
		pixel = (uint8_t)10;
	}

	// Draw the window if it is enabled
	if (GetWindowEnabled() || true) {
		wX = memory->get(WX);
		wY = memory->get(WY);
		pX = pY = 0;
		uint8_t offsetX, offsetY;
		offsetX = wX - 7;
		offsetY = wY;

		if (wX >= 7 && wX <= 166 && wY >= 0 && wY <= 143) { // Don't bother if the window is offscreen
			for (int sY = 0; sY < Height - offsetY; sY++) {
				for (sX = 0; sX < Width - offsetX; sX++) {
					pX = sX % 8;
					pY = sY % 8;

					index = ((sY / 8) * 32) + (sX / 8);
					sPixelsIndex = (sX + offsetX) + ((sY + offsetY) * Width);

					getPixel(windowX[index]->t, pX, pY, &pixel, windowX[index]->XFlip, windowX[index]->YFlip);
					screenPixels[sPixelsIndex] = GetColourForPixel(false, pixel, windowX[index]->CGBPalette);
				}
			}
		}
	}
}

void GBCDraw::setFullBackgroundPixels() {
	tile* cur;
	uint8_t pixel;
	int sPixelsIndex = 0;

	// Which tile to start with
	uint8_t pX = 0;
	uint8_t pY = 0;
	uint8_t wX, wY;
	uint16_t x, y, tY, tX, index;

	for (y = 0; y < FULL_BACKGROUND_HEIGHT; y++) {
		for (x = 0; x < FULL_BACKGROUND_WIDTH; x++, sPixelsIndex++) {

			pX = x % 8;
			pY = y % 8;

			index = ((y / 8) * 32) + (x / 8);
			cur = background[index]->t;

			getPixel(cur, pX, pY, &pixel, background[index]->XFlip, background[index]->YFlip);

			fullBackgroundPixels[sPixelsIndex] = GetColourForPixel(false, pixel, background[index]->CGBPalette);
		}
		pixel = (uint8_t)10;
	}
}

#pragma region Sprite Methods

void GBCDraw::setSpritePixels() {
	Sprite* sprite = new Sprite();
	tile cur;
	uint16_t base = 0x8000;
	uint8_t pixel = 0;
	uint8_t spriteMode = (memory->ReadMem(LCDC) & 0x4) > 0 ? SPRITE_MODE_8x16 : SPRITE_MODE_8x8;
	vector<Sprite*> sprites = {};
	Palette *palette = (Palette*)malloc(sizeof(Palette));

	for (int i = 0; i < 40; i++) {
		Sprite::GetSpriteByNumber(i, memory, sprite);
		
		if (sprite->X <= 0 || sprite->Y <= 0) {
			continue; // Sprite is hidden
		}
		else {
			// Get screen position of sprite
			uint8_t scX, scY;

			if (spriteMode == SPRITE_MODE_8x8) {
				scX = sprite->X - 8;
				scY = sprite->Y - 16;
				getTileAt(base + (sprite->TileNumber * 16), &cur, sprite->CGBVbank);

				for (int y = 0; y < 8; y++) {
					for (int x = 0; x < 8; x++) {
						getPixel(&cur, x, y, &pixel, sprite->XFlip, sprite->YFlip);
						uint32_t curPixel = screenPixels[(scY + y) * 160 + (scX + x)];
						if (sprite->SpritePriority == 0 || (sprite->SpritePriority == 1 && (curPixel == WHITE || curPixel == CLASSIC_WHITE))) {
							uint32_t colour = GetColourForPixel(true, pixel, sprite->CGBPalette);
							if(colour != TRANSPARENT)
								screenPixels[(scY + y) * 160 + (scX + x)] = colour;
						}
					}
				}
			}
			else
			{ // 8x16 sprites
				for (int i = 0; i < 2; i++) {
					scX = sprite->X - 8;
					scY = sprite->Y - 16 + (i * 8);

					getTileAt(base + (sprite->TileNumber * 16), &cur, sprite->CGBVbank);
					for (int y = 0; y < 8; y++) {
						for (int x = 0; x < 8; x++) {
							getPixel(&cur, x, y, &pixel, sprite->XFlip, sprite->YFlip);
							uint32_t curPixel = screenPixels[(scY + y) * 160 + (scX + x)];
							if (sprite->SpritePriority == 0 || (sprite->SpritePriority == 1 && (curPixel == WHITE || curPixel == CLASSIC_WHITE))) {
								uint32_t colour = GetColourForPixel(true, pixel, sprite->CGBPalette);
								if (colour != TRANSPARENT)
									screenPixels[(scY + y) * 160 + (scX + x)] = colour;
							}
						}
					}
				}
			}
		}
	}
	free(palette);
}

void GBCDraw::GetPaletteByNumber(bool isSprite, uint8_t number, Palette *palette) {
	uint16_t colour = 0;
	uint8_t index = (number * 8) + (isSprite ? 0x40 : 0);
	uint8_t paletteDataH,  paletteDataL;

	for(int i=0; i<4; i++) {
		paletteDataL = memory->PaletteData[index++];
		paletteDataH = memory->PaletteData[index++];
		palette->Colours[i] = paletteDataL + (((uint16_t)paletteDataH) << 8);
	}
}

#pragma endregion

#pragma region Tile Methods

void GBCDraw::setTilePixels() {
	tile tile;
	uint8_t pX = 0;
	uint8_t pY = 0;
	uint8_t pixel;
	uint16_t sPixelsIndex = 0;
	uint16_t i, x, y, tile_x, tile_y;
	uint16_t tile_index = 0;
	uint16_t rwidth = 256;
	int index = 0;
	int loffset = 0;

	for(int vram = 0; vram < 2; vram++) {
		for (i = 0x8000; i < 0x9800; i += 0x10, tile_index++) {
			
			getTileAt(i, &tile, vram);

			for (y = 0; y < 8; y++) {
				for (x = 0; x < 8; x++, sPixelsIndex++) {
					pX = x % 8;
					pY = y % 8;
					tile_x = ((tile_index % 16) * 8);
					tile_y = (tile_index / 16) * rwidth * 8;

					getPixel(&tile, pX, pY, &pixel);
					int index = loffset + tile_x + tile_y + (pY * rwidth) + pX;

					tilePixels[index] = GetColourFor(pixel, &tile);
				}
			}
		}
		loffset += 128;
	}
}

void GBCDraw::getTileAt(uint16_t address, tile* t, uint8_t vramBank) {
	for (int i = 0; i < 16; i++) {
		// TODO: Why is tattributes taking the same location as data?
		t->data[i] = memory->GetVramForAddress(address + i, vramBank);
		t->address = address + i;
		t->attributes = memory->GetVramForAddress(address + i, vramBank); // ???
	}
}

#pragma endregion

#pragma region Colour Methods

uint32_t GBCDraw::GetColourForPixel(bool isSprite, uint8_t pixel, uint8_t paletteNumber) {
	if(isSprite && pixel == 0) return TRANSPARENT;

	Palette *palette = (Palette*)malloc(sizeof(Palette));
	GetPaletteByNumber(isSprite, paletteNumber, palette);

	uint16_t colourData = palette->Colours[pixel];
	RGB rgb = PaletteColourToRGB(colourData);
	uint32_t colour = static_cast<uint32_t>(rgb.b) +
		(static_cast<uint32_t>(rgb.g) << 8) + 
		(static_cast<uint32_t>(rgb.r) << 16);
	
	free(palette);

	return colour;
}


uint32_t GBCDraw::GetColourFor(uint8_t number, tile *t) {

	uint8_t palette = memory->get(BGP);

	switch (number) {
		case 0:
			return GetColourForPaletteNumber(palette & 0x03); break;
		case 1:
			return GetColourForPaletteNumber((palette & 0x0c) >> 2); break;
		case 2:
			return GetColourForPaletteNumber((palette & 0x30) >> 4); break;
		case 3:
			return GetColourForPaletteNumber((palette & 0xc0) >> 6); break;
	}
	return WHITE;
}

uint32_t GBCDraw::GetColourForPaletteNumber(uint8_t paletteNumber) {
	switch (paletteNumber) {
		case 0:
			if (colourMode == MODE_CLEAR)
				return WHITE;
			else
				return CLASSIC_WHITE;
			break;
		case 1:
			if (colourMode == MODE_CLEAR)
				return DK_GRAY;
			else
				return CLASSIC_LT_GRAY;
			break;
		case 2:
			if (colourMode == MODE_CLEAR)
				return LT_GRAY;
			else
				return CLASSIC_DK_GRAY;
			break;
		case 3:
			if (colourMode == MODE_CLEAR)
				return BLACK;
			else
				return CLASSIC_BLACK;
			break;
		default:
			return WHITE; break;
		}
}

void GBCDraw::SetColourMode(uint8_t mode) {
	colourMode = mode;
}
void GBCDraw::ToggleColourMode() {
	colourMode = colourMode == MODE_CLASSIC ? MODE_CLEAR : MODE_CLASSIC;
}
void GBCDraw::ToggleVRAMLocation() {
	vRAMLocation = vRAMLocation == 0x9800 ? 0x9c00 : 0x9800;
}

#pragma endregion

void GBCDraw::clean() {
	SDL_DestroyWindow(window);
	SDL_DestroyWindow(debugWindow);
	SDL_DestroyWindow(fullBackgroundWindow);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyRenderer(debugRenderer);
	SDL_DestroyRenderer(fullBackgroundRenderer);
	SDL_DestroyTexture(texture);
	SDL_DestroyTexture(debugTexture);
	SDL_DestroyTexture(fullBackgroundTexture);
	SDL_FreeSurface(textSurface);
	SDL_Quit();
}