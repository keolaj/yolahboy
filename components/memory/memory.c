#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "memory.h"
#include "../gpu/gpu.h"
#include "../controller/controller.h"
#include "../debugger/imgui_custom_widget_wrapper.h"
#include "./cartridge.h"

Memory* create_memory() {
	Memory* ret = (Memory*)malloc(sizeof(Memory));
	if (ret == NULL) {
		printf("could not allocate memory");
		return NULL;
	}
	ret->cartridge.rom = NULL;
	ret->cartridge.ram = NULL;
	ret->cartridge.rom_bank = 1;
	ret->cartridge.ram_bank = 0;
	ret->cartridge.banking_mode = BANKMODESIMPLE;
	ret->cartridge.ram_enabled = false;
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
	if (mem->in_bios) {
		if (address < 0x100) {
			return mem->bios[address];
		}
	}
	if (address < 0x8000) { // cartridge rom
		return cart_read8(&mem->cartridge, address);
	}
	if (address >= 0xA000 && address < 0xC000) { // cartridge ram
		return cart_read8(&mem->cartridge, address);
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
		return cart_write8(&mem->cartridge, address, data);
	}
	mem->memory[address] = data;

	if (address >= 0x8000 && address <= 0x97FF) {
		if (address % 2 == 0) update_tile(mem->gpu, address, data);
		if (address % 2 != 0) update_tile(mem->gpu, address - 1, data);
	}
	if (address >= 0xA000 && address < 0xC000) { // cartridge ram
		cart_write8(&mem->cartridge, address, data);
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

int load_rom(Memory* mem, const char* path) { // I believe this is working
	FILE* fp;
	fp = fopen(path, "rb");
	if (fp == NULL) {
		AddLog("error opening rom");
		return -1;
	}
	memset(mem->memory, 0, sizeof(mem->memory));

	u8 cart_type_val;
	if (fseek(fp, 0x147, SEEK_SET) != 0) {
		AddLog("Error seeking to Cart Type!");
		fclose(fp);
		return -1;
	}
	fread(&cart_type_val, sizeof(u8), 1, fp);
	rewind(fp);

	mem->cartridge.type = cart_type_val;

	u8 rom_size_val; // read rom file into mem->cartridge.rom
	if (fseek(fp, 0x148, SEEK_SET) != 0) {
		AddLog("error seeking to position 0x148");
		fclose(fp);
		return -1;
	}
	if (fread(&rom_size_val, sizeof(u8), 1, fp) != 1) {
		AddLog("couldnt read to rom_size_val");
		fclose(fp);
		return -1;
	}
	rewind(fp);

	int rom_size = (BANKSIZE * 2) * (1 << rom_size_val);
	mem->cartridge.rom_size = rom_size;
	mem->cartridge.num_rom_banks = rom_size / BANKSIZE;
	mem->cartridge.rom = (u8*)malloc(sizeof(u8) * rom_size);
	if (mem->cartridge.rom == NULL) {
		AddLog("could not allocate cartridge rom");
		return -1;
	}
	fread(mem->cartridge.rom, sizeof(u8), rom_size, fp);
	rewind(fp);

	u8 ram_size_val;
	fseek(fp, 0x149, SEEK_SET);
	fread(&ram_size_val, sizeof(u8), 1, fp);
	rewind(fp);
	int actual_ram_size;
	switch (ram_size_val) {
	case 0:
		actual_ram_size = 0;
		break;
	case 1:
		actual_ram_size = 0;
		break;
	case 2:
		actual_ram_size = 0x8000;
		break;
	case 3:
		actual_ram_size = 0x8000 * 4;
		break;
	case 4:
		actual_ram_size = 0x8000 * 16;
		break;
	case 5:
		actual_ram_size = 0x8000 * 8;
		break;
	default:
		actual_ram_size = 0;
		AddLog("bad read from cartridge ram size");
		break;
	}
	mem->cartridge.ram_size = actual_ram_size;
	if (actual_ram_size) {
		mem->cartridge.ram = (u8*)malloc(sizeof(u8) * actual_ram_size);
		if (mem->cartridge.ram == NULL) {
			AddLog("couldn't allocate cartridge ram!");
			return -1;
		}
		memset(mem->cartridge.ram, 0, actual_ram_size);
	}
	fclose(fp);
	return 0;
}

void destroy_memory(Memory* mem) {
	if (mem == NULL) return;
	
	if (mem->cartridge.rom) free(mem->cartridge.rom);
	if (mem->cartridge.ram) free(mem->cartridge.ram);
	free(mem);
}

void set_use_gbd_log(Memory* mem, bool use_gbd_log) {
	mem->use_gbd_log = use_gbd_log;
}