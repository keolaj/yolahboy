#pragma once
#include "global_definitions.h"
#include "gpu_definitions.h"
#include "memory2.h"


Gpu* create_gpu(Memory* mem);
int init_gpu(Gpu* gpu, Memory* mem);
void destroy_gpu(Gpu* gpu);
void step_gpu(Gpu* gpu, u8 cycles);
