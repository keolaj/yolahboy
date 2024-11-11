#include <SDL3/SDL.h>
#include "debugger.h"
#include "../components/global_definitions.h"
#include "../components/emulator.h"
#include "../components/controller.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#define CIMGUI_USE_SDL3
#include "cimgui_impl.h"

// extern Emulator emu;
extern HANDLE emu_breakpoint_event;
extern HANDLE emu_draw_event;
extern CRITICAL_SECTION emu_crit;

void updateWindow(SDL_Surface* source, SDL_Window* dest) {
	SDL_BlitSurfaceScaled(source, NULL, SDL_GetWindowSurface(dest), NULL, SDL_SCALEMODE_NEAREST);
	SDL_UpdateWindowSurface(dest);
}

void init_debug_ui(SDL_Window* window, SDL_Renderer* renderer, ImGuiContext* ig_ctx, ImGuiIO* ioptr) {
	if (ig_ctx == NULL) {
		ig_ctx = igCreateContext(NULL);
		igStyleColorsDark(NULL);

		ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
		ImGui_ImplSDLRenderer3_Init(renderer);
	}
	if (ioptr == NULL) {
		ioptr = igGetIO();
		ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	}

}

void draw_debug_ui(SDL_Window* window, SDL_Renderer* renderer, ImGuiContext* ig_ctx, ImGuiIO* ioptr, Emulator* emu, SDL_Rect* emulator_screen_rect, SDL_Rect* tile_screen_rect) {
	static ImVec4 clear_color = {
		.x = 0.45f,
		.y = 0.55f,
		.z = 0.60f,
		.w = 1.00f
	};

	ImGui_ImplSDL3_NewFrame();
	ImGui_ImplSDLRenderer3_NewFrame();
	igNewFrame();

	igBegin("DEBUGGER", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	igButton("RUN", (ImVec2) { 40, 15 });
	igEnd();

	igRender();
	SDL_SetRenderDrawColorFloat(renderer, clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	SDL_RenderClear(renderer);
	ImGui_ImplSDLRenderer3_RenderDrawData(igGetDrawData(), renderer);
	SDL_RenderPresent(renderer);
}

void destroy_debug_ui(SDL_Window* window, SDL_Renderer* renderer, ImGuiContext* ig_ctx, ImGuiIO* ioptr) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();

	igDestroyContext(ig_ctx);
}

//int debugger_run_threaded(HANDLE emulator_thread, args* t_args) {
//
//	SDL_Window* window = SDL_CreateWindow("Yolahboy Debugger", 400, 400, 0);
//	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
//
//	if (window == NULL) {
//		printf("could not initialize debugger window");
//		return -1;
//	}
//
//	if (renderer == NULL) {
//		printf("could not initialize debugger renderer");
//		SDL_DestroyWindow(window);
//		return -1;
//	}
//
//	if (renderer == NULL) {
//		printf("could not initialize debugger renderer");
//		SDL_DestroyWindow(window);
//		return -1;
//	}
//
//	EnterCriticalSection(&emu_crit);
//	if (init_emulator(&emu, t_args->argv[1], t_args->argv[2], t_args->breakpoint_arr) < 0) {
//		destroy_emulator(&emu);
//	}
//	LeaveCriticalSection(&emu_crit);
//
//	ResumeThread(emulator_thread);
//
//	ImGuiContext* ig_ctx = NULL;
//	ImGuiIO* ioptr = NULL;
//
//	init_debug_ui(window, renderer, ig_ctx, ioptr);
//
//	bool quit = false;
//	while (!quit) {
//
//		switch (WaitForSingleObject(emu_breakpoint_event, 0)) {
//		case WAIT_OBJECT_0:
//			EnterCriticalSection(&emu_crit);
//			if (emu.cpu == NULL) {
//				quit = true;
//				if (emu.tile_window) SDL_DestroyWindow(emu.tile_window);
//				if (emu.emulator_renderer) SDL_DestroyRenderer(emu.emulator_renderer);
//				if (emu.tile_renderer) SDL_DestroyRenderer(emu.tile_renderer);
//				if (emu.emulator_window) SDL_DestroyWindow(emu.emulator_window);
//				if (emu.game_controller) SDL_CloseGamepad(emu.game_controller);
//				destroy_emulator(&emu);
//				break;
//			}
//			print_registers(emu.cpu);
//			emu.should_quit = true;
//			quit = true;
//			LeaveCriticalSection(&emu_crit);
//			ResumeThread(emulator_thread);
//			break;
//		case WAIT_TIMEOUT:
//			break;
//		case WAIT_FAILED:
//			break;
//		default:
//			break;
//
//		}
//
//		switch (WaitForSingleObject(emu_draw_event, 0)) {
//		case WAIT_OBJECT_0: {
//			EnterCriticalSection(&emu_crit);
//			SDL_Event e;
//			while (SDL_PollEvent(&e)) {
//			ImGui_ImplSDL3_ProcessEvent(&e);
//
//				if (e.type == SDL_EVENT_QUIT || e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
//					emu.should_quit = true;
//					quit = true;
//				}
//				else {
//					update_emu_controller(&emu, get_controller_state(emu.game_controller));
//					// print_controller(emu.memory->controller);
//				}
//			}
//			updateWindow(emu.gpu->screen, emu.emulator_window);
//			updateWindow(emu.gpu->tile_screen, emu.tile_window);
//
//			// draw debug window
//			draw_debug_ui(window, renderer, ig_ctx, ioptr, &emu);
//
//			LeaveCriticalSection(&emu_crit);
//			ResumeThread(emulator_thread);
//			break;
//		}
//		case WAIT_TIMEOUT:
//			break;
//		case WAIT_FAILED:
//			break;
//		}
//
//	}
//
//	SDL_DestroyWindow(window);
//	SDL_DestroyRenderer(renderer);
//
//	return 0;
//
//}

int debugger_run(args* emu_args) {
	SDL_Window* window = SDL_CreateWindow("Yolahboy Debugger", 800, 600, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

	if (window == NULL) {
		printf("could not initialize debugger window");
		return -1;
	}

	if (renderer == NULL) {
		printf("could not initialize debugger renderer");
		SDL_DestroyWindow(window);
		return -1;
	}

	Emulator emu;
	if (init_emulator(&emu, emu_args->argv[1], emu_args->argv[2], emu_args->breakpoint_arr) < 0) {
		destroy_emulator(&emu);
		return -1;
	}

	ImGuiContext* ig_ctx = NULL;
	ImGuiIO* ioptr = NULL;

	init_debug_ui(window, renderer, ig_ctx, ioptr);

	bool quit = false;
	int clocks = 0;
	SDL_Event e;

	SDL_Rect emulator_screen_rect;
	SDL_Rect tile_screen_rect;

	while (!quit) {

		for (int i = 0; i < MAX_BREAKPOINTS; ++i) {
			if ((int)emu.cpu->registers.pc == emu.breakpoints[i]) {
				emu.should_run = false;
			}
		}


		if (emu.should_run) {
			int c = step(&emu);
			if (c == -1) goto end;
			clocks += c;
		}

		if (clocks > 29780 || !emu.should_run) {
			

			draw_debug_ui(window, renderer, ig_ctx, ioptr, &emu, &emulator_screen_rect, &tile_screen_rect);

			while (SDL_PollEvent(&e)) {
				ImGui_ImplSDL3_ProcessEvent(&e);

				switch (e.type) {
				case SDL_EVENT_QUIT:
				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
					goto end;
				case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
				case SDL_EVENT_GAMEPAD_BUTTON_UP:
					update_emu_controller(&emu, get_controller_state(emu.game_controller));
					break;
				}
			}
			
			if (clocks > 29780) {
				
				

				clocks = 0;
			}
			// todo draw emulator surface on proper location on debug window
		}
	}
end:
	destroy_emulator(&emu);
	destroy_debug_ui(window, renderer, ig_ctx, ioptr);
	return 0;
}
