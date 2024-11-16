#include <stdio.h>
#include <string.h>
#include "controller.h"
#include <SDL3/SDL_gamepad.h>


void print_controller(Controller c) {
	printf("CONTROLLER STATE:\nA: %d\nB: %d\nUP: %d\nDOWN: %d\nLEFT: %d\nRIGHT: %d\nSELECT: %d\nSTART: %d\n", c.a, c.b, c.up, c.down, c.left, c.right, c.select, c.start);
}

Controller get_controller_state(SDL_Gamepad* sdl_c) { // doesn't need previous state as every button is set
	Controller c;
	c.a = SDL_GetGamepadButton(sdl_c, SDL_GAMEPAD_BUTTON_SOUTH);
	c.b = SDL_GetGamepadButton(sdl_c, SDL_GAMEPAD_BUTTON_SOUTH);
	c.select = SDL_GetGamepadButton(sdl_c, SDL_GAMEPAD_BUTTON_BACK);
	c.start = SDL_GetGamepadButton(sdl_c, SDL_GAMEPAD_BUTTON_START);
	c.up = SDL_GetGamepadButton(sdl_c, SDL_GAMEPAD_BUTTON_DPAD_UP);
	c.down = SDL_GetGamepadButton(sdl_c, SDL_GAMEPAD_BUTTON_DPAD_DOWN);
	c.left = SDL_GetGamepadButton(sdl_c, SDL_GAMEPAD_BUTTON_DPAD_LEFT);
	c.right = SDL_GetGamepadButton(sdl_c, SDL_GAMEPAD_BUTTON_DPAD_RIGHT);

	return c;
}

Controller get_keyboard_state(Controller prev, SDL_Event* e, KeyboardConfig* config) {
	
	Controller c = prev;

	if (e->key.scancode == config->a) c.a = e->key.down;	
	if (e->key.scancode == config->b) c.b = e->key.down;
	if (e->key.scancode == config->select) c.select = e->key.down;
	if (e->key.scancode == config->start) c.start = e->key.down;
	if (e->key.scancode == config->up) c.up = e->key.down;
	if (e->key.scancode == config->down) c.down = e->key.down;
	if (e->key.scancode == config->left) c.left = e->key.down;
	if (e->key.scancode == config->right) c.right = e->key.down;

	return c;
}

Controller* create_controller() {
	Controller* ret = malloc(sizeof(Controller));
	if (ret == NULL) {
		printf("Could not initialize controller!");
		return NULL;
	}
	memset(ret, 0, sizeof(Controller));
	return ret;
}

u8 joypad_return(Controller controller, u8 data) {
	bool select_buttons = data & (1 << 5);
	bool select_dpad = data & (1 << 4);
	u8 ret = 0b00001111;
	if (select_buttons && select_dpad) return 0x0F;
	if (!select_buttons) {
		ret |= 0b00100000;
		if (controller.start) ret &= ~(1 << 3);
		if (controller.select) ret &= ~(1 << 2);
		if (controller.b) ret &= ~(1 << 1);
		if (controller.a) ret &= ~(1);
	}
	if (!select_dpad) {
		ret |= 0b00010000;
		if (controller.down) ret &= ~(1 << 3);
		if (controller.up) ret &= ~(1 << 2);
		if (controller.left) ret &= ~(1 << 1);
		if (controller.right) ret &= ~(1);
	}
	return ret;
}

void destroy_controller(Controller* controller) {
	if (controller == NULL) return;
	free(controller);
}