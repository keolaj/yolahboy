#include <stdbool.h>

#include "emulator.h"
#include "global_definitions.h"
#include "controller.h"
#include "cpu.h"
#include "memory.h"
#include "operations.h"
#include "controller.h"
#include "gpu.h"
#include "../debugger/imgui_custom_widget_wrapper.h"

#include <SDL3/SDL.h>

int init_emulator(Emulator* emu, const char* bootrom_path, const char* rom_path) {
	emu->cpu = create_cpu();
	emu->memory = create_memory(bootrom_path, rom_path);
	emu->gpu = create_gpu(emu->memory);
	emu->controller = create_controller();
	emu->should_run = false;
	emu->clock = 0;

	if (emu->cpu == NULL || emu->memory == NULL || emu->gpu == NULL || emu->controller == NULL) {
		destroy_emulator(emu);
		return -1; // make this actually clean things up
	}
	set_gpu(emu->memory, emu->gpu);
	return 0;
}


void update_emu_controller(Emulator* emu, Controller controller) {
	set_mem_controller(emu->memory, controller);
}

int step(Emulator* emu) {
	Operation to_exec = get_operation(emu->cpu, emu->memory);
	Cycles c = step_cpu(emu->cpu, emu->memory, to_exec);
	step_gpu(emu->gpu, c.t_cycles);
	return c.t_cycles;
}


void destroy_emulator(Emulator* emu) {
	if (emu->gpu) destroy_gpu(emu->gpu);
	if (emu->memory) destroy_memory(emu->memory);
	if (emu->cpu) destroy_cpu(emu->cpu);
	emu->cpu = NULL;
}
