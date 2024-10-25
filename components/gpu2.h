#pragma once
#include "global_definitions.h"
#include "gpu_definitions.h"
#include "SDL.h"

typedef u8** Tile;

typedef struct {
	u8 line;
	int clock;
	u32 framebuffer[23040];
	Tile* tiles;
	SDL_Surface* screen;
	SDL_Surface* tile_screen;
} Gpu;

Gpu* create_gpu();
void init_gpu(Gpu* gpu);
void step_gpu(Gpu* gpu);
