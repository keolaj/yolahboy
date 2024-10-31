#include <stdio.h>
#include <string.h>
#include "controller.h"

Controller get_controller_state(SDL_GameController* sdl_c) {
	Controller c;
	c.a = SDL_GameControllerGetButton(sdl_c, SDL_CONTROLLER_BUTTON_A);
	c.b = SDL_GameControllerGetButton(sdl_c, SDL_CONTROLLER_BUTTON_B);
	c.select = SDL_GameControllerGetButton(sdl_c, SDL_CONTROLLER_BUTTON_GUIDE);
	c.start = SDL_GameControllerGetButton(sdl_c, SDL_CONTROLLER_BUTTON_START);
	c.up = SDL_GameControllerGetButton(sdl_c, SDL_CONTROLLER_BUTTON_DPAD_UP);
	c.down = SDL_GameControllerGetButton(sdl_c, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
	c.left = SDL_GameControllerGetButton(sdl_c, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
	c.right = SDL_GameControllerGetButton(sdl_c, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

	return c;
}

Controller* create_controller(){
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
	
	if (select_buttons) {
		ret |= 0b00100000;
		if (controller.start) ret &= ~(1 << 3);
		if (controller.select) ret &= ~(1 << 2);
		if (controller.b) ret &= ~(1 << 1);
		if (controller.a) ret &= ~(1);
	}
	if (select_dpad) {
		ret |= 0b00010000;
		if (controller.down) ret &= ~(1 << 3);
		if (controller.up) ret &= ~(1 << 2);
		if (controller.left) ret &= ~(1 << 1);
		if (controller.right) ret &= ~(1);
	}
	return ret;
}

