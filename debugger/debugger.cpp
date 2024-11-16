#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>

extern "C" {
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include "debugger.h"
#include "../components/global_definitions.h"
#include "../components/emulator.h"
#include "../components/controller/controller.h"
#include "../components/memory/memory.h"
#include "../components/cpu/operations.h"
#include "../components/cpu/operation_defitions.h"
#include "../components/gpu/gpu.h"
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
	}
	if (rom_path) {
		strcpy(rom_path_buf, rom_path);
	}
	if (bootrom_path) {
		strcpy(bootrom_path_buf, bootrom_path);
	}
}

extern ExampleAppLog app_log;
static bool create_gbd_log = false;
static bool use_gamepad = false;

static float DEBUGGER_X = 260;
static float DEBUGGER_Y = 323;
static float SCREEN_X = 336;
static float SCREEN_Y = 323;
static float TILE_X = 144;
static float TILE_Y = 323;
static float INSTRUCTION_X = 310;
static float INSTRUCTION_Y = 600;
static float TABS_X = DEBUGGER_X + SCREEN_X + TILE_X;
static float TABS_Y = 277;

static int WINDOW_WIDTH = DEBUGGER_X + SCREEN_X + TILE_X + INSTRUCTION_X;


void draw_debug_ui(SDL_Window* window, SDL_Renderer* renderer, ImGuiContext* ig_ctx, ImGuiIO* ioptr, Emulator* emu, SDL_FRect* emulator_screen_rect, SDL_FRect* tile_screen_rect, bool run_once) {

	static MemoryEditor ram_viewer;
	static ImGuiListClipper clipper;

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
					skip_bootrom(emu);
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
		//for (int i = 0; i < 0x10000; ++i) {
		//	emu->memory->memory[i] = read8(emu->memory, i);
		//}
	}

	ImGui::SameLine();
	if (ImGui::Button("RESET", { 40, 15 })) {
		destroy_emulator(emu);
		init_emulator(emu);
		if (load_bootrom(emu->memory, bootrom_path_buf) < 0) {
			skip_bootrom(emu);
		}
		load_rom(emu->memory, rom_path_buf);
	}

	if (ImGui::Button("STEP", { 40, 15 })) {
		step(emu);
	}

	ImGui::BeginChild("REGISTERS");

	char af_buf[10];
	char bc_buf[10];
	char de_buf[10];
	char hl_buf[10];
	char sp_buf[10];
	char pc_buf[10];
	char rom_bank_buf[20];
	char ram_bank_buf[20];
	char clock_buf[20];
	char div_buf[20];
	char ly_buf[20];

	sprintf_s(af_buf, "AF: %04hX", emu->cpu->registers.af);
	sprintf_s(bc_buf, "BC: %04hX", emu->cpu->registers.bc);
	sprintf_s(de_buf, "DE: %04hX", emu->cpu->registers.de);
	sprintf_s(hl_buf, "HL: %04hX", emu->cpu->registers.hl);
	sprintf_s(sp_buf, "SP: %04hX", emu->cpu->registers.sp);
	sprintf_s(pc_buf, "PC: %04hX", emu->cpu->registers.pc);
	sprintf_s(rom_bank_buf, "ROM BANK: %04hX", emu->memory->cartridge.rom_bank);
	sprintf_s(ram_bank_buf, "RAM BANK: %04hX", emu->memory->cartridge.ram_bank);
	sprintf_s(clock_buf, "CLOCK: %04hX", emu->timer->clock);
	sprintf_s(div_buf, "DIV: %02hX", emu->memory->memory[DIV]);

	ImGui::Text(af_buf);
	ImGui::SameLine();
	ImGui::Text(bc_buf);
	ImGui::Text(de_buf);
	ImGui::SameLine();
	ImGui::Text(hl_buf);
	ImGui::Text(sp_buf);
	ImGui::SameLine();
	ImGui::Text(pc_buf);
	ImGui::Text(rom_bank_buf);
	ImGui::SameLine();
	ImGui::Text(ram_bank_buf);
	ImGui::Text(div_buf);
	ImGui::SameLine();
	ImGui::Text(clock_buf);
	ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
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
			Operation* currentOp;
			if (emu->memory->cartridge.rom == NULL) {
				currentOp = &operations[0];
			}
			else {
				currentOp = &operations[read8(emu->memory, i)];
			}
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
			case MEM_READ_ADDR:
				clipper.DisplayEnd += 2;
				i += 2;
				sprintf(op_buf, "%04hX", read16(emu->memory, i - 1));
				ImGui::SameLine();
				ImGui::Text(op_buf);
				break;
			}
			switch (currentOp->dest_addr_mode) {
			case MEM_READ_ADDR_OFFSET:
				clipper.DisplayEnd += 1;
				i += 1;
				sprintf(op_buf, "%04hX", read8(emu->memory, i));

				ImGui::SameLine();
				ImGui::Text(op_buf);
				break;
			case MEM_READ_ADDR:
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
		//if (run_once) {
		//	for (int i = 0; i < 0x10000; ++i) {
		//		emu->memory->memory[i] = read8(emu->memory, i);
		//	}
		//}
		ram_viewer.DrawContents(emu->memory->memory, 0x10000);
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("LOG")) {
		// TODO
		app_log.Draw();
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("CARTRIDGE")) {
		char title_buf[0x11];
		char full_title[0x30];
		char type_buf[0x20];
		char rom_size_buf[0x30];
		char ram_size_buf[0x20];

		if (emu->memory->cartridge.rom != NULL) {
			Cartridge cart = emu->memory->cartridge;
			memcpy(title_buf, emu->memory->cartridge.rom + 0x134, 0x10);
			title_buf[0x10] = '\0';
			sprintf_s(full_title, "TITLE: %s", title_buf);
			sprintf_s(type_buf, "TYPE: %d", cart.type);
			sprintf_s(rom_size_buf, "ROM SIZE: 0x%X", cart.rom_size);
			sprintf_s(ram_size_buf, "RAM SIZE: 0x%X", cart.ram_size);

			ImGui::Text(full_title);
			ImGui::Text(type_buf);
			ImGui::Text(rom_size_buf);
			ImGui::Text(ram_size_buf);
		}

		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("GPU")) {
		
		ImVec2 contentSize = ImGui::GetContentRegionAvail();

		ImGui::BeginChild("LCDC", {contentSize.x / 3, contentSize.y});
		ImGui::Text("LCD CONTROL (LCDC FF40)");
		ImGui::Separator();
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 15, 3 });
		if (ImGui::BeginTable("LCDC TABLE", 2)) {

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			bool lcd_on = emu->memory->memory[LCD_CONTROL] & (1 << 7);
			ImGui::Checkbox("LCD", &lcd_on);
			ImGui::TableNextColumn();
			ImGui::Text("%s", (lcd_on ? "ON" : "OFF"));
			ImGui::Separator();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			bool window_area = (emu->memory->memory[LCD_CONTROL] & (1 << 6));
			ImGui::Checkbox("WINDOW MAP AREA", &window_area);
			ImGui::TableNextColumn();
			ImGui::Text("%s", window_area ? "9C00" : "9800");
			ImGui::Separator();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			bool window_enable = emu->memory->memory[LCD_CONTROL] & (1 << 5);
			ImGui::Checkbox("WINDOW ENABLE", &window_enable);
			ImGui::TableNextColumn();
			ImGui::Text("%s", (window_enable ? "ON" : "OFF"));
			ImGui::Separator();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			bool tile_area = emu->memory->memory[LCD_CONTROL] & (1 << 4);
			ImGui::Checkbox("TILE DATA AREA", &tile_area);
			ImGui::TableNextColumn();
			ImGui::Text("%s", (tile_area ? "8000" : "8800"));
			ImGui::Separator();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			bool bg_area = emu->memory->memory[LCD_CONTROL] & (1 << 3);
			ImGui::Checkbox("BG MAP AREA", &bg_area);
			ImGui::TableNextColumn();
			ImGui::Text("%s", (bg_area ? "9800" : "9C00"));
			ImGui::Separator();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			bool obj_mode = emu->memory->memory[LCD_CONTROL] & (1 << 2);
			ImGui::Checkbox("OBJ SIZE", &obj_mode);
			ImGui::TableNextColumn();
			ImGui::Text("%s", (obj_mode ? "8x16" : "8x8"));
			ImGui::Separator();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			bool obj_enable = emu->memory->memory[LCD_CONTROL] & (1 << 1);
			ImGui::Checkbox("OBJ ENABLE", &obj_enable);
			ImGui::TableNextColumn();
			ImGui::Text("%s", (obj_enable ? "ON" : "OFF"));
			ImGui::Separator();
			
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			bool bg_enable = emu->memory->memory[LCD_CONTROL] & (1 << 0);
			ImGui::Checkbox("BG ENABLE", &bg_enable);
			ImGui::TableNextColumn();
			ImGui::Text("%s", (bg_enable ? "ON" : "OFF"));
			ImGui::EndTable();
		}
		ImGui::PopStyleVar();
		ImGui::EndChild();

		ImGui::SameLine();
		ImGui::BeginChild("STAT", {contentSize.x / 3, contentSize.y});
		ImGui::Text("LCD STATUS (STAT FF41)");
		ImGui::Separator();
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 25, 2 });
		if (ImGui::BeginTable("STAT TABLE", 2)) {


			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			bool lyc_ly_intr = (emu->memory->memory[LCD_STATUS] & (1 << 6));
			ImGui::Checkbox("LYC == LY INTERRUPT", &lyc_ly_intr);
			ImGui::TableNextColumn();
			ImGui::Text("%s", lyc_ly_intr ? "ON" : "OFF");
			ImGui::Separator();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			bool mode_2_intr = emu->memory->memory[LCD_STATUS] & (1 << 5);
			ImGui::Checkbox("OAM INTERRUPT", &mode_2_intr);
			ImGui::TableNextColumn();
			ImGui::Text("%s", (mode_2_intr ? "ON" : "OFF"));
			ImGui::Separator();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			bool mode_1_intr = emu->memory->memory[LCD_STATUS] & (1 << 4);
			ImGui::Checkbox("VBLANK INTERRTUPT", &mode_1_intr);
			ImGui::TableNextColumn();
			ImGui::Text("%s", (mode_1_intr ? "ON" : "OFF"));
			ImGui::Separator();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			bool mode_0_intr = emu->memory->memory[LCD_STATUS] & (1 << 3);
			ImGui::Checkbox("HBLANK INTERRUPT", &mode_0_intr);
			ImGui::TableNextColumn();
			ImGui::Text("%s", (mode_0_intr ? "ON" : "OFF"));
			ImGui::Separator();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			bool ly_lyc = emu->memory->memory[LCD_STATUS] & (1 << 2);
			ImGui::Checkbox("LYC == LY", &ly_lyc);
			ImGui::TableNextColumn();
			ImGui::Text("%s", (ly_lyc ? "TRUE" : "FALSE"));
			ImGui::Separator();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			u8 ppu_mode = emu->memory->memory[LCD_STATUS] & 3;
			ImGui::Text("GPU MODE", &ppu_mode);
			ImGui::TableNextColumn();
			ImGui::Text("%d", ppu_mode);
			ImGui::Separator();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			u8 gpu_clocks = emu->gpu->clock;
			ImGui::Text("GPU CLOCKS", &gpu_clocks);
			ImGui::TableNextColumn();
			ImGui::Text("%d", gpu_clocks);
			ImGui::Separator();

			ImGui::EndTable();
			
		}
		ImGui::PopStyleVar();
		ImGui::EndChild();

		

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
				if (load_bootrom(emu->memory, bootrom_path_buf) < 0) {
					app_log.AddLog("Couldn't load bootrom\n");
					skip_bootrom(emu);
				}
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
				if (load_rom(emu->memory, rom_path_buf) < 0) {
					app_log.AddLog("Couldn't load rom!\n");
				}
			}

			// close
			ImGuiFileDialog::Instance()->Close();
		}


		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();
	ImGui::End();
	if (run_once) {
	}

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
			app_log.AddLog("Unable to open game controller! SDL Error: %s\n", SDL_GetError());
		}
	}
	else {
		app_log.AddLog("No Controllers Connected!\n");
	}
	SDL_free(joysticks);
	return ret;
}

