#pragma once
#include <stdbool.h>
#include "../global_definitions.h"

Memory* create_memory();
int load_bootrom(Memory* mem, const char* path);
int load_rom(Memory* mem, const char* path);
u8 read8(Memory* mem, u16 address);
void write8(Memory* mem, u16 address, u8 data);
u16 read16(Memory* mem, u16 address);
void write16(Memory* mem, u16 address, u16 data);
void set_use_gbd_log(Memory* mem, bool use_gbd_log);

void destroy_memory(Memory* mem);