#include <stdbool.h>

#include "emulator.h"
#include "global_definitions.h"
#include "controller.h"
#include "cpu2.h"
#include "memory2.h"
#include "operations.h"
#include "controller.h"
#include "gpu2.h"
#include "../debugger/imgui_custom_widget_wrapper.h"

#include <SDL3/SDL.h>

int init_emulator(Emulator* emu, const char* bootrom_path, const char* rom_path, int* breakpoints) {
	emu->cpu = create_cpu();
	emu->memory = create_memory(bootrom_path, rom_path);
	emu->gpu = create_gpu(emu->memory);
	emu->controller = create_controller();
	emu->breakpoints = breakpoints;
	emu->should_quit = false;
	emu->should_run = false;

	// emu->emulator_window = SDL_CreateWindow("YolahBoy", SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2, 0);
	//if (!emu->emulator_window) {
	//	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set create renderer: %s\n", SDL_GetError());
	//	AddLog("create render error");
	//	// todo cleanup bs
	//}

	//emu->emulator_renderer = SDL_CreateRenderer(emu->emulator_window, NULL);
	//if (!emu->emulator_renderer) {
	//	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set create renderer: %s\n", SDL_GetError());
	//	AddLog("create render error");
	//}

	//emu->tile_window = SDL_CreateWindow("YolahBoy tiles", TILES_X * 8 * 2, TILES_Y * 8 * 2, 0);
	//if (!emu->tile_window) {
	//	AddLog("create render error");
	//}
	//if (SDL_SetWindowMinimumSize(emu->tile_window, 0, 0)) {

	//}
	//else {
	//	AddLog("couldn't resize tile window!");
	//}


	//emu->tile_renderer = SDL_CreateRenderer(emu->tile_window, NULL);
	//if (!emu->tile_renderer) {
	//	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set create renderer: %s\n", SDL_GetError());
	//	AddLog("create render error");
	//}

	int num_joysticks = 0;
	SDL_JoystickID* joysticks = SDL_GetJoysticks(&num_joysticks);

	if (num_joysticks) {
		emu->game_controller = SDL_OpenGamepad(joysticks[0]);
		if (emu->game_controller == NULL) {
			AddLog("Unable to open game controller! SDL Error: %s", SDL_GetError());
		}
	}
	else {
		AddLog("no joysticks connected!");
	}
	SDL_free(joysticks);



	if (emu->cpu == NULL || emu->memory == NULL || emu->gpu == NULL || emu->controller == NULL) return -1; // make this actually clean things up

	set_gpu(emu->memory, emu->gpu);
	return 0;
}


void update_emu_controller(Emulator* emu, Controller controller) {
	set_mem_controller(emu->memory, controller);
}

int step(Emulator* emu) {
	Operation to_exec = get_operation(emu->cpu, emu->memory);
	Cycles clock = step_cpu(emu->cpu, emu->memory, to_exec);
	if (clock.m_cycles == -1 && clock.t_cycles == -1) {
		emu->should_quit = true;
	}
	step_gpu(emu->gpu, clock.t_cycles);
	return clock.t_cycles;
}


void destroy_emulator(Emulator* emu) {
	destroy_gpu(emu->gpu);
	destroy_memory(emu->memory);
	destroy_cpu(emu->cpu);
	destroy_controller(emu->controller);
	//SDL_DestroyWindow(emu->emulator_window);
	//SDL_DestroyWindow(emu->tile_window);
	//SDL_DestroyRenderer(emu->emulator_renderer);
	//SDL_DestroyRenderer(emu->tile_renderer);
	emu->cpu = NULL;
}
