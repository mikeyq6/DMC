#include "Draw.h"

#include <stdlib.h>
#include "alias.h"


Draw::Draw(Memory* _memory, Registers* _registers) {
	memory = _memory;
	registers = _registers;

	Width = Height = 0;
}
Draw::~Draw() {
	;
}

void Draw::drawInit(const char* title, int xpos, int ypos, uint8_t width, uint8_t height, bool fullscreen, bool _showCommandOutput, bool _showBackgroundMap, bool _showTileMap) {
	showCommandOutput = _showCommandOutput;
	showBackgroundMap = _showBackgroundMap;
	showTileMap = _showTileMap;

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
	window = SDL_CreateWindow(title, xpos, ypos, Width*3, Height*3, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | flags);
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

	TTF_Init();
	if (showCommandOutput) {
		debugWindow = SDL_CreateWindow("Output", 1400, 400, 300, 300, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		debugRenderer = SDL_CreateRenderer(debugWindow, -1, 0);
		font = TTF_OpenFont("./VeraMono.ttf", 16);
		if (!font) {
			cout << "TTF_OpenFont: " << TTF_GetError() << endl;
			// handle error
		}
	}

	if (showBackgroundMap) {
		fullBackgroundWindow = SDL_CreateWindow("Full Background", 350, 40, FULL_BACKGROUND_WIDTH, FULL_BACKGROUND_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		fullBackgroundRenderer = SDL_CreateRenderer(fullBackgroundWindow, -1, 0);
		fullBackgroundTexture = SDL_CreateTexture(fullBackgroundRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, FULL_BACKGROUND_WIDTH, FULL_BACKGROUND_HEIGHT);
	}
}

void Draw::render(bool CPUIsStopped) {
	//printf("Refresh...");
	//_getch();
	//glutMainLoopEvent();
	loadBackground();
	loadWindow();
	setBackgroundPixels(); 
	if (showBackgroundMap) {
		setFullBackgroundPixels();
	}
	if (showTileMap) {
		setTilePixels();
	}
	if(SpritesEnabled())
		setSpritePixels();

	//if (!CPUIsStopped)
	SDL_UpdateTexture(texture, NULL, screenPixels, Width * sizeof(uint32_t));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL); 
	SDL_RenderPresent(renderer);
	//SDL_PollEvent(NULL);

	if (showCommandOutput) {
		//textSurface = TTF_RenderText_Solid(font, (st.str()).c_str(), textColor);
		registerInfo = GetRegisterInfo();
		textSurface = TTF_RenderText_Blended_Wrapped(font, registerInfo.c_str(), textColor, 300);   
		debugTexture = SDL_CreateTextureFromSurface(debugRenderer, textSurface);
		if (textSurface) {
			text_width = textSurface->w;
			text_height = textSurface->h;
		}
		renderQuad = { 20, 10, text_width, text_height };
		SDL_RenderClear(debugRenderer);
		SDL_RenderCopy(debugRenderer, debugTexture, NULL, &renderQuad);
		SDL_RenderPresent(debugRenderer);
	}

	if (showTileMap) {
		SDL_UpdateTexture(tileTexture, NULL, tilePixels, 256 * sizeof(uint32_t));
		SDL_RenderClear(tileRenderer);
		SDL_RenderCopy(tileRenderer, tileTexture, NULL, NULL);
		SDL_RenderPresent(tileRenderer);
	}
	//_getch();

	if (showBackgroundMap) {
		SDL_UpdateTexture(fullBackgroundTexture, NULL, fullBackgroundPixels, FULL_BACKGROUND_WIDTH * sizeof(uint32_t));
		SDL_RenderClear(fullBackgroundRenderer);
		SDL_RenderCopy(fullBackgroundRenderer, fullBackgroundTexture, NULL, NULL);
		SDL_RenderPresent(fullBackgroundRenderer);
	}
}

string Draw::GetRegisterInfo() {
	std::stringstream st;
	st.exceptions(ifstream::badbit);

	st << "AF: " << ToHexString(registers->AF.af) << "  DE: " << ToHexString(registers->DE.de) << endl;
	st << "BC: " << ToHexString(registers->BC.bc) << "  HL: " << ToHexString(registers->HL.hl) << endl;
	st << "SP: " << ToHexString(registers->SP) << "  PC: " << ToHexString(registers->PC) << endl;
	st << "ZNHC: " << (memory->getFlag(Z) == 0 ? '0' : '1') << (memory->getFlag(N) == 0 ? '0' : '1') << (memory->getFlag(H) == 0 ? '0' : '1') << (memory->getFlag(C) == 0 ? '0' : '1') << endl;
	st << "TMA:  " << ToHexString(memory->get(TMA))  << "  TIMA: " << ToHexString(memory->get(TIMA)) << endl;
	st << "LCDC: " << ToHexString(memory->get(LCDC)) << "  STAT: " << ToHexString(memory->get(STAT)) << endl;
	st << "IE:   " << ToHexString(memory->get(IE))   << "  IF:   " << ToHexString(memory->get(IF)) << endl;
	st << "ROMB: " << ToHexString(memory->RomBank) << endl;

	return st.str();
}

string Draw::ToHexString(uint8_t val) {
	std::stringstream st;
	if (val == 0)
		st << hex << setw(2) << "00";
	else
		st << hex << setw(2) << setfill('0') << (uint16_t)val;
	return st.str();

}
string Draw::ToHexString(uint16_t val) {
	stringstream st;
	if (val == 0)
		st << hex << setw(4) << "0000";
	else
		st << hex << setw(4) << setfill('0') << val;
	return st.str();
}

void Draw::displayMe(void)
{
	printf("x");
	loadBackground();
}

void Draw::loadBackground() {
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
void Draw::loadWindow() {
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

void Draw::printTileData(int tileNum) {

	//printf("address=%04x\n", (offset * tileNum) + address);
	printf("Tile data: ");
	for (int k = 0; k < 16; k++) {
		printf("%02x ", background[tileNum]->data[k]);
	}
	printf("\n");
}

void Draw::setBackgroundPixels() {
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
			screenPixels[sPixelsIndex] = GetColourFor(pixel);
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
					screenPixels[sPixelsIndex] = GetColourFor(pixel);
				}
			}
		}
	}
}

