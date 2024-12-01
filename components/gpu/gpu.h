#pragma once
#include "../global_definitions.h"
#include "gpu_definitions.h"
#include "../mmu/mmu.h"


Gpu* create_gpu(Mmu* mem);
int init_gpu(Gpu* gpu);
void destroy_gpu(Gpu* gpu);
void gpu_step(Gpu* gpu, Mmu* mem, u8 cycles);
u8 read_tile(Mmu* mem, int tile_index, u8 x, u8 y);
u32 pixel_from_palette(u8 palette, u8 id);
