
extern "C" {
#include <SDL3/SDL.h>
#include "debugger.h"
#include "../components/global_definitions.h"
#include "../components/emulator.h"
#include "../components/controller.h"
#include "../components/memory2.h"
#include "../components/operations.h"
#include "../components/operation_defitions.h"
	extern Operation operations[];
	extern Operation cb_operations[];
}

#include "imgui.h"
#include "./backends/imgui_impl_sdl3.h"
#include "./backends/imgui_impl_sdlrenderer3.h"
#include "../imgui_memory_editor.h"

#include <stdio.h>

void updateWindow(SDL_Surface* source, SDL_Window* dest) {
	SDL_BlitSurfaceScaled(source, NULL, SDL_GetWindowSurface(dest), NULL, SDL_SCALEMODE_NEAREST);
	SDL_UpdateWindowSurface(dest);
}

void init_debug_ui(SDL_Window* window, SDL_Renderer* renderer, ImGuiContext* ig_ctx, ImGuiIO* ioptr) {
	if (ig_ctx == NULL) {
		ig_ctx = ImGui::CreateContext(NULL);
		ImGui::StyleColorsDark(NULL);

		ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
		ImGui_ImplSDLRenderer3_Init(renderer);
	}
	if (ioptr == NULL) {
		ioptr = &ImGui::GetIO();
		ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	}

}

bool breakpoints[0x10000];

