#pragma once
#include "../global_definitions.h"

u8 cart_read8(Cartridge* cart, u16 address);
void cart_write8(Cartridge* cart, u16 address, u8 data);
void set_current_rom_bank(Cartridge* cart, u8 data);
void print_cartridge(Cartridge* cart);