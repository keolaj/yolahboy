#include "gpu.h"
#include "memory.h"
#include <cassert>
#include <bitset>

constexpr int TILES_Y = 24;
constexpr int TILES_X = 16;

Gpu::Gpu(Memory* memory, SDL_Renderer* renderer) : _memory{ memory }, _frameBuffer{ 0 }, _line{ 0 }, _clock{ 0 }, _mode{ GpuMode::OAM_ACCESS } {
	_screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	_tileScreen = SDL_CreateRGBSurface(0, 32 * 8, 64 * 8, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

	_tiles = new Tile[NUM_TILES];
	for (int i = 0; i < NUM_TILES; i++) {
		_tiles[i] = new uint8_t * [8];
		for (int y = 0; y < 8; ++y) {
			_tiles[i][y] = new uint8_t[8];
			for (int x = 0; x < 8; ++x) {
				_tiles[i][y][x] = 0;
			}
		}
	}

}
Gpu::~Gpu() {
	SDL_FreeSurface(_screen);
}

uint8_t Gpu::read8(int address) {
	return _memory->read8(address);
}
void Gpu::write8(int address, uint8_t data) {
	_memory->write8(address, data);
}

bool Gpu::writeBufferToScreen() {
	if (SDL_LockSurface(_screen) < 0) {
		return false;
	}

	for (int x = 0; x < SCREEN_WIDTH; ++x) {
		for (int y = 0; y < SCREEN_HEIGHT; ++y) {
			writePixel(_screen, x, y, _frameBuffer[x + (y * SCREEN_WIDTH)]);
		}
	}

	SDL_UnlockSurface(_screen);
	return true;
}

bool Gpu::writeTileBufferToScreen() {
	if (SDL_LockSurface(_tileScreen) < 0) {
		return false;
	}

	for (int y = 0; y < TILES_Y; ++y) {
		for (int x = 0; x < TILES_X; ++x) { // iterate through each tile
			for (int tiley = 0; tiley < TILE_HEIGHT; ++tiley) {
				for (int tilex = 0; tilex < TILE_WIDTH; ++tilex) {
					int tile = y * TILES_X + x;
					int pixelY = y * TILE_HEIGHT + tiley;
					int pixelX = x * TILE_WIDTH + tilex;
					writePixel(_tileScreen, pixelX, pixelY, createPixelFromPaletteId(_tiles[tile][tiley][tilex]));

				}
			}
		}
	}
	SDL_UnlockSurface(_tileScreen);
}

uint32_t Gpu::createPixelFromPaletteId(uint8_t id) {
	uint8_t palette = read8(BGP);
	uint8_t value = 0;
	switch (id) { // read palette and assign value for id
	case 0:
		value = (palette & 0b00000011);
		break;
	case 1:
		value = (palette & 0b00001100) >> 2;
		break;
	case 2:
		value = (palette & 0b00110000) >> 4;
		break;
	case 3:
		value = (palette & 0b11000000) >> 6;
		break;
	}
	switch (value) {
	case 0:
		return WHITE;
	case 1:
		return LIGHT;
	case 2:
		return DARK;
	case 3:
		return BLACK;
	}
}

void Gpu::writePixel(SDL_Surface* surface, int x, int y, uint32_t pixel) {
	uint32_t* const target = (uint32_t*)((uint8_t*)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel); // for some reason I need to cast to uint8
	*target = pixel;
}

void Gpu::updateTile(int address, uint8_t value) {

	int tileIndex = ((address & 0x1FFE) >> 4) & 0x01FF;
	int y = ((address & 0x1FFE) >> 1) & 7;

	uint8_t itX = 1;
	for (int x = 0; x < 8; ++x) { // higher nibble is stored in next address. could use read16 but this works
		itX = 1 << (7 - x);
		_tiles[tileIndex][y][x] = ((_memory->read8(address) & itX) ? 1 : 0) + ((_memory->read8(address + 1) & itX) ? 2 : 0);
	}

}

void Gpu::drawLine() {
	int scx = read8(SCX);
	int scy = read8(SCY);
	int control = read8(LCD_CONTROL);

	bool BGTileMapArea = (control & (1 << 3));
	bool BGTileAddressMode = !(control & (1 << 4));
	int mapAddress = (BGTileMapArea) ? 0x9C00 : 0x9800; // if bit 3 of the LCD Control registers is set we use the tilemap at 0x9C00, else use tile map at 0x9800
	mapAddress += (((_line + scy) & 0xFF) >> 3) << 5;

	int lineOffset = (scx >> 3);
	int tileX = scx & 7;
	int tileY = (_line + scy) & 7;

	int tile = read8(mapAddress + lineOffset);

	if (BGTileAddressMode && tile < 128) tile += 256;

	for (int i = 0; i < SCREEN_WIDTH; ++i) {
		uint32_t pixel = createPixelFromPaletteId(_tiles[tile][tileY][tileX]);
		_frameBuffer[_line * SCREEN_WIDTH + i] = pixel;

		++tileX;
		if (tileX == 8) {
			tileX = 0;
			lineOffset = (lineOffset + 1) & 31;
			tile = read8(mapAddress + lineOffset);
			if (BGTileAddressMode && tile < 128) tile += 256;
		}
	}

	// TODO: implements sprites
}

SDL_Surface* Gpu::getScreen() {
	return _screen;
}

void Gpu::handleOamAccess() {
	if (_clock >= 80) {
		_mode = VRAM_ACCESS;
		_clock = 0;
	}
}
void Gpu::handleVramAccess() {
	if (_clock >= 172) {
		_mode = HBLANK;
		_clock = 0;

		drawLine();
	}
}
void Gpu::handleHBlank() {
	if (_clock >= 204) {
		_clock = 0;
		_memory->write8(LY, ++_line);

		if (_line == 143) {
			uint8_t interrupt = _memory->read8(IF);
			_memory->write8(IF, interrupt | VBLANK_INTERRUPT);
			_mode = VBLANK;
			writeBufferToScreen();
			writeTileBufferToScreen();
		}
		else {
			_mode = OAM_ACCESS;
		}
	}
}
void Gpu::handleVBlank() {
	if (_clock >= 456) {
		_clock = 0;
		_memory->write8(LY, ++_line);

		if (_line > 153) {
			_mode = OAM_ACCESS;
			_line = 0;
		}
	}
}

void Gpu::step(Cycles cycles) {
	_clock += cycles.t_cycles;

	switch (_mode) {
	case OAM_ACCESS:
		handleOamAccess();
		break;

	case VRAM_ACCESS:
		handleVramAccess();
		break;

	case HBLANK:
		handleHBlank();
		break;
	case VBLANK:
		handleVBlank();
		break;
	}
}

Tile* Gpu::getTiles() {
	return _tiles;
}

SDL_Surface* Gpu::getTileScreen() {
	return _tileScreen;
}
