#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>

extern "C" {
#include <SDL3/SDL.h>
#include "debugger.h"
#include "../components/global_definitions.h"
#include "../components/emulator.h"
#include "../components/controller.h"
#include "../components/memory.h"
#include "../components/operations.h"
#include "../components/operation_defitions.h"
	extern Operation operations[];
	extern Operation cb_operations[];
}

#include "imgui.h"
#include "./backends/imgui_impl_sdl3.h"
#include "./backends/imgui_impl_sdlrenderer3.h"
#include "../imgui_memory_editor.h"
#include "./imgui_custom_widgets.h"

bool breakpoints[0x10000];
static char bootrom_path_buf[200] = "";
static char rom_path_buf[200] = "";


void init_debug_ui(SDL_Window* window, SDL_Renderer* renderer, ImGuiContext* ig_ctx, ImGuiIO* ioptr, char* rom_path, char* bootrom_path) {
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
	if (rom_path) {
		strcpy(rom_path_buf, rom_path);
	}
	if (bootrom_path) {
		strcpy(bootrom_path_buf, bootrom_path);
	}
}

extern ExampleAppLog app_log;
static bool create_gbd_log = true;
static bool use_gamepad = true;

void draw_debug_ui(SDL_Window* window, SDL_Renderer* renderer, ImGuiContext* ig_ctx, ImGuiIO* ioptr, Emulator* emu, SDL_FRect* emulator_screen_rect, SDL_FRect* tile_screen_rect, bool run_once) {

	static MemoryEditor ram_viewer;
	static ImGuiListClipper clipper;

	static float DEBUGGER_X = 160;
	static float DEBUGGER_Y = 323;
	static float SCREEN_X = 336;
	static float SCREEN_Y = 323;
	static float TILE_X = 144;
	static float TILE_Y = 323;
	static float INSTRUCTION_X = 210;
	static float INSTRUCTION_Y = 600;
	static float TABS_X = 640;
	static float TABS_Y = 277;

	ImGui_ImplSDL3_NewFrame();
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui::NewFrame();

	// Do all ImGui widgets here

	ImGui::SetNextWindowSize({ (float)DEBUGGER_X, (float)DEBUGGER_Y });
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::Begin("DEBUGGER", NULL, ImGuiWindowFlags_NoResize);
	if (ImGui::Button("RUN", { 40, 15 })) {
		if (!cartridge_loaded(emu)) {
			if (strlen(bootrom_path_buf) > 0) {
				if (load_bootrom(emu->memory, bootrom_path_buf) < 0) {
					app_log.AddLog("Couldn't load bootrom!\n");
				}
			}
			else {
				skip_bootrom(emu);
			}
			if (strlen(rom_path_buf) > 0) {
				if (load_rom(emu->memory, rom_path_buf) < 0) {
					app_log.AddLog("Couldn't load rom!\n");
				}
				else {
					emu->should_run = true;
				}
			}
		}
		else {
			emu->should_run = true;
		}
	}

	ImGui::SameLine();
	ImGui::Button("PAUSE", { 40, 15 });
	if (ImGui::IsItemClicked()) { // for some reason this is how I got it to work...
		app_log.AddLog("PAUSE\n");
		emu->should_run = false;
	}

	ImGui::SameLine();
	if (ImGui::Button("RESET", { 40, 15 })) {
		destroy_emulator(emu);
		init_emulator(emu);
	}

	ImGui::BeginChild("REGISTERS");

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

	ImGui::Text(af_buf);
	ImGui::Text(bc_buf);
	ImGui::Text(de_buf);
	ImGui::Text(hl_buf);
	ImGui::Text(sp_buf);
	ImGui::Text(pc_buf);
	ImGui::Text("FPS: %.4f", ImGui::GetIO().Framerate);
	ImGui::EndChild();
	ImGui::End();

	ImGui::SetNextWindowSize({ SCREEN_X, SCREEN_Y });
	ImGui::SetNextWindowPos({ (float)DEBUGGER_X ,(float)0 });
	if (emu->should_run) ImGui::SetNextWindowFocus();
	ImGui::Begin("SCREEN", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
	{

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();
		ImVec2 windowpos = ImGui::GetWindowPos();

		emulator_screen_rect->x = vMin.x + windowpos.x;
		emulator_screen_rect->y = vMin.y + windowpos.y;
		emulator_screen_rect->w = vMax.x - vMin.x;
		emulator_screen_rect->h = vMax.y - vMin.y;

	}
	ImGui::End();

	ImGui::SetNextWindowSize({ TILE_X, TILE_Y });
	ImGui::SetNextWindowPos({ (float)DEBUGGER_X + SCREEN_X, (float)0 });
	ImGui::Begin("TILES", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
	{

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();
		ImVec2 windowpos = ImGui::GetWindowPos();

		tile_screen_rect->x = vMin.x + windowpos.x;
		tile_screen_rect->y = vMin.y + windowpos.y;
		tile_screen_rect->w = vMax.x - vMin.x;
		tile_screen_rect->h = 192;
	}
	ImGui::End();

	ImGui::SetNextWindowSize({ (float)INSTRUCTION_X, (float)INSTRUCTION_Y });
	ImGui::SetNextWindowPos({ (float)DEBUGGER_X + SCREEN_X + TILE_X, (float)0 });
	ImGui::Begin("INSTRUCTIONS", NULL, ImGuiWindowFlags_NoResize);

	u16 currentPC = emu->cpu->registers.pc;

	static int GOTO_Y = 50;

	ImGui::SetNextWindowSize({ ImGui::GetContentRegionMax().x, ImGui::GetContentRegionMax().y - GOTO_Y });
	ImGui::BeginChild("INST_VIEW");
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

			ImGui::Checkbox(op_buf, &breakpoints[i]);
			ImGui::SameLine();
			if (currentOp->type != UNIMPLEMENTED) ImGui::Text(currentOp->mnemonic);
			else {
				char unimpl_buf[30];
				sprintf(unimpl_buf, "UNIMPL OP: 0x%02hX", read8(emu->memory, i));
				ImGui::Text(unimpl_buf);
			}
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
			float item_pos_y = clipper.ItemsHeight * (emu->cpu->registers.pc) + (clipper.ItemsHeight * 0.5);
			ImGui::SetScrollY(item_pos_y);
		}
	}
	ImGui::EndChild();
	ImGui::BeginChild("GOTO"); // TODO make this work
	static char goto_buf[5] = { 0 };
	ImGui::InputText("##GOTO", goto_buf, 5);
	ImGui::SameLine();
	if (ImGui::Button("GOTO")) {
		int to = atoi(goto_buf);
		float item_pos_y = clipper.ItemsHeight * (to)+clipper.ItemsHeight;
		ImGui::SetScrollY(item_pos_y); // need to find out how to scroll sibling window

	}
	ImGui::EndChild();
	ImGui::End();

	ImGui::SetNextWindowSize({ (float)TABS_X, (float)TABS_Y });
	ImGui::SetNextWindowPos({ (float)0, (float)DEBUGGER_Y });
	ImGui::Begin("OTHER", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);
	ImGui::BeginTabBar("##SETTINGSTABS");
	if (ImGui::BeginTabItem("RAM")) {
		ram_viewer.DrawContents(emu->memory->memory, 0x10000);
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("CONSOLE")) {
		// TODO
		app_log.Draw();
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("SETTINGS")) {
		// TODO
		ImGui::Checkbox("Create GameboyDoctor Log", &create_gbd_log);
		ImGui::Checkbox("Use Gamepad", &use_gamepad);

		ImGui::InputText("Bootrom Path", bootrom_path_buf, 200);
		ImGui::SameLine();
		if (ImGui::Button("Open##1")) {
			IGFD::FileDialogConfig config;
			config.filePathName = std::string{ bootrom_path_buf };
			ImGuiFileDialog::Instance()->OpenDialog("ChooseBootrom", "Choose File", ".bin", config);
		}		
		if (ImGuiFileDialog::Instance()->Display("ChooseBootrom")) {
			if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
				// action
				strcpy(bootrom_path_buf, filePathName.c_str());
			}

			// close
			ImGuiFileDialog::Instance()->Close();
		}

		ImGui::InputText("Rom Path", rom_path_buf, 200);
		ImGui::SameLine();
		if (ImGui::Button("Open##2")) {
			IGFD::FileDialogConfig config;
			config.filePathName = std::string{ rom_path_buf };
			ImGuiFileDialog::Instance()->OpenDialog("ChooseRom", "Choose File", ".gb,.bin", config);
		}		
		if (ImGuiFileDialog::Instance()->Display("ChooseRom")) {
			if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
				// action
				strcpy(rom_path_buf, filePathName.c_str());
			}

			// close
			ImGuiFileDialog::Instance()->Close();
		}


		ImGui::EndTabItem();
	}

	if (run_once) {
	}

	ImGui::EndTabBar();
	ImGui::End();
	ImGui::Render();
}

