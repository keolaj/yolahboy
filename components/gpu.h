#pragma once

#include "cpu_definitons.h"
#include "SDL.h"
#include "gpu_definitions.h"
#include "global_definitions.h"

using Tile = uint8_t**;

constexpr int NUM_TILES = 0x180;
constexpr int TILE_WIDTH = 8;
constexpr int TILE_HEIGHT = 8;

class Memory;

class Gpu {
private:
	Memory* _memory;
	GpuMode _mode;
	int _line;
	int _clock;
	uint32_t _frameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
	// uint32_t _tileFrameBuffer[TILE_WIDTH * TILE_HEIGHT * NUM_TILES];
	Tile* _tiles;
	SDL_Surface* _screen;
	SDL_Surface* _tileScreen;
	bool writeBufferToScreen();
	bool writeTileBufferToScreen();
	void writePixel(SDL_Surface* surface, int x, int y, uint32_t pixel);
	uint32_t createPixelFromPaletteId(uint8_t id);
	void drawLine();
	void handleHBlank();
	void handleVBlank();
	void handleOamAccess();
	void handleVramAccess();
	uint8_t read8(int address);
	uint16_t read16(int address);
	void write8(int address, uint8_t data);
	void write16(int address, uint16_t data);
public:
	Gpu(Memory* memory, SDL_Renderer* renderer);
	~Gpu();
	void step(Cycles cycles);
	SDL_Surface* getScreen();
	SDL_Surface* getTileScreen();
	void updateTile(int address, uint8_t value);
	Tile* getTiles();
};