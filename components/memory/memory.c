#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "memory.h"
#include "../controller/controller.h"
#include "../debugger/imgui_custom_widget_wrapper.h"

Memory* create_memory() {
	Memory* ret = (Memory*)malloc(sizeof(Memory));
	if (ret == NULL) {
		printf("could not allocate memory");
		return NULL;
	}
	ret->cartridge.rom = NULL;
	ret->cartridge.ram = NULL;
	memset((void*)&ret->controller, 0, sizeof(Controller));
	memset(ret->memory, 0, 0x10000);
	memset(ret->bios, 0, 0x100);
	ret->in_bios = true;
	return ret;
}

void set_gpu(Memory* mem, Gpu* gpu) {
	mem->gpu = gpu;
}
void set_mem_controller(Memory* mem, Controller* controller) {
	mem->controller = controller;
}

u8 read8(Memory* mem, u16 address) {
	if (address < 0x100) {
		if (mem->in_bios) {
			return mem->bios[(u8)address];
		}
		else {
			return mem->memory[address];
		}
	}
	if (address == 0xFF00) {
		u8 j_ret = joypad_return(*mem->controller, mem->memory[address]);
		return j_ret; 
	}
	if (address == 0xFF44 && mem->use_gbd_log) return 0x90; 
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

	if (address >= 0x8000 && address <= 0x97FF) {
		if (address % 2 == 0) update_tile(mem->gpu, address, data);
		if (address % 2 != 0) update_tile(mem->gpu, address - 1, data);
	}
	if (address == DMA && data <= 0xDF) {
		for (int i = 0; i < 0x100; ++i) {
			mem->memory[0xFE00 + i] = mem->memory[(data << 8) + i];
		}
	}
	if (address == 0xFF02 && data == 0x81) {
		AddLog("%c", read8(mem, 0xff01));
	}
	if (address == 0xFF04) mem->memory[address] = 0;

}

int load_bootrom(Memory* mem, const char* path) {
	FILE* fp;
	fp = fopen(path, "rb");
	if (fp == NULL) {
		AddLog("error opening bootrom");
		return -1;
	}
	fread(mem->bios, sizeof(u8), 0x100, fp);
	return 0;
}
int load_rom(Memory* mem, const char* path) {
	FILE* fp;
	fp = fopen(path, "rb");
	if (fp == NULL) {
		AddLog("error opening rom");
		return -1;
	}
	memset(mem->memory, 0, sizeof(mem->memory));
	fread(mem->memory, sizeof(u8), 0x8000, fp);

	mem->cartridge.rom = (u8*)malloc(sizeof(u8) * 0x8000);
	if (mem->cartridge.rom == NULL) {
		AddLog("could not allocate cartridge rom");
		return -1;
	}
	fread(mem->cartridge.rom, sizeof(u8), 0x8000, fp);
	fclose(fp);
	return 0;
}

void destroy_memory(Memory* mem) {
	if (mem == NULL) return;
	
	if (mem->cartridge.rom) free(mem->cartridge.rom);
	free(mem);
}

void write_mem_layout_to_buffer(u8* buffer) {

}

void set_use_gbd_log(Memory* mem, bool use_gbd_log) {
	mem->use_gbd_log = use_gbd_log;
}