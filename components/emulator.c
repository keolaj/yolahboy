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

int init_emulator(Emulator* emu) {
	emu->cpu = create_cpu();
	emu->memory = create_memory();
	emu->gpu = create_gpu(emu->memory);
	emu->controller = create_controller();
	emu->should_run = false;
	emu->clock = 0;

	if (emu->cpu == NULL || emu->memory == NULL || emu->gpu == NULL || emu->controller == NULL) {
		destroy_emulator(emu);
		return -1;
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
	if (c.t_cycles < 0) {
		return -1; // push error to whatever is using emulator
	}
	emu->clock += c.t_cycles;
	step_gpu(emu->gpu, c.t_cycles);
	++emu->memory->memory[0xFF04];
	if (emu->clock > 29780) {
		emu->clock = 0;
		emu->should_draw = true;
	}
	return 0;
}


void destroy_emulator(Emulator* emu) {
	if (emu->gpu) destroy_gpu(emu->gpu);
	if (emu->memory) destroy_memory(emu->memory);
	if (emu->cpu) destroy_cpu(emu->cpu);
	emu->cpu = NULL;
}

void reset_emulator(Emulator* emu) {
	
}

bool cartridge_loaded(Emulator* emu) {
	if (emu->memory->cartridge.rom == NULL) return false;
	else return true;
}

void skip_bootrom(Emulator* emu) {
	emu->cpu->registers.a = 0x01;
	emu->cpu->registers.f = 0xB0;
	emu->cpu->registers.b = 0x00;
	emu->cpu->registers.c = 0x13;
	emu->cpu->registers.d = 0x00;
	emu->cpu->registers.e = 0xD8;
	emu->cpu->registers.h = 0x01;
	emu->cpu->registers.l = 0x4D;
	emu->cpu->registers.sp = 0xFFFE;
	emu->cpu->registers.pc = 0x0100;

	emu->memory->in_bios = false;
}
