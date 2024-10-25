#include "yolahboy.h"
#include <cassert>
#include <thread>
#include <chrono>

int main(int argc, char* argv[])
{

	uint8_t a = 255;
	uint8_t b = 1;
	std::cout << std::uppercase << std::hex;

	SDL_Window* window;
	SDL_Renderer* renderer;

	SDL_Window* tileWindow;
	SDL_Renderer* tileRenderer;

	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
	}

	window = SDL_CreateWindow("YolahBoy", 700, 200, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
	if (!window) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set create window: %s\n", SDL_GetError());
		assert(false);
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set create renderer: %s\n", SDL_GetError());
		assert(false);
	}

	tileWindow = SDL_CreateWindow("YolahBoy tiles", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 16 * 8, 24 * 8, SDL_WINDOW_RESIZABLE);
	if (!window) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set create window: %s\n", SDL_GetError());
		assert(false);
	}

	tileRenderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set create renderer: %s\n", SDL_GetError());
		assert(false);
	}



	Memory* mem = new Memory{ std::string(argv[1]), std::string(argv[2]) };
	Cpu cpu{ mem };
	Gpu gpu{ mem, renderer };
	mem->setGpu(&gpu);

	int cycles = 0;

	while (true) {
		Cycles stepCycles = cpu.step();
		cycles += stepCycles.t_cycles;
		gpu.step(stepCycles);
		if (cycles > 29780) {
			updateWindow(gpu.getScreen(), window);
			updateWindow(gpu.getTileScreen(), tileWindow);
			cycles = 0;
			//	std::this_thread::sleep_for(std::chrono::milliseconds(5));
			// cpu.printRegisters();
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return 0;

}

void updateWindow(SDL_Surface* source, SDL_Window* dest) {
	SDL_BlitSurface(source, nullptr, SDL_GetWindowSurface(dest), nullptr);
	SDL_UpdateWindowSurface(dest);
}
