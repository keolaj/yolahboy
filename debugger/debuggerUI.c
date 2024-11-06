#include <SDL.h>
#include "debuggerUI.h"
#include "../components/global_definitions.h"

extern Emulator emu;
extern LPHANDLE emu_breakpoint_event;
extern CRITICAL_SECTION emu_crit;



int debugger_run(HANDLE emulator_thread) {

	SDL_Window* window = SDL_CreateWindow("Yolahboy Debugger", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 400, 400, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	if (window = NULL) {
		printf("could not initialize debugger window");
		return -1;
	}

	if (renderer == NULL) {
		printf("could not initialize debugger renderer");
		SDL_DestroyWindow(window);
		return -1;
	}

	bool quit = false;
	while (!quit) {

		switch (WaitForSingleObject(emu_breakpoint_event, 10)) {
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
			// ResumeThread(emulator_thread);
			break;
		case WAIT_TIMEOUT:
			break;
		}
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	return 0;

}
