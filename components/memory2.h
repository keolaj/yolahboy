#pragma once
#include <stdbool.h>
#include "memory_definitions.h"
#include "gpu_definitions.h"


Memory* create_memory(const char* bootrom_path, const char* rom_path);
int load_bootrom(Memory* mem, const char* path);
int load_rom(Memory* mem, const char* path);
u8 read8(Memory* mem, u16 address);
void write8(Memory* mem, u16 address, u8 data);
u16 read16(Memory* mem, u16 address);
void write16(Memory* mem, u16 address, u16 data);
void set_gpu(Memory* mem, Gpu* gpu);
void set_mem_controller(Memory* mem, Controller controller);

void destroy_memory(Memory* mem);