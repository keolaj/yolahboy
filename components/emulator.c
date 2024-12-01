#include <stdbool.h>

#include "emulator.h"
#include "global_definitions.h"
#include "./timer/timer.h"
#include "./controller/controller.h"
#include "./cpu/cpu.h"
#include "./memory/memory.h"
#include "./cpu/operations.h"
#include "./controller/controller.h"
#include "./gpu/gpu.h"
#include "./apu/apu.h"
#include "../debugger/imgui_custom_widget_wrapper.h"

#include <SDL3/SDL.h>

int init_emulator(Emulator* emu) {
	// emu->cpu = create_cpu();
	memset(&emu->cpu, 0, sizeof(Cpu));
	emu->memory = create_memory();
	emu->gpu = create_gpu(emu->memory);
	emu->timer = create_timer();
	emu->apu = create_apu(48000, 1024);
	emu->timer->clock = 0;
	emu->cpu.timer = emu->timer;
	emu->memory->timer = emu->timer;
	emu->should_run = false;
	emu->clock = 0;
	memset(&emu->controller, 0, sizeof(Controller));

	if (emu->memory == NULL || emu->gpu == NULL) {
		destroy_emulator(emu);
		return -1;
	}
	emu->memory->gpu = emu->gpu;
	emu->memory->controller =  &emu->controller;
	emu->memory->apu = emu->apu;
	return 0;
}


void update_emu_controller(Emulator* emu, Controller controller) {
	emu->controller = controller;
}

int step(Emulator* emu) {
	Operation to_exec = get_operation(&emu->cpu, emu->memory);
	Cycles c;
	if (!emu->cpu.halted) {
		c = cpu_step(&emu->cpu, emu->memory, to_exec);
	}
	else {
		c = run_halted(&emu->cpu, emu->memory);
	}
	if (c.t_cycles < 0) {
		return -1;
	}
	tick(emu, c.t_cycles);
	gpu_step(emu->gpu, emu->memory, c.t_cycles);
	apu_step(emu->apu, c.t_cycles);

	return 0;
}


void destroy_emulator(Emulator* emu) {
	// if (emu->cpu) destroy_cpu(emu->cpu);
	if (emu->memory) destroy_memory(emu->memory);
	if (emu->gpu) destroy_gpu(emu->gpu);
	if (emu->timer) free(emu->timer);
	if (emu->apu) destroy_apu(emu->apu);
	// emu->cpu = NULL;
	emu->memory = NULL;
	emu->gpu = NULL;
	emu->timer = NULL;
	emu->apu = NULL;
}

bool cartridge_loaded(Emulator* emu) {
	if (emu->memory->cartridge.rom == NULL) return false;
	else return true;
}

void skip_bootrom(Emulator* emu) {
	//emu->cpu.registers.a = 0x01;
	//emu->cpu.registers.f = 0xB0;
	//emu->cpu->registers.b = 0x00;
	//emu->cpu->registers.c = 0x13;
	//emu->cpu->registers.d = 0x00;
	//emu->cpu->registers.e = 0xD8;
	//emu->cpu->registers.h = 0x01;
	//emu->cpu->registers.l = 0x4D;
	//emu->cpu->registers.sp = 0xFFFE;
	//emu->cpu->registers.pc = 0x0100;

	//emu->memory->in_bios = false;
}
