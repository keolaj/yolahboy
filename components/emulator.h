#pragma once
#include "cpu2.h";
#include "memory2.h"
#include "operations.h"
#include "gpu2.h"

typedef struct emu_ctx Emulator;
struct emu_ctx {
	Cpu* cpu;
	Memory* memory;
	Gpu* gpu;
};

void init_emulator(Emulator* emu, const char* bootrom_path, const char* rom_path);
void destroy_emulator(Emulator* emu);
bool step(Emulator* emu);