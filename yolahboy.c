#include <Windows.h>
#include <stdio.h>
#include <assert.h>

#include <SDL3/SDL.h>

#include "emulator_main.h"
#include "components/emulator.h"
#include "debugger/debugger.h"

#define APP_NAME "YolahBoy Debugger"

Emulator emu;
HANDLE emu_breakpoint_event;
HANDLE emu_draw_event;
CRITICAL_SECTION emu_crit;

HANDLE emulator_thread;

args* create_args(int argc, char** argv) {
	args* rom_args = (args*)malloc(sizeof(args));
	if (rom_args == NULL) {
		printf("could not init rom args");
		return NULL;
	}

	rom_args->argc = argc;
	rom_args->argv = argv;

	rom_args->breakpoint_arr = (int*)malloc(sizeof(int) * MAX_BREAKPOINTS);
	if (rom_args->breakpoint_arr == NULL) {
		printf("Couldn't allocate breakpoint array");
		free(rom_args);
		return NULL;
	}
	for (int i = 0; i < MAX_BREAKPOINTS; ++i) {
		rom_args->breakpoint_arr[i] = -1;
	}
	// rom_args->breakpoint_arr[0] = 0x100;
	return rom_args;
}

int main(int argc, char* argv[]) {

	DWORD emu_exit_code = -1;

	SDL_SetMainReady();
	InitializeCriticalSection(&emu_crit);

	emu_breakpoint_event = CreateEventExA(NULL, TEXT("BREAKPOINT_EMULATOR"), 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
	if (emu_breakpoint_event == NULL) {
		printf("could not create breakpoint event");
		goto cleanup;
	}
	emu_draw_event = CreateEventExA(NULL, TEXT("DRAW_EMULATOR"), 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
	if (emu_draw_event == NULL) {
		printf("could not create draw event");
		goto cleanup;
	}

	SECURITY_ATTRIBUTES emu_thread_atts = {
		sizeof(SECURITY_ATTRIBUTES), NULL, false
	};

	args* rom_args = create_args(argc, argv);
	if (rom_args == NULL) {
		printf("could not initialize emulator thread args");
		goto cleanup;
	}

	int did_SDL_init = 0;
	did_SDL_init = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO);
	if (did_SDL_init < 0) {
		printf("could not init SDL: %s", SDL_GetError());
		goto cleanup;
	}

	emulator_thread = CreateThread(&emu_thread_atts, 0, run_emulator, rom_args, CREATE_SUSPENDED, NULL);
	if (emulator_thread == NULL) {
		printf("could not start emulator thread");
		return -1;
	}

	if (debugger_run(emulator_thread, rom_args) < 0) {
		TerminateThread(emulator_thread, 0);
		return -1;
	}
	else {
		WaitForSingleObject(emulator_thread, INFINITE);
		GetExitCodeThread(emulator_thread, &emu_exit_code);
	}

cleanup:
	if (emu_breakpoint_event != NULL) CloseHandle(emu_breakpoint_event);
	if (emu_draw_event != NULL) CloseHandle(emu_draw_event);
	if (emulator_thread != NULL) CloseHandle(emulator_thread);
	DeleteCriticalSection(&emu_crit);
	if (rom_args != NULL) free(rom_args);

	return emu_exit_code;
}
