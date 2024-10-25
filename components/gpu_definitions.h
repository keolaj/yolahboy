#pragma once

constexpr int SCREEN_WIDTH = 160;
constexpr int SCREEN_HEIGHT = 144;

constexpr int LCD_CONTROL = 0xFF40; // LCD control address
constexpr int LCD_STATUS = 0xFF41; // LCD status address
constexpr int LY = 0xFF44; // Current horizontal line address
constexpr int LYC = 0xFF45; // Current horizonal line compare address
constexpr int SCY = 0xFF42; // Scroll Y address
constexpr int SCX = 0xFF43; // Scroll X address
constexpr int WY = 0xFF4A; // Window Y address
constexpr int WX = 0xFF4B; // Window X address
constexpr int BGP = 0xFF47; // Background Palette address

// constexpr int IF = 0xFF0F; // Interrupt flag

constexpr int BLACK = 0x252b25FF;
constexpr int DARK = 0x555a56FF;
constexpr int LIGHT = 0x5e785dFF;
constexpr int WHITE = 0x84d07dFF;

enum GpuMode {
	HBLANK,
	VBLANK,
	OAM_ACCESS,
	VRAM_ACCESS
};

