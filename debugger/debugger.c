#include <SDL.h>
#include "debugger.h"
#include "../components/global_definitions.h"
#include "../components/emulator.h"

extern Emulator emu;
extern HANDLE emu_breakpoint_event;
extern HANDLE emu_draw_event;
extern CRITICAL_SECTION emu_crit;

void updateWindow(SDL_Surface* source, SDL_Window* dest) {
	SDL_BlitSurface(source, NULL, SDL_GetWindowSurface(dest), NULL);
	SDL_UpdateWindowSurface(dest);
}

int debugger_run(HANDLE emulator_thread) {

	//SDL_Window* window = SDL_CreateWindow("Yolahboy Debugger", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 400, 400, 0);
	//SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	//if (window == NULL) {
	//	printf("could not initialize debugger window");
	//	return -1;
	//}

	//if (renderer == NULL) {
	//	printf("could not initialize debugger renderer");
	//	SDL_DestroyWindow(window);
	//	return -1;
	//}

	ResumeThread(emulator_thread);

	bool quit = false;
	while (!quit) {

		switch (WaitForSingleObject(emu_draw_event, 0)) {
		case WAIT_OBJECT_0:
			EnterCriticalSection(&emu_crit);
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
				break;
			}
			print_registers(emu.cpu);
			// emu.should_quit = true;
			// quit = true;
			LeaveCriticalSection(&emu_crit);
			ResumeThread(emulator_thread);
			break;
		case WAIT_TIMEOUT:
			break;
		}
	}

	//SDL_DestroyWindow(window);
	//SDL_DestroyRenderer(renderer);

	return 0;

}
