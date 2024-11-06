#include <SDL.h>
#include "debugger.h"
#include "../components/global_definitions.h"
#include "../components/emulator.h"
#include "../components/controller.h"

extern Emulator emu;
extern HANDLE emu_breakpoint_event;
extern HANDLE emu_draw_event;
extern CRITICAL_SECTION emu_crit;

void updateWindow(SDL_Surface* source, SDL_Window* dest) {
	SDL_BlitSurface(source, NULL, SDL_GetWindowSurface(dest), NULL);
	SDL_UpdateWindowSurface(dest);
}

int debugger_run(HANDLE emulator_thread, args* t_args) {

	// TODO: this only works sometimes, might need to call sdl entirely from one thread

	SDL_Window* window = SDL_CreateWindow("Yolahboy Debugger", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 400, 400, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	if (window == NULL) {
		printf("could not initialize debugger window");
		return -1;
	}

	if (renderer == NULL) {
		printf("could not initialize debugger renderer");
		SDL_DestroyWindow(window);
		return -1;
	}

	EnterCriticalSection(&emu_crit);
	if (init_emulator(&emu, t_args->argv[1], t_args->argv[2], t_args->breakpoint_arr) < 0) {
		destroy_emulator(&emu);
	}
	LeaveCriticalSection(&emu_crit);


	ResumeThread(emulator_thread);

	bool quit = false;
	while (!quit) {

		switch (WaitForSingleObject(emu_draw_event, 0)) {
		case WAIT_OBJECT_0:
			EnterCriticalSection(&emu_crit);
			SDL_Event e;
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT) {
					quit = true;
				}
				else {
					update_emu_controller(&emu, get_controller_state(emu.game_controller));
					print_controller(emu.memory->controller);
				}
			}

			updateWindow(emu.gpu->screen, emu.emulator_window);
			updateWindow(emu.gpu->tile_screen, emu.tile_window);
			LeaveCriticalSection(&emu_crit);
			ResumeThread(emulator_thread);
		case WAIT_TIMEOUT:
			break;
		}

		switch (WaitForSingleObject(emu_breakpoint_event, 0)) {
		case WAIT_OBJECT_0:
			EnterCriticalSection(&emu_crit);
			if (emu.cpu == NULL) {
				quit = true;
				if (emu.tile_window) SDL_DestroyWindow(emu.tile_window);
				if (emu.emulator_renderer) SDL_DestroyRenderer(emu.emulator_renderer);
				if (emu.tile_renderer) SDL_DestroyRenderer(emu.tile_renderer);
				if (emu.emulator_window) SDL_DestroyWindow(emu.emulator_window);
				if (emu.game_controller) SDL_GameControllerClose(emu.game_controller);
				destroy_emulator(&emu);
				break;
			}
			print_registers(emu.cpu);
			emu.should_quit = true;
			quit = true;
			LeaveCriticalSection(&emu_crit);
			ResumeThread(emulator_thread);
			break;
		case WAIT_TIMEOUT:
			break;
		}
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	return 0;

}
