#include <stdbool.h>

#include "emulator.h"
#include "global_definitions.h"
#include "controller.h"

void init_emulator(Emulator* emu, const char* bootrom_path, const char* rom_path) {
	emu->cpu = create_cpu();
	emu->memory = create_memory(bootrom_path, rom_path);
	emu->gpu = create_gpu(emu->memory);
	emu->controller = create_controller();
	set_gpu(emu->memory, emu->gpu);
}


void update_emu_controller(Emulator* emu, Controller controller) {
	set_mem_controller(emu->memory, controller);
}



void destroy_emulator(Emulator* emu) {
	destroy_gpu(emu->gpu);
}
