#pragma once

#define SCREEN_WIDTH = 160
#define SCREEN_HEIGHT = 144

#define LCD_CONTROL = 0xFF40 // LCD control address
#define LCD_STATUS = 0xFF41 // LCD status address
#define LY = 0xFF44 // Current horizontal line address
#define LYC = 0xFF45 // Current horizonal line compare address
#define SCY = 0xFF42 // Scroll Y address
#define SCX = 0xFF43 // Scroll X address
#define WY = 0xFF4A // Window Y address
#define WX = 0xFF4B // Window X address
#define BGP = 0xFF47 // Background Palette address

// #define IF = 0xFF0F // Interrupt flag

#define BLACK = 0x252b25FF
#define DARK = 0x555a56FF
#define LIGHT = 0x5e785dFF
#define WHITE = 0x84d07dFF

typedef enum {
	HBLANK,
	VBLANK,
	OAM_ACCESS,
	VRAM_ACCESS
} gpu_mode;

