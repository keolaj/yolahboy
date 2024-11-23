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
	if (init_gpu(ret) == -1) {
		printf("could not initialize Gpu");
		return NULL;
	}
	return ret;
}

int init_gpu(Gpu* gpu) {
	memset(gpu, 0, sizeof(Gpu));

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

u8 createIdFromAddress(Gpu* gpu, Memory* mem, u16 address) {
	
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

void update_tile(Gpu* gpu, Memory* mem, int address, u8 value) {
	int tileIndex = ((address & 0x1FFF) >> 4) & 0x01FF;
	int y = ((address & 0x1FFF) >> 1) & 7;

	uint8_t itX = 1;
	for (int x = 0; x < 8; ++x) { // higher nibble is stored in next address.
		itX = 1 << (7 - x);
		u8 tile_id = ((mem->memory[address] & itX) ? 1 : 0) + ((mem->memory[address + 1] & itX) ? 2 : 0);
		gpu->tiles[tileIndex][y][x] = tile_id;
	}
}



void draw_line(Gpu* gpu, Memory* mem) {

	bool BGTileMapArea = (gpu->lcdc & (1 << 3));
	bool BGTileAddressMode = !(gpu->lcdc & (1 << 4));
	int mapAddress = (BGTileMapArea) ? 0x9C00 : 0x9800; // if bit 3 of the LCD Control registers is set we use the tilemap at 0x9C00, else use tile map at 0x9800
	mapAddress += (((gpu->ly + gpu->scy) & 0xFF) >> 3) << 5;

	int lineOffset = (gpu->scx >> 3);
	int tileX = gpu->scx & 7;
	int tileY = (gpu->ly + gpu->scy) & 7;
	int tile = read8(mem, mapAddress + lineOffset);

	if (BGTileAddressMode && tile < 128) tile += 256;

	if (gpu->lcdc & 1) {
		for (int i = 0; i < SCREEN_WIDTH; ++i) {
			gpu->framebuffer[gpu->ly * SCREEN_WIDTH + i] = createPixelFromPaletteId(read8(mem, BGP), gpu->tiles[tile][tileY][tileX]);
			++tileX;
			if (tileX == 8) {
				tileX = 0;
				lineOffset = (lineOffset + 1) & 31;
				tile = read8(mem, mapAddress + lineOffset);
				if (BGTileAddressMode && tile < 128) tile += 256;
			}
		}

	}

	// draw window
	u8 windowx = gpu->wx - 7;
	u8 windowy = gpu->wy;
	if (gpu->lcdc & (1 << 5) && gpu->ly >= windowy) { // draw window
		BGTileMapArea = (gpu->lcdc & (1 << 6));
		BGTileAddressMode = !(gpu->lcdc & (1 << 4));
		mapAddress = (BGTileMapArea) ? 0x9C00 : 0x9800; // if bit 3 of the LCD Control registers is set we use the tilemap at 0x9C00, else use tile map at 0x9800
		mapAddress += (((gpu->ly + windowy) & 0xFF) >> 3) << 5;

		if (BGTileAddressMode && tile < 128) tile += 256;

		int lineOffset = (windowx >> 3);
		int tileX = windowx & 7;
		int tileY = (gpu->ly + windowy) & 7;
		int tile = read8(mem, mapAddress + lineOffset);

		for (int i = 0; i < SCREEN_WIDTH; ++i) {
			if (windowx < i) continue;
			uint32_t window_pixel = createPixelFromPaletteId(read8(mem, BGP), gpu->tiles[tile][tileY][tileX]);
			gpu->framebuffer[gpu->ly * SCREEN_WIDTH + i] = window_pixel;

			++tileX;
			if (tileX == 8) {
				tileX = 0;
				lineOffset = (lineOffset + 1) & 31;
				tile = read8(mem, mapAddress + lineOffset);
				if (BGTileAddressMode && tile < 128) tile += 256;
			}
		}


	}

	// draw sprites
	if (gpu->lcdc & (1 << 1)) { // if sprites enabled in LDC Control
		bool eight_by_16_mode = gpu->lcdc & (1 << 2);
		u16 current_OAM_address = 0xFE00;
		int sprite_count_for_line = 0;

		while (current_OAM_address <= 0xFE9F && sprite_count_for_line < 10) {
			if (sprite_count_for_line > 10) break;
			u8 sprite_pos_y = read8(mem, current_OAM_address);
			u8 sprite_pos_x = read8(mem, current_OAM_address + 1);
			u8 tile_index = read8(mem, current_OAM_address + 2);
			u8 attributes = read8(mem, current_OAM_address + 3);
			bool palette_mode = attributes & (1 << 4);
			bool x_flip = attributes & (1 << 5);
			bool y_flip = attributes & (1 << 6);
			bool priority = attributes & (1 << 7);


			int sprite_bottom = sprite_pos_y - 16 + (eight_by_16_mode ? 16 : 8);
			int sprite_top = sprite_pos_y - 16;
			if (gpu->ly < sprite_bottom && gpu->ly >= sprite_top) { // if current line within sprite 
				++sprite_count_for_line; // 10 sprites a line
				if (eight_by_16_mode) tile_index &= 0xFE; // make sure we aren't indexing out of where we should be
				u8 sprite_line = gpu->ly - sprite_top; // get current line of sprite
				if (y_flip) {
					if (eight_by_16_mode) {
						if (sprite_line >= 8) { // address next tile if we are in 16 bit mode
							sprite_line = 15 - sprite_line;
						}
						else {
							sprite_line = 7 - sprite_line;
							tile_index++;
						}
					}
					else {
						sprite_line = 7 - sprite_line;
					}
				}
				else {
					if (sprite_line >= 8) {
						sprite_line -= 8;
						tile_index++;
					}
				}
				u8 palette = read8(mem, (palette_mode ? OBP1 : OBP0));
				for (int i = 0; i < 8; ++i) {
					if (sprite_pos_x - 8 + i < 0) continue;
					u8 id = gpu->tiles[tile_index][sprite_line][i];
					if (id == 0) continue;
					u32 pixel = createPixelFromPaletteId(palette, id);
					int x = i;
					if (x_flip) x = 7 - i;
					gpu->framebuffer[gpu->ly * SCREEN_WIDTH + sprite_pos_x - 8 + x] = pixel;
				}
			}
			current_OAM_address += 4;
		}
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
}

void handle_oam(Gpu* gpu, Memory* mem) {
	if (gpu->should_stat_interrupt && gpu->clock > 4) {
		write8(mem, IF, read8(mem, IF) | STAT_INTERRUPT);
		gpu->should_stat_interrupt = false;
	}
	if (gpu->clock >= 80) {
		gpu->clock -= 80;
		gpu->mode = VRAM_ACCESS;
		write8(mem, IF, (read8(mem, IF) & ~(STAT_INTERRUPT))); // reset STAT interrupt on mode 3 enter. There is no stat interrupt for mode 3
	}
}

void handle_vram(Gpu* gpu, Memory* mem) {
	if (gpu->clock >= 172) {
		gpu->clock -= 172;
		gpu->mode = HBLANK;
		draw_line(gpu, mem);
		if (gpu->stat & (1 << 3)) {
			gpu->should_stat_interrupt = true;
		}
	}
}

void handle_hblank(Gpu* gpu, Memory* mem) {
	if (gpu->should_stat_interrupt && gpu->clock > 4) {
		write8(mem, IF, read8(mem, IF) | STAT_INTERRUPT);
		gpu->should_stat_interrupt = false;
	}
	if (gpu->clock >= 204) {
		gpu->clock -= 204;
		++gpu->ly;
		if (gpu->lyc == gpu->ly) {
			gpu->stat |= (1 << 2);
			if (gpu->stat & (1 << 6)) {
				gpu->should_stat_interrupt = true;
			}
		}
		else {
			gpu->stat &= ~(1 << 2);
		}

		if (gpu->ly == 143) {
			u8 interrupt = read8(mem, IF);
			write8(mem, IF, interrupt | VBLANK_INTERRUPT);
			gpu->mode = VBLANK;
			if (gpu->stat & (1 << 4)) {
				gpu->should_stat_interrupt = true;
			}

		}
		else {
			gpu->mode = OAM_ACCESS;
			if (read8(mem, STAT) & (1 << 5)) {
				gpu->should_stat_interrupt = true;
			}
		}
	}
}

void handle_vblank(Gpu* gpu, Memory* mem) {
	if (gpu->should_stat_interrupt && gpu->clock > 4) {
		write8(mem, IF, read8(mem, IF) | STAT_INTERRUPT);
		gpu->should_stat_interrupt = false;
	}
	if (gpu->clock >= 456) {
		gpu->clock -= 456;
		++gpu->ly;
		mem->memory[LY] = gpu->ly;
		if (mem->memory[LYC] == gpu->ly) {
			gpu->stat |= (1 << 2);
			if (read8(mem, STAT) & (1 << 6)) {
				mem->memory[IF] |= STAT_INTERRUPT;
			}
		}
		else {
			gpu->stat &= ~(1 << 2);
		}


		if (gpu->ly > 153) {			
			gpu->should_draw = true;
			gpu->mode = OAM_ACCESS;
			gpu->ly = 0;
			if (gpu->lyc == gpu->ly) {
				gpu->stat |= (1 << 2);
				if (read8(mem, STAT) & (1 << 6)) {
					gpu->should_stat_interrupt = true;
					// mem->memory[IF] |= STAT_INTERRUPT;
				}
			}
			else {
				gpu->stat &= ~(1 << 2);
			}

			if (read8(mem, STAT) & (1 << 5)) { // mode 2 interrupt select
				write8(mem, IF, read8(mem, IF) | STAT_INTERRUPT);
			}
		}
	}

}

void step_gpu(Gpu* gpu, Memory* mem, u8 cycles) {
	bool lcd_enabled = gpu->lcdc & (1 << 7);
	gpu->should_draw = false;
	if (!lcd_enabled) {
		return;
	}
	gpu->clock += cycles;
	switch (gpu->mode) {
	case OAM_ACCESS:
		handle_oam(gpu, mem);
		break;
	case VRAM_ACCESS:
		handle_vram(gpu, mem);
		break;
	case HBLANK:
		handle_hblank(gpu, mem);
		break;
	case VBLANK:
		handle_vblank(gpu, mem);
		break;
	}
	gpu->stat = (gpu->stat & 0b11111100) | (gpu->mode & 0b00000011);
}
