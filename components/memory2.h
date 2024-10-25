#pragma once
#include <stdbool.h>
#include "global_definitions.h"

typedef struct {
	u8 bios[0x100];
	u8 memory[0x10000];
	bool in_bios;
} Memory;

Memory* create_memory(const char* bootrom_path, const char* rom_path);
void load_bootrom(Memory* mem, const char* path);
void load_rom(Memory* mem, const char* path);
u8 read8(Memory* mem, u16 address);
void write8(Memory* mem, u8 data);
u16 read16(Memory* mem, u16 address);
void write16(Memory* mem, u16 address, u16 data);