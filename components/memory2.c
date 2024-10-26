#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "memory2.h"

Memory* create_memory(const char* bootrom_path, const char* rom_path) {
	Memory* ret = (Memory*)malloc(sizeof(Memory));
	assert(ret != NULL && "Cannot allocate Memory");
	load_bootrom(ret, bootrom_path);
	load_rom(ret, rom_path);
	return ret;
}

u8 read8(Memory* mem, u16 address) {
	if (address < 0x100) {
		if (mem->in_bios) {
			return mem->bios[(u8)address];
		}
	}
	return mem->memory[address];
}

u16 read16(Memory* mem, u16 address) {
	u16 ret = 0;
	ret |= read8(mem, address);
	ret |= read8(mem, address + 1) << 8;
	return ret;
}

void write16(Memory* mem, u16 address, u16 value) {
	write8(mem, address, value & 0xFF);
	write8(mem, address + 1, value >> 8);
}

void write8(Memory* mem, u16 address, u8 data) {
	mem->memory[address] = data;
}

void load_bootrom(Memory* mem, const char* path) {
	FILE* fp;
	fp = fopen(path, "r+b");
	assert(fp != NULL && "error opening bootrom");
	fread(mem->bios, sizeof(u8), 0x100, fp);
}
void load_rom(Memory* mem, const char* path) {
	FILE* fp;
	fp = fopen(path, "r+b");
	assert(fp != NULL && "error opening rom");
	fread(mem->memory, sizeof(u8), 0x8000, fp);
}

