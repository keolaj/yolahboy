#include "gpu.h"
#include <stdio.h>
#include <SDL3/SDL.h>
#include "../memory/memory.h"

Gpu* create_gpu(Memory* mem) {
	Gpu* ret = (Gpu*)malloc(sizeof(Gpu));
	if (ret == NULL) {
		printf("could not allocate Gpu");
		return NULL;
	}
	if (init_gpu(ret, mem) == -1) {
		printf("could not initialize Gpu");
		return NULL;
	}
	return ret;
}

int init_gpu(Gpu* gpu, Memory* mem) {
	gpu->mode = OAM_ACCESS;
	gpu->line = 0;
	gpu->mem = mem;
	gpu->clock = 0;
	memset(gpu->framebuffer, 0, sizeof(gpu->framebuffer));
	gpu->screen = SDL_CreateSurface(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_PIXELFORMAT_ARGB32);
	gpu->tile_screen = SDL_CreateSurface(TILES_X * TILE_WIDTH, TILES_Y * TILE_HEIGHT, SDL_PIXELFORMAT_ARGB32);
	gpu->drawline = false;

	// setup Tiles array
	gpu->tiles = (Tile*)malloc(NUM_TILES * sizeof(Tile));
	if (gpu->tiles == NULL) {
		printf("could not allocate Tile");
		return -1;
	}
	for (int i = 0; i < NUM_TILES; ++i) {
		gpu->tiles[i] = (Tile)malloc(sizeof(u8*) * 8);
		if (gpu->tiles[i] == NULL) {
			printf("unable to allocate tile array");
			free(gpu->tiles);
			return -1;
		}
		for (int y = 0; y < 8; ++y) {
			gpu->tiles[i][y] = (u8*)malloc(sizeof(u8) * 8);
			if (gpu->tiles[i][y] == NULL) {
				printf("unable to allocate tile");
				free(gpu->tiles[i]);
				free(gpu->tiles);
				return -1;
			}
			for (int x = 0; x < 8; ++x) {
				gpu->tiles[i][y][x] = 0;
			}
		}
	}
	return 0;
}

u32 createPixelFromPaletteId(u8 palette, u8 id) {
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
	default:
		value = 0;
		printf("createPixelFromPalette bad value");
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
	default:
		printf("invalid color");
		return 0;
	}
}

void writePixel(SDL_Surface* surface, int x, int y, u32 pixel) {
	uint32_t* const target = (u32*)((u8*)surface->pixels + y * surface->pitch + x * SDL_GetPixelFormatDetails(surface->format)->bytes_per_pixel); // for some reason I need to cast to uint8
	*target = pixel;
}