bool Draw::tileIsNotEmpty(tile *t) {
	for (int i = 0; i < 16; i++) {
		if (t->data[i] > 0)
			return true;
	}
	return false;
}

void Draw::setFullBackgroundPixels() {
	tile* cur;
	uint8_t pixel;
	int sPixelsIndex = 0;

	// Which tile to start with
	uint8_t pX = 0;
	uint8_t pY = 0;
	uint8_t wX, wY;
	uint16_t x, y, tY, tX, index;
	// printf("sX=%02x, sY=%02x\n", sX, sY);

	for (y = 0; y < FULL_BACKGROUND_HEIGHT; y++) {
		for (x = 0; x < FULL_BACKGROUND_WIDTH; x++, sPixelsIndex++) {
			
			pX = x % 8;
			pY = y % 8;

			index = ((y / 8) * 32) + (x / 8); 
			cur = background[index];

			getPixel(cur, pX, pY, &pixel);

			//printf("windowPixels[%04x] = %08x\n", sPixelsIndex, sPixel);
			fullBackgroundPixels[sPixelsIndex] = GetColourFor(pixel);
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

void Draw::setSpritePixels() {
	Sprite* s = new Sprite();
	tile cur;
	uint16_t base = 0x8000;
	uint8_t pixel = 0;
	uint8_t spriteMode = (memory->ReadMem(LCDC) & 0x4) > 0 ? SPRITE_MODE_8x16 : SPRITE_MODE_8x8;

	for (int i = 0; i < 40; i++) {
		GetSpriteByNumber(i, s);
		if (s->X <= 0 || s->Y <= 0) {
			continue; // Sprite is hidden
		}
		else {
			// Get screen position of sprite
			uint8_t scX, scY;

			if (spriteMode == SPRITE_MODE_8x8) {
				scX = s->X - 8;
				scY = s->Y - 16;
				getTileAt(base + (s->TileNumber * 16), &cur);

				for (int y = 0; y < 8; y++) {
					for (int x = 0; x < 8; x++) {
						getPixel(&cur, x, y, &pixel, s->XFlip, s->YFlip);
						uint32_t colour = GetColourFor(pixel);
						if (colour != WHITE)
							screenPixels[(scY + y) * 160 + (scX + x)] = colour;
					}
				}
			}
			else 
			{ // 8x16 sprites
				for (int i = 0; i < 2; i++) {
					scX = s->X - 8;
					scY = s->Y - 16 + (i*8);

					getTileAt(base + ((s->TileNumber + i) * 16), &cur);
					for (int y = 0; y < 8; y++) {
						for (int x = 0; x < 8; x++) {
							getPixel(&cur, x, y, &pixel, s->XFlip, s->YFlip);
							uint32_t colour = GetColourFor(pixel);
							if(colour != WHITE)
								screenPixels[(scY + y) * 160 + (scX + x)] = colour;
						}
					}
				}
			}
		}
	}
}
bool Draw::SpritesEnabled() {
	return (memory->ReadMem(LCDC) & 0x2) == 0x2;
}

void Draw::setTilePixels() {  
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

				tilePixels[index] = GetColourFor(pixel);
			}
		}
	}
}
void Draw::getPixel(tile* t, uint8_t col, uint8_t row, uint8_t* val) {
	return getPixel(t, col, row, val, false, false);
}
void Draw::getPixel(tile* t, uint8_t col, uint8_t row, uint8_t* val, bool xFlip, bool yFlip) {
	uint8_t bit = 0;
	uint8_t rIndex = 0;
	if (xFlip)
		bit = 0x80 >> (8 - (col + 1));
	else
		bit = 1 << (8 - (col + 1));
	if (yFlip)
		rIndex = 16 - (row * 2);
	else
		rIndex = (row * 2);
	*val = ((t->data[rIndex] & bit) ? 1 : 0) + (((t->data[rIndex + 1]) & bit) ? 2 : 0);
}
void Draw::getTileAt(uint16_t address, tile* t) {
	//printf("address=%04x\n", address);
	for (int i = 0; i < 16; i++) {
		t->data[i] = memory->ReadMem(address + i);
		//printf("row[%u] = %x, ReadMem(%02x) = %x\n", i, t->data[i], address + i, ReadMem(address + i));
	}

	// uint8_t val = 0;
	// if(address != 0x8000) {
		// printf("Tile data: ");
		// for(int k=0; k<16; k++) {
			// printf("%02x ", t->data[k]);
		// }
		// printf("\n");
		// for(int i=0; i<8; i++) {
			// for(int j=0; j<8; j++) {
				// getPixel(t, i, j, &val);
				// if(val > 0) {
					// printf("%u", val);
				// } else {
					// printf(" ");
				// }
			// }
			// printf("\n");
		// }
		// printf("\n");
	// }

}

