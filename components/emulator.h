#pragma once

#include "global_definitions.h"

void update_emu_controller(Emulator* emu, Controller controller);
int init_emulator(Emulator* emu, const char* bootrom_path, const char* rom_path, int* breakpoints);
void destroy_emulator(Emulator* emu);
bool step(Emulator* emu);