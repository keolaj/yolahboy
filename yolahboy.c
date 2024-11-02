#include "yolahboy.h"
#include <Windows.h>

#define MAX_BREAKPOINTS 0x100

void updateWindow(SDL_Surface* source, SDL_Window* dest) {
	SDL_BlitSurface(source, NULL, SDL_GetWindowSurface(dest), NULL);
	SDL_UpdateWindowSurface(dest);
}

typedef struct {
	// HANDLE mem_pipe_handle;
	int argc;
	char** argv;
	u16* breakpoint_arr;

} args;

Emulator emu;
HANDLE emu_mutex;

int run_emulator(LPVOID t_args) {


	DWORD dwWaitResult;

	dwWaitResult = WaitForSingleObject(emu_mutex, INFINITE);


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

	switch (dwWaitResult) {
	case WAIT_OBJECT_0:
		__try {
			if (init_emulator(&emu, argv[1], argv[2], breakpoints) < 0) {
				goto cleanup;
			}

		}

		__finally {
			printf("initialized emulator");
		}
		break;

	case WAIT_ABANDONED:
		goto cleanup;

	}

	int c = 0;
	bool quit = false;
	while (!quit) {

		for (int i = 0; i < MAX_BREAKPOINTS; ++i) {
			if (emu.cpu->registers.pc == emu.breakpoints[i]) {
				printf("BREAKPOINT!!\n");
				// print_registers(emu.cpu);

				ReleaseMutex(emu_mutex);

				SuspendThread(GetCurrentThread());
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
	}

cleanup:
	if (tile_window) SDL_DestroyWindow(tile_window);
	if (renderer) SDL_DestroyRenderer(renderer);
	if (tile_renderer) SDL_DestroyRenderer(tile_renderer);
	if (window) SDL_DestroyWindow(window);
	if (game_controller) SDL_GameControllerClose(game_controller);
	if (did_SDL_init > 0) SDL_Quit();
	destroy_emulator(&emu);

	return 0;
}

int main(int argc, const char* argv[]) {
	//
	//HANDLE memReadPipe, memWritePipe;
	//SECURITY_ATTRIBUTES mem_pipe_atts = {
	//	sizeof(SECURITY_ATTRIBUTES), NULL, true
	//};
	//if (CreatePipe(memReadPipe, memWritePipe, &mem_pipe_atts, sizeof(Memory*)) == false) {
	//	printf("could not create memory pipe");
	//	return -1;
	//}

	emu_mutex = CreateMutex(NULL, false, NULL);


	args* rom_args = (args*)malloc(sizeof(args));
	if (rom_args == NULL) {
		printf("could not init rom args");
		return -1;
	}
	rom_args->argc = argc;
	rom_args->argv = argv;
	rom_args->breakpoint_arr = (u16*)malloc(sizeof(u16) * MAX_BREAKPOINTS);
	memset(rom_args->breakpoint_arr, 0, sizeof(u16) * MAX_BREAKPOINTS);

	SECURITY_ATTRIBUTES emu_thread_atts = {
		sizeof(SECURITY_ATTRIBUTES), NULL, false
	};

	HANDLE emulator_thread = CreateThread(&emu_thread_atts, 0, run_emulator, rom_args, 0, NULL);

	if (emulator_thread == NULL) {
		printf("could not start emulator thread");
		return -1;
	}



	while (true) {
		Sleep(10);
		DWORD dwWaitResult;

		dwWaitResult = WaitForSingleObject(emu_mutex, INFINITE);
		switch (dwWaitResult) {
		case WAIT_OBJECT_0:
			__try {
				print_registers(emu.cpu);
			}

			__finally {
				printf("back to emulator");
				ReleaseMutex(emu_mutex);
				ResumeThread(emulator_thread);
			}
			break;

		case WAIT_ABANDONED:
			return -1;
		}

	}

	WaitForSingleObject(emulator_thread, INFINITE);

	int emu_exit_code;

	GetExitCodeThread(emulator_thread, (LPDWORD)&emu_exit_code);

	return emu_exit_code;
}