#include <stdbool.h>

#include "emulator.h"
#include "global_definitions.h"
#include "controller.h"

int init_emulator(Emulator* emu, const char* bootrom_path, const char* rom_path, u16* breakpoints) {
	emu->cpu = create_cpu();
	emu->memory = create_memory(bootrom_path, rom_path);
	emu->gpu = create_gpu(emu->memory);
	emu->controller = create_controller();
	emu->breakpoints = breakpoints;
	emu->should_quit = false;
	
	if (emu->cpu == NULL || emu->memory == NULL || emu->gpu == NULL || emu->controller == NULL) return -1;

	set_gpu(emu->memory, emu->gpu);
	return 0;
}


void update_emu_controller(Emulator* emu, Controller controller) {
	set_mem_controller(emu->memory, controller);
}



void destroy_emulator(Emulator* emu) {
	destroy_gpu(emu->gpu);
	destroy_memory(emu->memory);
	destroy_cpu(emu->cpu);
	destroy_controller(emu->controller);
	emu->cpu = NULL;
}
