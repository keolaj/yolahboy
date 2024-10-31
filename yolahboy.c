#include "yolahboy.h"
#include <Windows.h>

void updateWindow(SDL_Surface* source, SDL_Window* dest) {
	SDL_BlitSurface(source, NULL, SDL_GetWindowSurface(dest), NULL);
	SDL_UpdateWindowSurface(dest);
}


int main(int argc, char* argv[]) {

	Emulator emu;
	init_emulator(&emu, argv[1], argv[2]);

	SDL_Window* window;
	SDL_Renderer* renderer;

	SDL_Window* tile_window;
	SDL_Renderer* tile_renderer;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		assert(false && "COULDNT INIT SDL");
	}

	window = SDL_CreateWindow("YolahBoy", 700, 200, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set create renderer: %s\n", SDL_GetError());
		assert(false);
	}

	tile_window = SDL_CreateWindow("YolahBoy tiles", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 16 * 8, 24 * 8, SDL_WINDOW_RESIZABLE);
	if (!window) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set create window: %s\n", SDL_GetError());
		assert(false);
	}

	tile_renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set create renderer: %s\n", SDL_GetError());
		assert(false);
	}

	int c = 0;

	while (true) {
		Operation to_execute = get_operation(emu.cpu, emu.memory);
		// print_operation(to_execute);
		Cycles clock = step_cpu(emu.cpu, emu.memory, to_execute);
		// print_registers(emu.cpu);
		c += clock.t_cycles;
		step_gpu(emu.gpu, clock.t_cycles);
		if (c > 29780) {
			updateWindow(emu.gpu->screen, window);
			updateWindow(emu.gpu->tile_screen, tile_window);
			c = 0;
			// Sleep(5);
		}

	}

	destroy_emulator(&emu);

	return 0;
}