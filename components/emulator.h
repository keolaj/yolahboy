#pragma once
#include "cpu2.h";
#include "memory2.h"
#include "operations.h"
#include "gpu2.h"

typedef struct {
	Cpu* cpu;
	Memory* memory;
	Gpu* gpu;
} Emulator;

void init_emulator(Emulator* emu, const char* bootrom_path, const char* rom_path);
void destroy_emulator(Emulator* emu);
bool step(Emulator* emu);