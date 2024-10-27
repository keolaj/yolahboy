#include "gpu2.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

Gpu* create_gpu(Memory* mem) {
	Gpu* ret = (Gpu*)malloc(sizeof(Gpu));
	assert(ret != NULL && "could not initialize gpu");
	init_gpu(ret, mem);
	return ret;
}

void init_gpu(Gpu* gpu, Memory* mem) {
	gpu->mode = OAM_ACCESS;
	gpu->line = 0;
	gpu->mem = mem;
	gpu->clock = 0;
	memset(gpu->framebuffer, 0, sizeof(gpu->framebuffer));
	gpu->screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	gpu->tile_screen = SDL_CreateRGBSurface(0, 32 * 8, 64 * 8, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

	// setup Tiles array
	gpu->tiles = (Tile*)malloc(NUM_TILES * sizeof(Tile));
	if (gpu->tiles == NULL) {
		assert(false && "WTF");
	}
	for (int i = 0; i < NUM_TILES; ++i) {
		gpu->tiles[i] = (Tile)malloc(sizeof(u8*) * 8); // I will soon find out if malloc returns NULL
		for (int y = 0; y < 8; ++y) {
			gpu->tiles[i][y] = (u8*)malloc(sizeof(u8) * 8);
			for (int x = 0; x < 8; ++x) {
				gpu->tiles[i][y][x] = 0;
			}
		}
	}
}

void writePixel(SDL_Surface* surface, int x, int y, u32 pixel) {
	uint32_t* const target = (u32*)((u8*)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel); // for some reason I need to cast to uint8
	*target = pixel;
}

void write_buffer_to_screen(Gpu* gpu) {
	if (SDL_LockSurface(gpu->screen) < 0) {
		return false;
	}

	for (int x = 0; x < SCREEN_WIDTH; ++x) {
		for (int y = 0; y < SCREEN_HEIGHT; ++y) {
			writePixel(gpu->screen, x, y, gpu->framebuffer[x + (y * SCREEN_WIDTH)]);
		}
	}

	SDL_UnlockSurface(gpu->screen);
	return true;

}
void write_tile_buffer_to_screen(Gpu* gpu) {
	if (SDL_LockSurface(gpu->tile_screen) < 0) {
		return false;
	}

	for (int y = 0; y < TILES_Y; ++y) {
		for (int x = 0; x < TILES_X; ++x) { // iterate through each tile
			for (int tiley = 0; tiley < TILE_HEIGHT; ++tiley) {
				for (int tilex = 0; tilex < TILE_WIDTH; ++tilex) {
					int tile = y * TILES_X + x;
					int pixelY = y * TILE_HEIGHT + tiley;
					int pixelX = x * TILE_WIDTH + tilex;
					writePixel(gpu->tile_screen, pixelX, pixelY, createPixelFromPaletteId(gpu->mem, gpu->tiles[tile][tiley][tilex]));

				}
			}
		}
	}
	SDL_UnlockSurface(gpu->tile_screen);

}


void update_tile(Gpu* gpu, int address, u8 value) {
	int tileIndex = ((address & 0x1FFE) >> 4) & 0x01FF;
	int y = ((address & 0x1FFE) >> 1) & 7;

	uint8_t itX = 1;
	for (int x = 0; x < 8; ++x) { // higher nibble is stored in next address. could use read16 but this works
		itX = 1 << (7 - x);
		gpu->tiles[tileIndex][y][x] = ((read8(gpu->mem, address) & itX) ? 1 : 0) + ((read8(gpu->mem, address + 1) & itX) ? 2 : 0);
	}
}


u32 createPixelFromPaletteId(Memory* mem, u8 id) {
	uint8_t palette = read8(mem, BGP);
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


void draw_line(Gpu* gpu) {
	int scx = read8(gpu->mem, SCX);
	int scy = read8(gpu->mem, SCY);
	int control = read8(gpu->mem, LCD_CONTROL);

	bool BGTileMapArea = (control & (1 << 3));
	bool BGTileAddressMode = !(control & (1 << 4));
	int mapAddress = (BGTileMapArea) ? 0x9C00 : 0x9800; // if bit 3 of the LCD Control registers is set we use the tilemap at 0x9C00, else use tile map at 0x9800
	mapAddress += (((gpu->line + scy) & 0xFF) >> 3) << 5;

	int lineOffset = (scx >> 3);
	int tileX = scx & 7;
	int tileY = (gpu->line + scy) & 7;

	int tile = read8(gpu->mem, mapAddress + lineOffset);

	if (BGTileAddressMode && tile < 128) tile += 256;

	for (int i = 0; i < SCREEN_WIDTH; ++i) {
		uint32_t pixel = createPixelFromPaletteId(gpu->mem, gpu->tiles[tile][tileY][tileX]);
		gpu->framebuffer[gpu->line * SCREEN_WIDTH + i] = pixel;

		++tileX;
		if (tileX == 8) {
			tileX = 0;
			lineOffset = (lineOffset + 1) & 31;
			tile = read8(gpu->mem, mapAddress + lineOffset);
			if (BGTileAddressMode && tile < 128) tile += 256;
		}
	}

}

void destroy_gpu(Gpu* gpu) {
	for (int i = 0; i < NUM_TILES; ++i) {
		for (int y = 0; y < 8; ++y) {
			free(gpu->tiles[i][y]);
		}
		free(gpu->tiles[i]);
	}
	SDL_FreeSurface(gpu->screen);
	SDL_FreeSurface(gpu->tile_screen);
}

void handle_oam(Gpu* gpu) {
	if (gpu->clock >= 80) {
		gpu->mode = VRAM_ACCESS;
		gpu->clock = 0;
	}
}

void handle_vram(Gpu* gpu) {
	if (gpu->clock >= 172) {
		gpu->mode = HBLANK;
		gpu->clock = 0;

		draw_line(gpu);
	}
}

void handle_hblank(Gpu* gpu) {
	if (gpu->clock >= 204) {
		gpu->clock = 0;
		write8(gpu->mem, LY, ++gpu->line);

		if (gpu->line == 143) {
			u8 interrupt = read8(gpu->mem, IF);
			write8(gpu->mem, IF, interrupt | VBLANK_INTERRUPT);
			gpu->mode = VBLANK;
			write_buffer_to_screen(gpu);
			write_tile_buffer_to_screen(gpu);
		}
		else {
			gpu->mode = OAM_ACCESS;
		}
	}
}

void handle_vblank(Gpu* gpu) {
	if (gpu->clock >= 456) {
		gpu->clock = 0;
		write8(gpu->mem, LY, ++gpu->line);

		if (gpu->line > 153) {
			gpu->mode = OAM_ACCESS;
			gpu->line = 0;
		}
	}

}

void step_gpu(Gpu* gpu, u8 cycles) {
	gpu->clock += cycles;
	switch (gpu->mode) {
	case OAM_ACCESS:
		handle_oam(gpu);
		break;
	case VRAM_ACCESS:
		handle_vram(gpu);
		break;
	case HBLANK:
		handle_hblank(gpu);
		break;
	case VBLANK:
		handle_vblank(gpu);
		break;
	}
}
