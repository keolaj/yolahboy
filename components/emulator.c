#include <stdbool.h>

#include "emulator.h"
#include "global_definitions.h"

void init_emulator(Emulator* emu, const char* bootrom_path, const char* rom_path) {
	emu->cpu = create_cpu();
	emu->memory = create_memory(bootrom_path, rom_path);
}