void destroy_debug_ui(SDL_Window* window, SDL_Renderer* renderer, ImGuiContext* ig_ctx, ImGuiIO* ioptr) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();

	ImGui::DestroyContext(ig_ctx);
}

SDL_Gamepad* get_first_gamepad() {

	SDL_Gamepad* ret = NULL;

	int num_joysticks = 0;
	SDL_JoystickID* joysticks = SDL_GetJoysticks(&num_joysticks);

	if (num_joysticks) {
		ret = SDL_OpenGamepad(joysticks[0]);
		if (ret == NULL) {
			app_log.AddLog("Unable to open game controller! SDL Error: %s", SDL_GetError());
		}
	}
	else {
		app_log.AddLog("no joysticks connected!");
	}
	SDL_free(joysticks);
	return ret;
}

int debugger_run(char* rom_path, char* bootrom_path) {

	SDL_Window* window = SDL_CreateWindow("Yolahboy Debugger", 850, 600, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
	SDL_Texture* screen_tex = NULL;
	SDL_Texture* tile_tex = NULL;
	SDL_Gamepad* gamepad = NULL;
	SDL_FRect emulator_screen_rect{ 0, 0, 160, 144 };
	SDL_FRect tile_screen_rect{ 0, 0, 128, 192 };

	ImVec4 clear_color = {
			0.45f,
			0.55f,
			0.60f,
			1.00f
	};

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
	if (init_emulator(&emu) < 0) {
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		destroy_emulator(&emu);
		return -1;
	}

	screen_tex = SDL_CreateTextureFromSurface(renderer, emu.gpu->screen);
	tile_tex = SDL_CreateTextureFromSurface(renderer, emu.gpu->tile_screen);

	ImGuiContext* ig_ctx = NULL;
	ImGuiIO* ioptr = NULL;

	init_debug_ui(window, renderer, ig_ctx, ioptr, rom_path, bootrom_path);

	bool quit = false;
	bool run_once = false;
	bool set_run_once = false;
	SDL_Event e;

	std::ofstream gbd_log;

	uint64_t NOW = SDL_GetPerformanceCounter();
	uint64_t LAST = 0;

	double deltaTime = 0;
	double timer = 0;

	while (!quit) {
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();
		deltaTime = ((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());
		timer += deltaTime;


		if (use_gamepad && gamepad == NULL) {
			gamepad = get_first_gamepad();
		}

		if (emu.should_run) {
			if (create_gbd_log && emu.memory->in_bios == false) {
				if (!gbd_log.is_open()) {
					app_log.AddLog("opening gbd log\n");
					gbd_log.open("./gbd_log.txt");
				}
				char str_buf[200];
				sprintf(str_buf, "A:%02hX F:%02hX B:%02hX C:%02hX D:%02hX E:%02hX H:%02hX L:%02hX SP:%04hX PC:%04hX PCMEM:%02hX,%02hX,%02hX,%02hX\n",
					emu.cpu->registers.a,
					emu.cpu->registers.f,
					emu.cpu->registers.b,
					emu.cpu->registers.c,
					emu.cpu->registers.d,
					emu.cpu->registers.e,
					emu.cpu->registers.h,
					emu.cpu->registers.l,
					emu.cpu->registers.sp,
					emu.cpu->registers.pc,
					read8(emu.memory, emu.cpu->registers.pc),
					read8(emu.memory, emu.cpu->registers.pc + 1),
					read8(emu.memory, emu.cpu->registers.pc + 2),
					read8(emu.memory, emu.cpu->registers.pc + 3)
				);
				gbd_log << str_buf;
			}
			if (step(&emu) < 0) emu.should_run = false; // if step returns negative the operation failed to execute
			set_run_once = false;

		}

		if (breakpoints[emu.cpu->registers.pc]) {
			emu.should_run = false;
			if (!set_run_once) { // logic for only running a function once in draw debug ui
				run_once = true;
				set_run_once = true;
				app_log.AddLog("BREAKPOINT! 0x%04hX\n", emu.cpu->registers.pc);
			}
		}

		if (emu.should_run) {
			if (emu.should_draw) {
				while (SDL_PollEvent(&e)) {
					ImGui_ImplSDL3_ProcessEvent(&e);
					switch (e.type) {
					case SDL_EVENT_QUIT:
					case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
						goto end;
					case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
					case SDL_EVENT_GAMEPAD_BUTTON_UP:
						if (use_gamepad && gamepad) update_emu_controller(&emu, get_controller_state(gamepad)); // TODO make this logic less ugly. I think I should store SDL_Gamepad here instead of in emulator
						break;
					}
				}
				draw_debug_ui(window, renderer, ig_ctx, ioptr, &emu, &emulator_screen_rect, &tile_screen_rect, run_once);
				SDL_DestroyTexture(screen_tex);
				screen_tex = SDL_CreateTextureFromSurface(renderer, emu.gpu->screen);
				SDL_SetTextureScaleMode(screen_tex, SDL_SCALEMODE_NEAREST);
				SDL_DestroyTexture(tile_tex);
				tile_tex = SDL_CreateTextureFromSurface(renderer, emu.gpu->tile_screen);
				SDL_SetTextureScaleMode(tile_tex, SDL_SCALEMODE_NEAREST);
				emu.should_draw = false;
				SDL_SetRenderDrawColorFloat(renderer, clear_color.x, clear_color.y, clear_color.z, clear_color.w);
				SDL_RenderClear(renderer);
				ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
				SDL_RenderTexture(renderer, screen_tex, nullptr, &emulator_screen_rect);
				SDL_RenderTexture(renderer, tile_tex, nullptr, &tile_screen_rect);
				SDL_RenderPresent(renderer);
				SDL_Delay(1);
			}
			run_once = false;

		}
		else {
			if (timer > 16.6) {
				while (SDL_PollEvent(&e)) {
					ImGui_ImplSDL3_ProcessEvent(&e);
					switch (e.type) {
					case SDL_EVENT_QUIT:
					case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
						goto end;
					case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
					case SDL_EVENT_GAMEPAD_BUTTON_UP:
						if (use_gamepad && gamepad) update_emu_controller(&emu, get_controller_state(gamepad)); // TODO make this logic less ugly. I think I should store SDL_Gamepad here instead of in emulator
						break;
					}
				}
				draw_debug_ui(window, renderer, ig_ctx, ioptr, &emu, &emulator_screen_rect, &tile_screen_rect, run_once);
				SDL_SetRenderDrawColorFloat(renderer, clear_color.x, clear_color.y, clear_color.z, clear_color.w);
				SDL_RenderClear(renderer);
				ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
				SDL_RenderTexture(renderer, screen_tex, nullptr, &emulator_screen_rect);
				SDL_RenderTexture(renderer, tile_tex, nullptr, &tile_screen_rect);
				SDL_RenderPresent(renderer);
				timer = 0;
				SDL_Delay(1);
			}
		}
	}
end:
	if (create_gbd_log) gbd_log.close();
	if (gamepad) SDL_CloseGamepad(gamepad);
	SDL_DestroyTexture(screen_tex);
	SDL_DestroyTexture(tile_tex);
	destroy_emulator(&emu);
	destroy_debug_ui(window, renderer, ig_ctx, ioptr);
	return 0;
}
