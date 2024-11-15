#include "cartridge.h"
#include "../../debugger/imgui_custom_widget_wrapper.h"


u8 cart_read8(Cartridge* cart, u16 address) {
	switch (cart->type) {
	case ROM_ONLY: {
		if (address < 0x8000) {
			return cart->rom[address];
		}
		return 0xFF;
	}
	case MBC1_RAM:
	case MBC1_RAM_BATTERY: {
		if (address < 0x4000) { // bank 0
			return cart->rom[address];
		}
		if (address < 0x8000) { // selectable rom bank
			address -= 0x4000;
			return cart->rom[cart->rom_bank * BANKSIZE + address];
		}
		if (address >= 0xA000 && address < 0xC000) {
			if (cart->ram_enabled) {
				address -= 0xA000;
				return cart->ram[cart->ram_bank * BANKSIZE + address];
			}
			else { // if ram disabled return 0xFF
				return 0xFF;
			}
		}
		
	}
	}
}

void cart_write8(Cartridge* cart, u16 address, u8 data) {
	if (address < 0x2000) {
		data = data & 0x0F;
		if (data == 0xA) {
			cart->ram_enabled = true;
		}
		else {
			cart->ram_enabled = false;
		}
		return;
	}
	if (address < 0x4000) {
		data = data & 0b00011111;
		if (data == 0) data = 1;
		cart->rom_bank = data;
		return;
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