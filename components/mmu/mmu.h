#pragma once
#include <stdbool.h>
#include "../global_definitions.h"

void init_Mmu(Mmu* mem);
int load_bootrom(Mmu* mem, const char* path);
int load_rom(Mmu* mem, const char* path);
u8 read8(Emulator* emu, u16 address);
void write8(Emulator* emu, u16 address, u8 data);
u16 read16(Emulator* emu, u16 address);
void write16(Emulator* emu, u16 address, u16 data);

void destroy_Mmu(Mmu* mem);