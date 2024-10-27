#pragma once
#include <stdbool.h>
#include "SDL.h"
#include <stdlib.h>

#define IF 0xFF0F

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
	u8 bios[0x100];
	u8 memory[0x10000];
	bool in_bios;
	struct Gpu* gpu;
} Memory;

typedef u8** Tile;

typedef enum {
	HBLANK,
	VBLANK,
	OAM_ACCESS,
	VRAM_ACCESS
} gpu_mode;

typedef struct {
	int line;
	int clock;
	u32 framebuffer[23040];

	Memory* mem;

	Tile* tiles;
	SDL_Surface* screen;
	SDL_Surface* tile_screen;

	gpu_mode mode;
} Gpu;