uint32_t Draw::GetColourFor(uint8_t number) {
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
uint32_t Draw::GetColourForPaletteNumber(uint8_t pNumber) {
	switch (pNumber) {
		case 0:
			return WHITE; break;
		case 1:
			return DK_GRAY; break;
		case 2:
			return LT_GRAY; break;
		case 3:
			return BLACK; break;
		default:
			return WHITE; break;
	}
}

uint16_t Draw::GetBackgroundTileMapLocation() {
	uint8_t val = memory->get(LCDC);
	return (((val & (1 << 3)) == 0) ? 0x9800 : 0x9c00);
}
uint16_t Draw::GetWindowTileMapLocation() {
	uint8_t val = memory->get(LCDC);
	return (((val & (1 << 6)) == 0) ? 0x9800 : 0x9c00);
}
uint16_t Draw::BGWindowTileLocation() {
	uint8_t val = memory->get(LCDC);
	return (((val & (1 << 4)) == 0) ? 0x9000 : 0x8000);
}
bool Draw::GetWindowEnabled() {
	uint8_t val = memory->get(LCDC);
	return (val & (1 << 5)) == 1;

}
void Draw::GetSpriteByNumber(uint8_t spriteNum, Sprite* sprite) {
	uint16_t address = 0xfe00 + (spriteNum * 4); // Start address of sprite data
	if (address == 0xfe04) {
		int x = 1;
	}
	if (spriteNum == 0 && memory->ReadMem(address) == 0x10) {
		int y = 1; 
	}
	uint8_t attributes = memory->ReadMem(address + 3);
	uint8_t spriteMode = (memory->ReadMem(LCDC) & 0x4) > 0 ? SPRITE_MODE_8x16 : SPRITE_MODE_8x8;

	sprite->Y = memory->ReadMem(address);
	sprite->X = memory->ReadMem(address + 1);
	if (spriteMode == SPRITE_MODE_8x8)
		sprite->TileNumber = memory->ReadMem(address + 2);
	else
		sprite->TileNumber = memory->ReadMem(address + 2) & 0xfe;
	sprite->Attributes = attributes;
	sprite->CGBPalette = attributes & 0x07;
	sprite->SpritePriority = (attributes & 0x80) == 0x80;
	sprite->YFlip = (attributes & 0x40) == 0x40;
	sprite->XFlip = (attributes & 0x20) == 0x20;
	sprite->GBPal = (attributes & 0x10) == 0x10;
	sprite->CGBVbank = (attributes & 0x08) == 0x08;
	sprite->Number = spriteNum + 1;
	if (address == 0xfe04 && sprite->TileNumber != 0x02) {
		int x = 1;
	}
}


void Draw::clean() {
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