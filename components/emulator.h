#pragma once

#include "cpu2.h"
#include "memory2.h"
#include "operations.h"
#include "gpu2.h"
#include "controller.h"

typedef struct {
	Cpu* cpu;
	Memory* memory;
	Gpu* gpu;
	Controller* controller;
} Emulator;

void update_emu_controller(Emulator* emu, Controller controller);
int init_emulator(Emulator* emu, const char* bootrom_path, const char* rom_path);
void destroy_emulator(Emulator* emu);
bool step(Emulator* emu);