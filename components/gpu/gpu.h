#pragma once
#include "../global_definitions.h"
#include "gpu_definitions.h"
#include "memory.h"


Gpu* create_gpu(Memory* mem);
int init_gpu(Gpu* gpu, Memory* mem);
void destroy_gpu(Gpu* gpu);
void step_gpu(Gpu* gpu, Memory* mem, u8 cycles);
void writePixel(SDL_Surface* surface, int x, int y, u32 pixel);
u32 createPixelFromPaletteId(u8 palette, u8 id);
