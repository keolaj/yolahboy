#include <stdbool.h>

#include "emulator.h"
#include "global_definitions.h"
#include "controller.h"
#include "cpu2.h"
#include "memory2.h"
#include "operations.h"
#include "controller.h"
#include "gpu2.h"

#include <SDL.h>


int init_emulator(Emulator* emu, const char* bootrom_path, const char* rom_path, int* breakpoints) {
	emu->cpu = create_cpu();
	emu->memory = create_memory(bootrom_path, rom_path);
	emu->gpu = create_gpu(emu->memory);
	emu->controller = create_controller();
	emu->breakpoints = breakpoints;
	emu->should_quit = false;

	emu->emulator_window = SDL_CreateWindow("YolahBoy", 700, 200, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
	if (!emu->emulator_window) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set create renderer: %s\n", SDL_GetError());
		printf("create render error");
		// todo cleanup bs
	}

	emu->emulator_renderer = SDL_CreateRenderer(emu->emulator_window, -1, 0);
	if (!emu->emulator_renderer) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set create renderer: %s\n", SDL_GetError());
		printf("create render error");
	}

	emu->tile_window = SDL_CreateWindow("YolahBoy tiles", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 16 * 8, 24 * 8, SDL_WINDOW_RESIZABLE);
	if (!emu->tile_window) {
		printf("create render error");
	}

	emu->tile_renderer = SDL_CreateRenderer(emu->tile_window, -1, 0);
	if (!emu->tile_renderer) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set create renderer: %s\n", SDL_GetError());
		printf("create render error");
	}

	if (SDL_NumJoysticks() < 1) {
		printf("no joystick connected!");
	}
	else {
		emu->game_controller = SDL_GameControllerOpen(0);
		if (emu->game_controller == NULL) {
			printf("Unable to open game controller! SDL Error: %s", SDL_GetError());

		}
	}


	if (emu->cpu == NULL || emu->memory == NULL || emu->gpu == NULL || emu->controller == NULL) return -1; // make this actually clean things up

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
	SDL_DestroyWindow(emu->emulator_window);
	SDL_DestroyWindow(emu->tile_window);
	SDL_DestroyRenderer(emu->emulator_renderer);
	SDL_DestroyRenderer(emu->tile_renderer);
	emu->cpu = NULL;
}
