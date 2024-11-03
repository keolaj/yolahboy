#pragma once

#include "controller.h"

typedef struct {
	Cpu* cpu;
	Memory* memory;
	Gpu* gpu;
	Controller* controller;
	u16* breakpoints;
	bool should_quit;
} Emulator;

void update_emu_controller(Emulator* emu, Controller controller);
int init_emulator(Emulator* emu, const char* bootrom_path, const char* rom_path, u16* breakpoints);
void destroy_emulator(Emulator* emu);
bool step(Emulator* emu);