int debugger_run(char* rom_path, char* bootrom_path) {

	SDL_Window* window = SDL_CreateWindow("Yolahboy Debugger", WINDOW_WIDTH, 600, 0);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
	SDL_Texture* screen_tex = NULL;
	SDL_Texture* tile_tex = NULL;
	SDL_Gamepad* gamepad = get_first_gamepad();
	SDL_FRect emulator_screen_rect{ 0, 0, 160, 144 };
	SDL_FRect tile_screen_rect{ 0, 0, 128, 192 };

	KeyboardConfig k_config{};
	k_config.a = SDL_SCANCODE_J;
	k_config.b = SDL_SCANCODE_K;
	k_config.start = SDL_SCANCODE_I;
	k_config.select = SDL_SCANCODE_U;
	k_config.up = SDL_SCANCODE_W;
	k_config.left = SDL_SCANCODE_A;
	k_config.down = SDL_SCANCODE_S;
	k_config.right = SDL_SCANCODE_D;

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

	// app_log.AddLog("Render driver: %s\nVsync number: %d", SDL_GetStringProperty(props, SDL_PROP_RENDERER_NAME_STRING, NULL), SDL_GetNumberProperty(props, SDL_PROP_RENDERER_VSYNC_NUMBER, 0));

	Emulator emu;
	if (init_emulator(&emu) < 0) {
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		destroy_emulator(&emu);
		return -1;
	}

	if (bootrom_path) {
		if (load_bootrom(emu.memory, bootrom_path) < 0) {
			app_log.AddLog("Couldn't load bootrom!\n");
			skip_bootrom(&emu);
		}
	}
	if (rom_path) {
		if (load_rom(emu.memory, rom_path) < 0) {
			app_log.AddLog("Couldn't load rom!\n");
		}
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


		//if (use_gamepad && gamepad == NULL) {
		//	gamepad = get_first_gamepad();
		//}

		if (emu.should_run) {
			set_use_gbd_log(emu.memory, create_gbd_log);
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
		}

		if (breakpoints[emu.cpu->registers.pc]) {
			emu.should_run = false;
			if (!set_run_once) { // logic for only running a function once in draw debug ui
				SDL_DestroyTexture(tile_tex);
				write_tile_buffer_to_screen(emu.gpu);
				tile_tex = SDL_CreateTextureFromSurface(renderer, emu.gpu->tile_screen);

				run_once = true;
				set_run_once = true;
				app_log.AddLog("BREAKPOINT! 0x%04hX\n", emu.cpu->registers.pc);
			}
		}

		if (emu.should_run) { // emulator controls rendering
			if (emu.gpu->should_draw) {
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
					case SDL_EVENT_KEY_DOWN:
					case SDL_EVENT_KEY_UP:
						if (!use_gamepad) update_emu_controller(&emu, get_keyboard_state(emu.controller, &e, &k_config));
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
			}
			set_run_once = false;
		
		}
		else { // emulator isn't running and we go to 60fps
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
				run_once = false;

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

