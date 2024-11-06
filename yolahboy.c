#include <Windows.h>
#include <stdio.h>
#include <assert.h>

#include <SDL.h>

#include "emulator_main.h"
#include "components/emulator.h"

#define APP_NAME "YolahBoy Debugger"

Emulator emu;
LPHANDLE emu_breakpoint_event;
LPHANDLE emu_step_event;
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
	rom_args->breakpoint_arr[0] = 0x100;
	return rom_args;
}

int main(int argc, char* argv[]) {

	SDL_SetMainReady();
	InitializeCriticalSection(&emu_crit);

	emu_breakpoint_event = CreateEventExA(NULL, TEXT("BREAKPOINT_EMULATOR"), 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
	emu_step_event = CreateEventExA(NULL, TEXT("STEP_EMULATOR"), 0, EVENT_MODIFY_STATE | SYNCHRONIZE);

	SECURITY_ATTRIBUTES emu_thread_atts = {
		sizeof(SECURITY_ATTRIBUTES), NULL, false
	};

	args* rom_args = create_args(argc, argv);
	if (rom_args == NULL) {
		printf("could not initialize emulator thread args");
		return FALSE;
	}

	emulator_thread = CreateThread(&emu_thread_atts, 0, run_emulator, rom_args, 0, NULL);
	if (emulator_thread == NULL) {
		printf("could not start emulator thread");
		return -1;
	}

	bool quit = false;
	while (!quit) {

		switch (WaitForSingleObject(emu_breakpoint_event, INFINITE)) {
		case WAIT_OBJECT_0:
			EnterCriticalSection(&emu_crit);
			if (emu.cpu == NULL) {
				quit = true;
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

	WaitForSingleObject(emulator_thread, INFINITE);

	DWORD emu_exit_code;

	GetExitCodeThread(emulator_thread, &emu_exit_code);
	CloseHandle(emulator_thread);

	free(rom_args);

	return emu_exit_code;
}