void draw_debug_ui(SDL_Window* window, SDL_Renderer* renderer, ImGuiContext* ig_ctx, ImGuiIO* ioptr, Emulator* emu, SDL_Rect* emulator_screen_rect, SDL_Rect* tile_screen_rect, bool run_once) {
	static ImVec4 clear_color = {
		0.45f,
		0.55f,
		0.60f,
		1.00f
	};

	static MemoryEditor ram_viewer;

	char af_buf[10];
	char bc_buf[10];
	char de_buf[10];
	char hl_buf[10];
	char sp_buf[10];
	char pc_buf[10];

	sprintf(af_buf, "AF: %04hX", emu->cpu->registers.af);
	sprintf(bc_buf, "BC: %04hX", emu->cpu->registers.bc);
	sprintf(de_buf, "DE: %04hX", emu->cpu->registers.de);
	sprintf(hl_buf, "HL: %04hX", emu->cpu->registers.hl);
	sprintf(sp_buf, "SP: %04hX", emu->cpu->registers.sp);
	sprintf(pc_buf, "PC: %04hX", emu->cpu->registers.pc);

	ImGui_ImplSDL3_NewFrame();
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui::NewFrame();

	// Do all ImGui widgets here

	ImGui::Begin("DEBUGGER", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	if (ImGui::Button("RUN", { 40, 15 })) {
		emu->should_run = true;
	}

	ImGui::Begin("REGISTERS", NULL, 0);
	ImGui::Text(af_buf);
	ImGui::Text(bc_buf);
	ImGui::Text(de_buf);
	ImGui::Text(hl_buf);
	ImGui::Text(sp_buf);
	ImGui::Text(pc_buf);
	ImGui::End();

	ImGui::Begin("INSTRUCTIONS", NULL, 0);

	ImGuiListClipper clipper;
	u16 currentPC = emu->cpu->registers.pc;
	clipper.Begin(0x10000);
	while (clipper.Step()) {
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
			Operation* currentOp = &operations[read8(emu->memory, i)];
			if (currentOp->type == CB) {
				++i;
				clipper.DisplayEnd += 1;
				currentOp = &cb_operations[read8(emu->memory, i)];
			}

			char op_buf[10];
			sprintf(op_buf, "%04hX", i);

			ImGui::PushID(i);
			ImGui::Checkbox(op_buf, &breakpoints[i]);
			ImGui::SameLine();
			if (currentOp->type != UNIMPLEMENTED) ImGui::Text(currentOp->mnemonic);
			else {
				char unimpl_buf[30];
				sprintf(unimpl_buf, "UNIMPL OP: 0x%02hX", read8(emu->memory, i));
				ImGui::Text(unimpl_buf);
			}
			ImGui::PopID();
			switch (currentOp->source_addr_mode) {
			case MEM_READ:
			case MEM_READ_ADDR_OFFSET:
				clipper.DisplayEnd += 1;
				i += 1;
				sprintf(op_buf, "%04hX", read8(emu->memory, i));

				ImGui::SameLine();
				ImGui::Text(op_buf);
				break;
			case MEM_READ16:
				clipper.DisplayEnd += 2;
				i += 2;
				sprintf(op_buf, "%04hX", read16(emu->memory, i - 1));
				ImGui::SameLine();
				ImGui::Text(op_buf);
				break;
			}
		}
		if (run_once) {
			float item_pos_y = clipper.ItemsHeight * (emu->cpu->registers.pc) + clipper.ItemsHeight;
			ImGui::SetScrollY(item_pos_y);
		}

	}





	//char op_buf[10];


	//if (emu->memory->in_bios) { // TODO: Implement skipping instructions with data values ex. u8, i8, u16.
	//	// TODO: only render what is in view 
	//	for (int i = emu->cpu->registers.pc; i < 0x100; ++i) {
	//		sprintf(op_buf, "%04hX", i);
	//		ImGui::PushID(i);
	//		if (operations[emu->memory->bios[i]].type != UNIMPLEMENTED) {
	//			ImGui::Checkbox(op_buf, &breakpoints[i]);
	//			ImGui::SameLine();
	//			ImGui::Text(operations[emu->memory->bios[i]].mnemonic);
	//		}
	//		else {
	//			ImGui::Checkbox(op_buf, &breakpoints[i]);
	//			ImGui::SameLine();
	//			ImGui::Text("UNIMPLEMENTED");
	//		}
	//		ImGui::PopID();
	//	}
	//}
	//else {
	//	for (int i = emu->cpu->registers.pc; i < emu->cpu->registers.pc + 0x100; ++i) {
	//		sprintf(op_buf, "%04hX", i);
	//		ImGui::PushID(i);
	//		if (operations[emu->memory->memory[i]].type != UNIMPLEMENTED) {
	//			ImGui::Checkbox(op_buf, &breakpoints[i]);
	//			ImGui::SameLine();
	//			ImGui::Text(operations[emu->memory->memory[i]].mnemonic);
	//		}
	//		else {
	//			ImGui::Checkbox(op_buf, &breakpoints[i]);
	//			ImGui::SameLine();
	//			ImGui::Text("UNIMPLEMENTED");
	//		}
	//		ImGui::PopID();
	//	}

	//}
	ImGui::End();

	ram_viewer.DrawWindow("RAM", emu->memory->memory, 0x10000);

	ImGui::End();

	ImGui::Render();
	SDL_SetRenderDrawColorFloat(renderer, clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	SDL_RenderClear(renderer);
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
	SDL_RenderPresent(renderer);
}

void destroy_debug_ui(SDL_Window* window, SDL_Renderer* renderer, ImGuiContext* ig_ctx, ImGuiIO* ioptr) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();

	ImGui::DestroyContext(ig_ctx);
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
	bool run_once = false;
	bool set_run_once = false;
	SDL_Event e;

	SDL_Rect emulator_screen_rect;
	SDL_Rect tile_screen_rect;

	while (!quit) {

		if (emu.should_run) {
			int c = step(&emu);
			if (c == -1) goto end;
			clocks += c;
			set_run_once = false;
		}

		if (breakpoints[emu.cpu->registers.pc]) {
			emu.should_run = false;
			if (!set_run_once) {
				run_once = true;
				set_run_once = true;
			}
		}

		if (clocks > 29780 || !emu.should_run) {



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

			if (!emu.should_run) {
				draw_debug_ui(window, renderer, ig_ctx, ioptr, &emu, &emulator_screen_rect, &tile_screen_rect, run_once);
				run_once = false;
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

