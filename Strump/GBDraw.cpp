#include "GBDraw.h"

#include <stdlib.h>
#include "alias.h"


GBDraw::GBDraw(Memory* _memory, Registers* _registers) {
	memory = _memory;
	registers = _registers;

	Width = Height = 0;
}
GBDraw::~GBDraw() {
	;
}

void GBDraw::drawInit(const char* title, int xpos, int ypos, uint8_t width, uint8_t height, bool fullscreen, 
	bool _showCommandOutput, bool _showBackgroundMap, bool _showTileMap, bool _showPaletteMap,
	bool _showOAMMap) {
	showCommandOutput = _showCommandOutput;
	showBackgroundMap = _showBackgroundMap;
	showTileMap = _showTileMap;
	showPaletteMap = _showPaletteMap;
	vRAMLocation = 0x9800;
	
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

	if (showTileMap) {
		tileWindow = SDL_CreateWindow("Tile info", 50, 260, 256, 192, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		tileRenderer = SDL_CreateRenderer(tileWindow, -1, 0);
		tileTexture = SDL_CreateTexture(tileRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 256, 192);
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

	if (showBackgroundMap) {
		fullBackgroundWindow = SDL_CreateWindow("Full Background", 350, 40, FULL_BACKGROUND_WIDTH, FULL_BACKGROUND_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		fullBackgroundRenderer = SDL_CreateRenderer(fullBackgroundWindow, -1, 0);
		fullBackgroundTexture = SDL_CreateTexture(fullBackgroundRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, FULL_BACKGROUND_WIDTH, FULL_BACKGROUND_HEIGHT);
	}
}

void GBDraw::render(bool CPUIsStopped) {

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
}

[[deprecated]]
string GBDraw::GetRegisterInfo() {
	std::stringstream st;
	st.exceptions(ifstream::badbit);

	st << "AF: " << ToHexString(registers->AF.af) << "  DE: " << ToHexString(registers->DE.de) << endl;
	st << "BC: " << ToHexString(registers->BC.bc) << "  HL: " << ToHexString(registers->HL.hl) << endl;
	st << "SP: " << ToHexString(registers->SP) << "  PC: " << ToHexString(registers->PC) << endl;
	st << "ZNHC: " << (memory->getFlag(Z) == 0 ? '0' : '1') << (memory->getFlag(N) == 0 ? '0' : '1') << (memory->getFlag(H) == 0 ? '0' : '1') << (memory->getFlag(C) == 0 ? '0' : '1') << endl;
	st << "TMA:  " << ToHexString(memory->get(TMA)) << "  TIMA: " << ToHexString(memory->get(TIMA)) << endl;
	st << "LCDC: " << ToHexString(memory->get(LCDC)) << "  STAT: " << ToHexString(memory->get(STAT)) << endl;
	st << "IE:   " << ToHexString(memory->get(IE)) << "  IF:   " << ToHexString(memory->get(IF)) << endl;
	st << "ROMB: " << ToHexString(memory->RomBank) << endl;

	return st.str();
}

[[deprecated]]
string GBDraw::ToHexString(uint8_t val) {
	std::stringstream st;
	if (val == 0)
		st << hex << setw(2) << "00";
	else
		st << hex << setw(2) << setfill('0') << (uint16_t)val;
	return st.str();

}
[[deprecated]]
string GBDraw::ToHexString(uint16_t val) {
	stringstream st;
	if (val == 0)
		st << hex << setw(4) << "0000";
	else
		st << hex << setw(4) << setfill('0') << val;
	return st.str();
}

void GBDraw::displayMe(void)
{
	loadBackground();
}

void GBDraw::loadBackground() {
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
			getTileAt((offset * (int8_t)tileNum) + address, background[i]);
		}
		else {
			getTileAt((offset * tileNum) + address, background[i]);
		}
	}
}
void GBDraw::loadWindow() {
	// Get wchich tile set to use
	uint16_t tileDataTableAddress = GetWindowTileMapLocation();
	uint16_t address = BGWindowTileLocation();
	uint16_t offset = 16;
	uint8_t tileNum;

	for (int i = 0; i < BACKGROUNDTILES; i++) {
		tileNum = memory->get(tileDataTableAddress + i);
		//printf("tileNum=%02x\n", Memory[bMap + i]);
		if (address == 0x9000) { // allow for negative numbers
			getTileAt((offset * (int8_t)tileNum) + address, windowX[i]);
		}
		else {
			getTileAt((offset * tileNum) + address, windowX[i]);
		}
	}
}

void GBDraw::printTileData(int tileNum) {

	//printf("address=%04x\n", (offset * tileNum) + address);
	printf("Tile data: ");
	for (int k = 0; k < 16; k++) {
		printf("%02x ", background[tileNum]->data[k]);
	}
	printf("\n");
}

void GBDraw::setBackgroundPixels() {
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

void GBDraw::setFullBackgroundPixels() {
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

void GBDraw::setSpritePixels() {
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
				getTileAt(base + (sprite->TileNumber * 16), &cur);

				for (int y = 0; y < 8; y++) {
					for (int x = 0; x < 8; x++) {
						getPixel(&cur, x, y, &pixel, sprite->XFlip, sprite->YFlip);
						uint32_t curPixel = screenPixels[(scY + y) * 160 + (scX + x)];
						if (sprite->SpritePriority == 0 || (sprite->SpritePriority == 1 && (curPixel == WHITE || curPixel == CLASSIC_WHITE))) {
							uint32_t colour = GetSpriteColourFor(pixel, sprite->CGBPalette);
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

					getTileAt(base + ((sprite->TileNumber + i) * 16), &cur);
					for (int y = 0; y < 8; y++) {
						for (int x = 0; x < 8; x++) {
							getPixel(&cur, x, y, &pixel, sprite->XFlip, sprite->YFlip);
							uint32_t curPixel = screenPixels[(scY + y) * 160 + (scX + x)];
							if (sprite->SpritePriority == 0 || (sprite->SpritePriority == 1 && (curPixel == WHITE || curPixel == CLASSIC_WHITE))) {
								uint32_t colour = GetSpriteColourFor(pixel, sprite->CGBPalette);
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
void GBDraw::GetSpriteByNumber(uint8_t spriteNum, Sprite* sprite) {
	uint16_t address = 0xfe00 + (spriteNum * 4); // Start address of sprite data
	uint8_t attributes = memory->ReadMem(address + 3);
	uint8_t spriteMode = (memory->ReadMem(LCDC) & 0x4) > 0 ? SPRITE_MODE_8x16 : SPRITE_MODE_8x8;

	sprite->Y = memory->ReadMem(address);
	sprite->X = memory->ReadMem(address + 1);
	if (spriteMode == SPRITE_MODE_8x8)
		sprite->TileNumber = memory->ReadMem(address + 2);
	else
		sprite->TileNumber = memory->ReadMem(address + 2) & 0xfe;
	sprite->Attributes = attributes;
	sprite->CGBPalette = (attributes & 0x3) == 0x3 ? 1 : 0;
	sprite->SpritePriority = ((attributes & 0x80) == 0x80) ? 1 : 0;
	sprite->YFlip = (attributes & 0x40) == 0x40;
	sprite->XFlip = (attributes & 0x20) == 0x20;
	sprite->CGBVbank = (attributes & 0x08) == 0x08;
	sprite->Number = spriteNum + 1;
}
uint32_t GBDraw::GetSpriteColourFor(uint8_t number, uint8_t paletteSwitch) {
	uint8_t palette = paletteSwitch == 1 ? memory->ReadMem(OBP1) : memory->ReadMem(OBP0);
	switch (number) {
		case 0:
			return TRANSPARENT; break;
		case 1:
			return GetColourForPaletteNumber((palette & 0x0c) >> 2); break;
		case 2:
			return GetColourForPaletteNumber((palette & 0x30) >> 4); break;
		case 3:
			return GetColourForPaletteNumber((palette & 0xc0) >> 6); break;
	}
	return WHITE;
}

#pragma endregion

void GBDraw::setTilePixels() {
	tile tile;
	uint8_t pX = 0;
	uint8_t pY = 0;
	uint8_t pixel;
	uint16_t sPixelsIndex = 0;
	uint16_t i, x, y, tile_x, tile_y;
	uint16_t tile_index = 0;
	uint16_t rwidth = 256;

	for (i = 0x8000; i <= 0x9fff; i += 0x10, tile_index++) {

		getTileAt(i, &tile);

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

void GBDraw::getTileAt(uint16_t address, tile* t) {
	for (int i = 0; i < 16; i++) {
		t->data[i] = memory->ReadMem(address + i);
	}
}

uint32_t GBDraw::GetColourFor(uint8_t number, tile *t) {
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
uint32_t GBDraw::GetColourForPaletteNumber(uint8_t pNumber) {
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

void GBDraw::SetColourMode(uint8_t mode) {
	colourMode = mode;
}
void GBDraw::ToggleColourMode() {
	colourMode = colourMode == MODE_CLASSIC ? MODE_CLEAR : MODE_CLASSIC;
}
void GBDraw::ToggleVRAMLocation() {
	vRAMLocation = vRAMLocation == 0x9800 ? 0x9c00 : 0x9800;
}


void GBDraw::clean() {
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