#include "global_definitions.h"
#include "SDL.h"


Controller get_controller_state(SDL_GameController* sdl_c);
u8 joypad_return(Controller controller, u8 data);
void joypad_write(Controller controller, u8 data);
