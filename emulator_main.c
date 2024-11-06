#include <SDL.h>

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
#include "components/gpu2.h"

extern Emulator emu;
extern LPHANDLE emu_breakpoint_event;
extern LPHANDLE emu_draw_event;
extern CRITICAL_SECTION emu_crit;


int run_emulator(LPVOID t_args) {

	// HANDLE mem_pipe_handle = ((args*)t_args)->mem_pipe_handle;
	int argc = ((args*)t_args)->argc;
	char** argv = ((args*)t_args)->argv;
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	SDL_Window* tile_window = NULL;
	SDL_Renderer* tile_renderer = NULL;
	SDL_GameController* game_controller = NULL;


	int did_SDL_init = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO);
	if (did_SDL_init < 0) {
		printf("could not init SDL: %s", SDL_GetError());
		goto cleanup;
	}


	EnterCriticalSection(&emu_crit);
	if (init_emulator(&emu, argv[1], argv[2], ((args*)t_args)->breakpoint_arr) < 0) {
		goto cleanup;
	}
	LeaveCriticalSection(&emu_crit);

	int c = 0;
	bool quit = false;
	while (!quit) {
		EnterCriticalSection(&emu_crit);
		for (int i = 0; i < MAX_BREAKPOINTS; ++i) {
			if ((int)emu.cpu->registers.pc == emu.breakpoints[i]) {
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
			emu.should_quit = true;
		}
		// print_registers(emu.cpu);
		c += clock.t_cycles;
		step_gpu(emu.gpu, clock.t_cycles);
		if (c > 29780) {

			LeaveCriticalSection(&emu_crit);
			SetEvent(emu_draw_event);
			SuspendThread(GetCurrentThread());
			EnterCriticalSection(&emu_crit);
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
	SetEvent(emu_breakpoint_event);
	return 0;
}
