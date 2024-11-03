#undef main
#define SDL_MAIN_HANDLED

#include "SDL.h"

#include <Windows.h>
#include "components/global_definitions.h"

#include "emulator_main.h"
#include "components/global_definitions.h"
#include "components/gpu_definitions.h"
#include "components/emulator.h"
#include "components/operations.h"
#include "components/operation_defitions.h"
#include "components/cpu2.h"
#include "components/memory2.h"

extern Emulator emu;
extern LPHANDLE emu_breakpoint_event;
extern LPHANDLE emu_step_event;
extern CRITICAL_SECTION emu_crit;

void updateWindow(SDL_Surface* source, SDL_Window* dest) {
	SDL_BlitSurface(source, NULL, SDL_GetWindowSurface(dest), NULL);
	SDL_UpdateWindowSurface(dest);
}

int run_emulator(LPVOID t_args) {

	// HANDLE mem_pipe_handle = ((args*)t_args)->mem_pipe_handle;
	int argc = ((args*)t_args)->argc;
	char** argv = ((args*)t_args)->argv;
	u16* breakpoints = ((args*)t_args)->breakpoint_arr;

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	SDL_Window* tile_window = NULL;
	SDL_Renderer* tile_renderer = NULL;
	SDL_GameController* game_controller = NULL;

	int did_SDL_init = -1;

	SDL_SetMainReady();

	window = SDL_CreateWindow("YolahBoy", 700, 200, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
	if (!window) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set create renderer: %s\n", SDL_GetError());
		goto cleanup;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set create renderer: %s\n", SDL_GetError());
		goto cleanup;
	}

	tile_window = SDL_CreateWindow("YolahBoy tiles", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 16 * 8, 24 * 8, SDL_WINDOW_RESIZABLE);
	if (!tile_window) {
		goto cleanup;
	}

	tile_renderer = SDL_CreateRenderer(tile_window, -1, 0);
	if (!tile_renderer) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set create renderer: %s\n", SDL_GetError());
		goto cleanup;
	}
	did_SDL_init = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
	if (did_SDL_init < 0) {
		printf("could not init SDL: %s", SDL_GetError());
		goto cleanup;
	}


	if (SDL_NumJoysticks() < 1) {
		printf("no joystick connected!");
		goto cleanup;
	}
	else {
		game_controller = SDL_GameControllerOpen(0);
		if (game_controller == NULL) {
			printf("Unable to open game controller! SDL Error: %s", SDL_GetError());
			goto cleanup;
		}
	}

	EnterCriticalSection(&emu_crit);
	if (init_emulator(&emu, argv[1], argv[2], breakpoints) < 0) {
		goto cleanup;
	}
	LeaveCriticalSection(&emu_crit);



	int c = 0;
	bool quit = false;
	while (!quit) {
		EnterCriticalSection(&emu_crit);
		for (int i = 0; i < MAX_BREAKPOINTS; ++i) {
			if (emu.cpu->registers.pc == emu.breakpoints[i] && emu.breakpoints[i] != 0) {
				printf("BREAKPOINT!!\n");
				LeaveCriticalSection(&emu_crit);
				SetEvent(emu_breakpoint_event);
				// print_registers(emu.cpu);


				SuspendThread(GetCurrentThread());
				EnterCriticalSection(&emu_crit);
				break;
			}
		}


		Operation to_execute = get_operation(emu.cpu, emu.memory);
		// print_operation(to_execute);
		Cycles clock = step_cpu(emu.cpu, emu.memory, to_execute);
		if (clock.m_cycles == -1 && clock.t_cycles == -1) {
			goto cleanup;
		}
		// print_registers(emu.cpu);
		c += clock.t_cycles;
		step_gpu(emu.gpu, clock.t_cycles);
		if (c > 29780) {
			SDL_Event e;
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT) {
					quit = true;
				}
				else {
					update_emu_controller(&emu, get_controller_state(game_controller));
					print_controller(emu.memory->controller);
				}
			}
			updateWindow(emu.gpu->screen, window);
			updateWindow(emu.gpu->tile_screen, tile_window);
			c = 0;
			// Sleep(5);
		}
		if (emu.should_quit) {
			quit = true;
		}
		LeaveCriticalSection(&emu_crit);
	}

cleanup:
	if (tile_window) SDL_DestroyWindow(tile_window);
	if (renderer) SDL_DestroyRenderer(renderer);
	if (tile_renderer) SDL_DestroyRenderer(tile_renderer);
	if (window) SDL_DestroyWindow(window);
	if (game_controller) SDL_GameControllerClose(game_controller);
	if (did_SDL_init > 0) SDL_Quit();
	EnterCriticalSection(&emu_crit);
	destroy_emulator(&emu);
	LeaveCriticalSection(&emu_crit);
	return 0;
}
