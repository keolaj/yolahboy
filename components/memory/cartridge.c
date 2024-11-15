#include "cartridge.h"
#include "../../debugger/imgui_custom_widget_wrapper.h"
#include <assert.h>

#define BANK_SELECT_HIGH 0x7FFF
#define BANK_SELECT_LOW 0x6000
#define RAM_BANK_HIGH 0x5FFF
#define RAM_BANK_LOW 0x4000
#define RAM_ENABLE_HIGH 0x1FFF
#define RAM_ENABLE_LOW 0x0000
#define ROM_BANK_HIGH 0x3FFF
#define ROM_BANK_LOW 0x2000

u8 cart_read8(Cartridge* cart, u16 address) {
	switch (cart->type) {
	case ROM_ONLY: {
		if (address < 0x8000) {
			return cart->rom[address];
		}
		return 0xFF;
	}
	case MBC1:
	case MBC1_RAM:
	case MBC1_RAM_BATTERY: {
		if (address < 0x4000) { // bank 0
			return cart->rom[address];
		}
		if (address < 0x8000) { // selectable rom bank (this is working so far)
			u8 current_bank = cart->rom_bank;
			u8 num_banks = (cart->rom_size / BANKSIZE);
			current_bank &= (num_banks - 1);
			int offset = current_bank * BANKSIZE;
			u16 newaddr = address - 0x4000;
			return cart->rom[offset + newaddr];
		}
		if (address >= 0xA000 && address < 0xC000) { // ram read
			if (cart->ram_enabled && cart->ram != NULL) {
				int newaddr = address - 0xA000;
				int offset = 0;
				if (cart->banking_mode == BANKMODEADVANCED) {
					offset = (cart->ram_bank * 0x2000);
				}
				else {
					offset = 0;
				}
				return cart->ram[offset + newaddr];
			}
			else { // if ram disabled return 0xFF
				return 0xFF;
			}
		}

	}
	}
}

void cart_write8(Cartridge* cart, u16 address, u8 data) {
	if (address <= 0x1FFF) { // ram enable register
		cart->ram_enabled = (data & 0x0F) == 0xA;
		return;
	}
	if (address >= 0x2000 && address <= 0x3FFF) { // rom bank number
		switch (cart->type) {
		case ROM_ONLY:
			return;
		case MBC1:
		case MBC1_RAM:
		case MBC1_RAM_BATTERY:
			data = data & 0b00011111;
			if (data == 0 || data == 0x20 || data == 0x40 || data == 0x60) data += 1;
			cart->rom_bank = data;
			return;
		default:
			return;
		}
	}
	if (address >= 0x4000 && address <= 0x5FFF) { // ram bank number or upper bits of rom bank number
		cart->ram_bank = data & 0b00000011;
	}
	if (address >= 0x6000 && address <= 0x7FFF) { // bank mode select
		if ((data & 0b00000001) == 1) {
			cart->banking_mode = BANKMODEADVANCED;
		}
		else {
			cart->banking_mode = BANKMODESIMPLE;
		}
		return;
	}
	if (address >= 0xA000 && address <= 0xBFFF) { // ram write
		if (cart->ram_enabled && cart->ram != NULL) {
			if (cart->banking_mode == BANKMODESIMPLE) {
				int newaddr = address - 0xA000;
				cart->ram[newaddr] = data;
			}
			else {
				int offset = cart->ram_bank * 0x2000;
				int newaddr = address - 0xA000;
				cart->ram[offset + newaddr] = data;
			}
		}
	}
}

void set_current_rom_bank(Cartridge* cart, u8 data) {
	cart->rom_bank = data & 0b00011111;
}

void print_cartridge(Cartridge* cart) {
	char title[TITLE_SIZE + 1];
	strcpy_s(title, TITLE_SIZE, cart->rom[0x0134]);
	title[TITLE_SIZE] = '/0';

	AddLog("CARTRIDGE:\nTITLE: %s\nCARTRIDGE_TYPE: %d", title, cart->rom[CARTRIDGE_TYPE]);
}