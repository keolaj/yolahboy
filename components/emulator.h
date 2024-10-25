#pragma once
#include "cpu2.h";
#include "memory2.h"
#include "operations.h"

typedef struct emu_ctx Emulator;
struct emu_ctx {
	Cpu* cpu;
	Memory* memory;
};

void init_emulator(Emulator* emu, const char* bootrom_path, const char* rom_path);
void destroy_emulator(Emulator* emu);
bool step(Emulator* emu);