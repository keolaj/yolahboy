
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "debugger/debugger.h"

#define APP_NAME "YolahBoy Debugger"


int main(int argc, const char** argv) {
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK)) {
		printf("could not initialize sdl! (%s)", SDL_GetError());
		return -1;
	}

	char* rom_path = argv[2];
	char* bootrom_path = argv[1];

	int emu_exit_code = debugger_run(rom_path, bootrom_path);

	return emu_exit_code;
}
