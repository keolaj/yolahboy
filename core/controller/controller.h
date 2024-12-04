#pragma once

#include "../global_definitions.h"
#include <SDL3/SDL_gamepad.h>

Controller* create_controller();
void print_controller(Controller c);
Controller get_controller_state(SDL_Gamepad* sdl_c);
u8 joypad_return(Controller controller, u8 data);
void joypad_write(Controller controller, u8 data);

void destroy_controller(Controller* controller);
