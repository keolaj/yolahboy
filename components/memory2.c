#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "memory2.h"
#include <string.h>

Memory* create_memory(const char* bootrom_path, const char* rom_path) {
	Memory* ret = (Memory*)malloc(sizeof(Memory));
	assert(ret != NULL && "Cannot allocate Memory");
	load_bootrom(ret, bootrom_path);
	load_rom(ret, rom_path);
	return ret;
}



void set_gpu(Memory* mem, Gpu* gpu) {
	mem->gpu = gpu;
}

u8 read8(Memory* mem, u16 address) {
	if (address < 0x100) {
		if (mem->in_bios) {
			return mem->bios[(u8)address];
		}
	}
	if (address == 0xFF00) return 0xFF; // joypad emulation for now
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

	if (address)

		mem->memory[address] = data;

	if (address > 0x8000 && address <= 0x97FF) {
		if (mem->in_bios) update_tile(mem->gpu, address, data);
		if (address % 2 != 0) update_tile(mem->gpu, address - 1, data);
	}
	if (address == IE) {
		printf("IE write: 0x%02X\n", data);
	}
}

void load_bootrom(Memory* mem, const char* path) {
	FILE* fp;
	fp = fopen(path, "rb");
	assert(fp != NULL && "error opening bootrom");
	fread(mem->bios, sizeof(u8), 0x100, fp);
}
void load_rom(Memory* mem, const char* path) {
	FILE* fp;
	fp = fopen(path, "rb");
	assert(fp != NULL && "error opening rom");
	memset(mem->memory, 0, sizeof(mem->memory));
	fread(mem->memory, sizeof(u8), 0x8000, fp);
}

