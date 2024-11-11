#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <SDL3/SDL.h>

#define MAX_BREAKPOINTS 0x100

#define IF 0xFF0F
#define IE 0xFFFF

#define DMA 0xFF46

#define OBP0 0xFF48
#define OBP1 0xFF49


#define VBLANK_INTERRUPT 1
#define VBLANK_ADDRESS 0x40

#define LCDSTAT_INTERRUPT 1 << 1
#define LCDSTAT_ADDRESS 0x48

#define TIMER_INTERRUPT 1 << 2
#define TIMER_ADDRESS 0x50

#define SERIAL_INTERRUPT 1 << 3
#define SERIAL_ADDRESS 0x58

#define JOYPAD_INTERRUPT 1 << 4
#define JOYPAD_ADDRESS 0x60

typedef unsigned char u8;
typedef char i8;
typedef short i16;
typedef unsigned short u16;
typedef unsigned int u32;

#define FLAG_ZERO 0b10000000
#define FLAG_SUB 0b01000000
#define FLAG_HALFCARRY 0b00100000
#define FLAG_CARRY 0b00010000

typedef struct {
	bool up;
	bool down;
	bool left;
	bool right;

	bool start;
	bool select;

	bool a;
	bool b;
} Controller;

typedef enum {
	ROM_ONLY,
	MBC1,
} MBC_TYPE;

typedef struct {
	u8* rom;
	u8* ram;
} Cartridge;

typedef struct mem_ctx Memory;
typedef struct gpu_ctx Gpu;

typedef struct mem_ctx {
	u8 bios[0x100];
	u8 memory[0x10000];
	Cartridge cartridge;
	bool in_bios;
	Gpu* gpu;
	Controller controller;
} Memory;

typedef struct {
	union {
		struct {
			u8 f;
			u8 a;
		};
		u16 af;
	};
	union {
		struct {
			u8 c;
			u8 b;
		};
		u16 bc;
	};
	union {
		struct {
			u8 e;
			u8 d;
		};
		u16 de;
	};
	union {
		struct {
			u8 l;
			u8 h;
		};
		u16 hl;
	};
	u16 sp;
	u16 pc;
} Registers;

typedef struct {
	Registers registers;
	bool IME;
	bool should_update_IME;
	bool update_IME_value;
	int update_IME_counter;
} Cpu;

typedef struct {
	int m_cycles;
	int t_cycles;
} Cycles;


typedef u8** Tile;

typedef enum {
	HBLANK,
	VBLANK,
	OAM_ACCESS,
	VRAM_ACCESS
} gpu_mode;

struct gpu_ctx {
	int line;
	int clock;
	u32 framebuffer[23040];

	Memory* mem;

	Tile* tiles;
	SDL_Surface* screen;
	SDL_Surface* tile_screen;

	gpu_mode mode;
	bool drawline;
};

typedef struct {
	Cpu* cpu;
	Memory* memory;
	Gpu* gpu;
	Controller* controller;
	// SDL_Window* emulator_window;
	// SDL_Window* tile_window;
	// SDL_Renderer* emulator_renderer;
	// SDL_Renderer* tile_renderer;
	SDL_Gamepad* game_controller;
	int* breakpoints;
	bool should_quit;
	bool should_run;
	bool draw;
} Emulator;