void write_buffer_to_screen(Gpu* gpu) {
	if (SDL_LockSurface(gpu->screen) < 0) {
		printf("could not lock screen surface");
		return;
	}

	for (int x = 0; x < SCREEN_WIDTH; ++x) {
		for (int y = 0; y < SCREEN_HEIGHT; ++y) {
			writePixel(gpu->screen, x, y, gpu->framebuffer[x + (y * SCREEN_WIDTH)]);
		}
	}

	SDL_UnlockSurface(gpu->screen);
}
void write_tile_buffer_to_screen(Gpu* gpu) {
	if (SDL_LockSurface(gpu->tile_screen) < 0) {
		printf("could not lock tile screen");
		return;
	}

	for (int y = 0; y < TILES_Y; ++y) {
		for (int x = 0; x < TILES_X; ++x) { // iterate through each tile
			for (int tiley = 0; tiley < TILE_HEIGHT; ++tiley) {
				for (int tilex = 0; tilex < TILE_WIDTH; ++tilex) {
					int tile = y * TILES_X + x;
					int pixelY = y * TILE_HEIGHT + tiley;
					int pixelX = x * TILE_WIDTH + tilex;
					writePixel(gpu->tile_screen, pixelX, pixelY, createPixelFromPaletteId(read8(gpu->mem, BGP), gpu->tiles[tile][tiley][tilex]));
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
		// ((read8(gpu->mem, address) & itX) ? 1 : 0) + ((read8(gpu->mem, address + 1) & itX) ? 2 : 0);
		u8 tile_id = ((gpu->mem->memory[address] & itX) ? 1 : 0) + ((gpu->mem->memory[address + 1] & itX) ? 2 : 0);
		gpu->tiles[tileIndex][y][x] = tile_id;
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

	// if (BGTileAddressMode && tile < 128) tile += 256;

	for (int i = 0; i < SCREEN_WIDTH; ++i) {
		uint32_t pixel = createPixelFromPaletteId(read8(gpu->mem, BGP), gpu->tiles[tile][tileY][tileX]);
		gpu->framebuffer[gpu->line * SCREEN_WIDTH + i] = pixel;

		++tileX;
		if (tileX == 8) {
			tileX = 0;
			lineOffset = (lineOffset + 1) & 31;
			tile = read8(gpu->mem, mapAddress + lineOffset);
			if (BGTileAddressMode && tile < 128) tile += 256;
		}
	}

	// draw sprites
	bool eight_by_16_mode = read8(gpu->mem, LCD_CONTROL) & (1 << 2);
	u16 current_OAM_address = 0xFE00;
	int sprite_count_for_line = 0;

	while (current_OAM_address <= 0xFE9F && sprite_count_for_line < 10) {
		u8 sprite_pos_y = read8(gpu->mem, current_OAM_address);
		u8 sprite_pos_x = read8(gpu->mem, current_OAM_address + 1);
		u8 tile_index = read8(gpu->mem, current_OAM_address + 2);
		u8 attributes = read8(gpu->mem, current_OAM_address + 3);

		int sprite_bottom = sprite_pos_y - 16 + (eight_by_16_mode ? 16 : 8);
		int sprite_top = sprite_pos_y - 16;
		// draw sprite
		if (gpu->line < sprite_bottom && gpu->line >= sprite_top) { // if current line within sprite (i think this is right, no way to check until I implement OAM dma and HBLANK interrupt)
			++sprite_count_for_line;
			int sprite_line = gpu->line - sprite_top;

			if (sprite_line >= 8) { // address next tile if we are in 16 bit mode
				sprite_line -= 8;
				tile_index += 1;
			}

			bool palette_mode = attributes & (1 << 4);
			u8 palette = read8(gpu->mem, (palette_mode ? OBP1 : OBP0));

			for (int i = 0; i < 8; ++i) {
				if (sprite_pos_x - 8 + i < 0) continue;
				u32 pixel = createPixelFromPaletteId(palette, gpu->tiles[tile_index][sprite_line][i]);
				gpu->framebuffer[gpu->line * SCREEN_WIDTH + sprite_pos_x - 8 + i] = pixel;
			}
		}
		current_OAM_address += 4;
	}

}

void destroy_gpu(Gpu* gpu) {
	if (gpu == NULL) {
		return;
	}
	for (int i = 0; i < NUM_TILES; ++i) {
		for (int y = 0; y < 8; ++y) {
			free(gpu->tiles[i][y]);
		}
		free(gpu->tiles[i]);
	}
	SDL_DestroySurface(gpu->screen);
	SDL_DestroySurface(gpu->tile_screen);
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
		if (read8(gpu->mem, LCD_STATUS) & (1 << 3)) {
			write8(gpu->mem, IF, read8(gpu->mem, IF) | LCDSTAT_INTERRUPT);
		}
	}
}

void handle_hblank(Gpu* gpu) {
	if (gpu->clock >= 204) {
		gpu->clock = 0;
		++gpu->line;
		write8(gpu->mem, LY, gpu->line);
		if (read8(gpu->mem, LYC) == gpu->line) {
			write8(gpu->mem, LCD_STATUS, read8(gpu->mem, LCD_STATUS) | (1 << 2));
			if (read8(gpu->mem, LCD_STATUS) & (1 << 6)) {
				write8(gpu->mem, IF, read8(gpu->mem, IF) | LCDSTAT_INTERRUPT);
			}
		}
		else {
			write8(gpu->mem, LCD_STATUS, read8(gpu->mem, LCD_STATUS) & ~(1 << 2));
		}



		if (gpu->line == 143) {
			u8 interrupt = read8(gpu->mem, IF);
			write8(gpu->mem, IF, interrupt | VBLANK_INTERRUPT);
			gpu->mode = VBLANK;
			write_buffer_to_screen(gpu);
			write_tile_buffer_to_screen(gpu);
			if (read8(gpu->mem, LCD_STATUS) & (1 << 4)) {
				write8(gpu->mem, IF, read8(gpu->mem, IF) | LCDSTAT_INTERRUPT);
			}

		}
		else {
			gpu->mode = OAM_ACCESS;
			if (read8(gpu->mem, LCD_STATUS) & (1 << 5)) {
				write8(gpu->mem, IF, read8(gpu->mem, IF) | LCDSTAT_INTERRUPT);
			}
		}
	}
}

void handle_vblank(Gpu* gpu) {
	if (gpu->clock >= 456) {
		gpu->clock = 0;
		++gpu->line;
		write8(gpu->mem, LY, gpu->line);

		if (gpu->line > 153) {
			gpu->mode = OAM_ACCESS;
			gpu->line = 0;
			write8(gpu->mem, LY, 0);
			if (read8(gpu->mem, LCD_STATUS) & (1 << 5)) {
				write8(gpu->mem, IF, read8(gpu->mem, IF) | LCDSTAT_INTERRUPT);
			}
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
