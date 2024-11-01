#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "memory2.h"
#include "controller.h"

Memory* create_memory(const char* bootrom_path, const char* rom_path) {
	Memory* ret = (Memory*)malloc(sizeof(Memory));
	assert(ret != NULL && "Cannot allocate Memory");
	load_bootrom(ret, bootrom_path);
	load_rom(ret, rom_path);
	memset((void*)&ret->controller, 0, sizeof(Controller));
	return ret;
}

void set_gpu(Memory* mem, Gpu* gpu) {
	mem->gpu = gpu;
}
void set_mem_controller(Memory* mem, Controller controller) {
	mem->controller = controller;
}

u8 read8(Memory* mem, u16 address) {
	if (address < 0x100) {
		if (mem->in_bios) {
			return mem->bios[(u8)address];
		}
	}
	if (address == 0xFF00) {
		u8 j_ret = joypad_return(mem->controller, mem->memory[address]);
		return j_ret; // joypad emulation for now
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

void update_tile(Gpu* gpu, int address, u8 value);

void write8(Memory* mem, u16 address, u8 data) {
	if (address < 0x8000) {
		return;
	}
	mem->memory[address] = data;

	if (address > 0x8000 && address <= 0x97FF) {
		if (mem->in_bios) update_tile(mem->gpu, address, data);
		if (address % 2 != 0) update_tile(mem->gpu, address - 1, data);
	}
	if (address == DMA && data <= 0xDF) {
		for (int i = 0; i < 0x100; ++i) {
			mem->memory[0xFE00 + i] = mem->memory[(data << 8) + i];
		}
	}
	if (address == 0xFF02 && data == 0x81) {
		printf("%c", read8(mem, 0xff01));
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

