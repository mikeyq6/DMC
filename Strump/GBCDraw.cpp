#include "GBCDraw.h"

#include <stdlib.h>
#include "alias.h"


GBCDraw::GBCDraw(Memory* _memory, Registers* _registers) {
	memory = _memory;
	registers = _registers;

	Width = Height = 0;
}
GBCDraw::~GBCDraw() {
	;
}

void GBCDraw::drawInit(const char* title, int xpos, int ypos, uint8_t width, uint8_t height, bool fullscreen, 
		bool _showCommandOutput, bool _showBackgroundMap, bool _showTileMap, bool _showPaletteMap) {
	showCommandOutput = _showCommandOutput;
	showBackgroundMap = _showBackgroundMap;
	showTileMap = _showTileMap;
	showPaletteMap = _showPaletteMap;

	int flags = 0;
	if (fullscreen) {
		flags = SDL_WINDOW_FULLSCREEN;
	}

	for (int i = 0; i < BACKGROUNDTILES; i++) {
		background[i] = new tile();
		windowX[i] = new tile();
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

	// TTF_Init();
	// if (showCommandOutput) {
	// 	debugWindow = SDL_CreateWindow("Output", 1400, 400, 300, 300, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	// 	debugRenderer = SDL_CreateRenderer(debugWindow, -1, 0);
	// 	font = TTF_OpenFont("./VeraMono.ttf", 16);
	// 	if (!font) {
	// 		cout << "TTF_OpenFont: " << TTF_GetError() << endl;
	// 		// handle error
	// 	}
	// }
}

void GBCDraw::render(bool CPUIsStopped) {

	loadBackground();
	loadWindow();
	setBackgroundPixels();
	if(!CPUIsStopped) {
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
}

void GBCDraw::DrawPalette(SDL_Renderer* r, uint8_t pType, uint8_t pNum) {
	SDL_Rect rect;
	uint8_t width = 20;
	uint8_t height = 20;
	uint8_t left_gutter = 10;
	uint8_t x_offset = 120;
	uint8_t x_gap = 5;
	uint8_t y_offset = 30;
	for(uint8_t i=0; i<4; i++) {
		rect.x = left_gutter + (pType * x_offset) + (i * (x_gap + width));
		rect.y = left_gutter + (pNum * y_offset);
		rect.w = width, rect.h = height;
		SDL_SetRenderDrawColor(r, 0xe0, 0xe0, 0x0, 0xff);
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
	uint8_t tileNum;
	//printf("bMap=%04x, address=%04x\n", bMap, address);

	for (int i = 0; i < BACKGROUNDTILES; i++) {
		tileNum = memory->get(tileDataTableAddress + i);
		//printf("tileNum=%02x\n", Memory[bMap + i]);
		if (address == 0x9000) { // allow for negative numbers
			getTileAt((offset * (int8_t)tileNum) + address, background[i], 0);
		}
		else {
			getTileAt((offset * tileNum) + address, background[i], 0);
		}
	}
}
void GBCDraw::loadWindow() {
	// Get wchich tile set to use
	uint16_t tileDataTableAddress = GetWindowTileMapLocation();
	uint16_t address = BGWindowTileLocation();
	uint16_t offset = 16;
	uint8_t tileNum;

	for (int i = 0; i < BACKGROUNDTILES; i++) {
		tileNum = memory->get(tileDataTableAddress + i);
		//printf("tileNum=%02x\n", Memory[bMap + i]);
		if (address == 0x9000) { // allow for negative numbers
			getTileAt((offset * (int8_t)tileNum) + address, windowX[i], 0);
		}
		else {
			getTileAt((offset * tileNum) + address, windowX[i], 0);
		}
	}
}

void GBCDraw::printTileData(int tileNum) {

	//printf("address=%04x\n", (offset * tileNum) + address);
	printf("Tile data: ");
	for (int k = 0; k < 16; k++) {
		printf("%02x ", background[tileNum]->data[k]);
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
			cur = background[index];
			if (tileIsNotEmpty(cur)) {
				int q = 0;
			}
			getPixel(cur, pX, pY, &pixel);

			//printf("windowPixels[%04x] = %08x\n", sPixelsIndex, sPixel);
			screenPixels[sPixelsIndex] = GetColourFor(pixel, cur);
			switch (pixel) {
			case 0:
				pixel = 32; break;
			case 1:
				pixel = 46; break;
			case 2:
				pixel = 56; break;
			case 3:
				pixel = 35; break;
			}
		}
		pixel = (uint8_t)10;
	}

	// TEST: Set one random pixel as black each refresh
	/*int randomnumber;
	randomnumber = rand() % (Height * Height);
	screenPixels[randomnumber] = BLACK;*/

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
					cur = windowX[index];
					sPixelsIndex = (sX + offsetX) + ((sY + offsetY) * Width);

					getPixel(cur, pX, pY, &pixel);
					screenPixels[sPixelsIndex] = GetColourFor(pixel, cur);
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
			cur = background[index];

			getPixel(cur, pX, pY, &pixel);

			fullBackgroundPixels[sPixelsIndex] = GetColourFor(pixel, cur);
			switch (pixel) {
				case 0:
					pixel = 32; break;
				case 1:
					pixel = 46; break;
				case 2:
					pixel = 56; break;
				case 3:
					pixel = 35; break;
			}
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

	for (int i = 0; i < 40; i++) {
		GetSpriteByNumber(i, sprite);
		//	sprites.push_back(sprite);
		//}
		//// Sort sprites by X
		////sort(sprites.begin(), sprites.end(), compareSpriteX);
		//for(Sprite* s : sprites) {
		if (sprite->X <= 0 || sprite->Y <= 0) {
			continue; // Sprite is hidden
		}
		else {
			// Get screen position of sprite
			uint8_t scX, scY;

			if (spriteMode == SPRITE_MODE_8x8) {
				scX = sprite->X - 8;
				scY = sprite->Y - 16;
				// getTileAt(base + ((sprite->TileNumber + i) * 16), &cur, sprite->GetSpriteTileVramBank());
				getTileAt(base + ((sprite->TileNumber + i) * 16), &cur, sprite->CGBVbank);

				for (int y = 0; y < 8; y++) {
					for (int x = 0; x < 8; x++) {
						getPixel(&cur, x, y, &pixel, sprite->XFlip, sprite->YFlip);
						uint32_t curPixel = screenPixels[(scY + y) * 160 + (scX + x)];
						if (sprite->SpritePriority == 0 || (sprite->SpritePriority == 1 && (curPixel == WHITE || curPixel == CLASSIC_WHITE))) {
							uint32_t colour = GetSpriteColourFor(pixel, sprite, &cur);
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
// getTileAt(base + ((sprite->TileNumber + i) * 16), &cur, sprite->GetSpriteTileVramBank());
				
					getTileAt(base + ((sprite->TileNumber + i) * 16), &cur, sprite->CGBVbank);
					for (int y = 0; y < 8; y++) {
						for (int x = 0; x < 8; x++) {
							getPixel(&cur, x, y, &pixel, sprite->XFlip, sprite->YFlip);
							uint32_t curPixel = screenPixels[(scY + y) * 160 + (scX + x)];
							if (sprite->SpritePriority == 0 || (sprite->SpritePriority == 1 && (curPixel == WHITE || curPixel == CLASSIC_WHITE))) {
								uint32_t colour = GetSpriteColourFor(pixel, sprite, &cur);
								if (colour != TRANSPARENT)
									screenPixels[(scY + y) * 160 + (scX + x)] = colour;
							}
						}
					}
				}
			}
		}
	}
}
void GBCDraw::GetSpriteByNumber(uint8_t spriteNum, Sprite* sprite) {
	uint16_t address = 0xfe00 + (spriteNum * 4); // Start address of sprite data
	uint8_t attributes = memory->ReadMem(address + 3);
	uint8_t spriteMode = (memory->ReadMem(LCDC) & 0x4) > 0 ? SPRITE_MODE_8x16 : SPRITE_MODE_8x8;
	sprite->Attributes = attributes;
	// printf("attributes: %x\n", attributes);
	sprite->Y = memory->ReadMem(address);
	sprite->X = memory->ReadMem(address + 1);
	if (spriteMode == SPRITE_MODE_8x8)
		sprite->TileNumber = memory->ReadMem(address + 2);
	else
		sprite->TileNumber = memory->ReadMem(address + 2) & 0xfe;
	sprite->CGBPalette = attributes & 0x7;
	sprite->SpritePriority = ((attributes & 0x80) == 0x80) ? 1 : 0;
	sprite->YFlip = (attributes & 0x40) == 0x40;
	sprite->XFlip = (attributes & 0x20) == 0x20;
	sprite->PaletteBank = (attributes & 0x10) == 0x10;
	// sprite->CGBVbank = (attributes & 0x8) == 0x8;
	sprite->CGBVbank = (attributes & 0x8) >> 3;
	sprite->Number = spriteNum + 1;
	if(sprite->CGBPalette > 1) {
		// sprite->Draw();
		// printf("attributes: %x\n", attributes);

	}
	// sprite->Draw();
}
uint32_t GBCDraw::GetSpriteColourFor(uint8_t number, Sprite *sprite, tile* t) {
	// uint8_t attributes = sprite->Attributes;
	// printf("palette: %x\n", palette);
	if(number == 0) return TRANSPARENT;
	Palette *palette = GetPaletteNumber(true, sprite->CGBPalette);
	uint16_t colourData = palette->Colours[number];

	// printf("address: %x, colourData: %x\n", t->address, colourData);

	uint8_t red = colourData & 0x1f;
	uint8_t green = colourData & (0x1f << 5);
	uint8_t blue = colourData & (0x1f << 10);

	float redP = red / 0x1f;
	float greenP = green / 0x1f;
	float blueP = blue / 0x1f;

	uint32_t colour = static_cast<uint32_t>(redP * 0xff) +
		(static_cast<uint32_t>(greenP * 0xff) << 8) + 
		(static_cast<uint32_t>(blueP * 0xff) << 16);

	// printf("red: %x, green: %x, blue: %x, redP: %f, greenP: %f, blueP: %f, colour: %x\n",
	// 	red, green, blue, redP, greenP, blueP, colour);
	
	free(palette);

	return colour;
}

Palette* GBCDraw::GetPaletteNumber(bool isSprite, uint8_t number) {
	Palette *palette = (Palette*)malloc(sizeof(Palette));

	uint16_t colour = 0;
	uint8_t index = (number * 4) + (isSprite ? 0x40 : 0);
	uint8_t paletteDataH,  paletteDataL;

	for(int i=0; i<4; i++) {
		paletteDataH = memory->PaletteData[index++];
		paletteDataL = memory->PaletteData[index++];
		palette->Colours[i] = paletteDataL + (((uint16_t)paletteDataH) << 8);
	}

	return palette;
}

#pragma endregion

void GBCDraw::setTilePixels() {
	tile tile;
	uint8_t pX = 0;
	uint8_t pY = 0;
	uint8_t pixel;
	uint16_t sPixelsIndex = 0;
	uint16_t i, x, y, tile_x, tile_y;
	uint16_t tile_index = 0;
	uint16_t rwidth = 256;

	for (i = 0x8000; i <= 0x9fff; i += 0x10, tile_index++) {

		getTileAt(i, &tile, 0);

		for (y = 0; y < 8; y++) {
			for (x = 0; x < 8; x++, sPixelsIndex++) {
				pX = x % 8;
				pY = y % 8;
				tile_x = (tile_index % 32) * 8;
				tile_y = (tile_index / 32) * rwidth * 8;

				getPixel(&tile, pX, pY, &pixel);
				int index = tile_x + tile_y + (pY * rwidth) + pX;

				tilePixels[index] = GetColourFor(pixel, &tile);
			}
		}
	}
}

void GBCDraw::getTileAt(uint16_t address, tile* t, uint8_t vramBank) {
	for (int i = 0; i < 16; i++) {
		if(vramBank == 0) {
			t->data[i] = memory->ReadMem(address + i);
		} else {
			t->data[i] = memory->GetVramForAddress(address + i);
		}
		t->address = address + i;
		t->attributes = memory->GetVramForAddress(address + i);
	}
}

uint32_t GBCDraw::GetColourFor(uint8_t number, tile *t) {
	// uint8_t attributes = memory->GetVramForAddress(t->address);
	// uint8_t palette = t->attributes & 0x3;

	// uint16_t colourData = memory->GetPaletteColourInfo((palette << 3) + (number << 1));

	// // printf("address: %x, attributes: %x, palette: %x, colourData: %x\n", t->address, attributes, palette, colourData);

	// uint8_t red = colourData & 0x1f;
	// uint8_t green = colourData & (0x1f << 5);
	// uint8_t blue = colourData & (0x1f << 10);

	// float redP = red / 0x1f;
	// float greenP = green / 0x1f;
	// float blueP = blue / 0x1f;

	// uint32_t colour = static_cast<uint32_t>(redP * 0xff) +
	// 	(static_cast<uint32_t>(greenP * 0xff) << 8) + 
	// 	(static_cast<uint32_t>(blueP * 0xff) << 16);

	// // printf("red: %x, green: %x, blue: %x, redP: %f, greenP: %f, blueP: %f, colour: %x\n",
	// // 	red, green, blue, redP, greenP, blueP, colour);
	// return colour;

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
uint32_t GBCDraw::GetColourForPaletteNumber(uint8_t pNumber) {
	switch (pNumber) {
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