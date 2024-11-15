#include "timer.h"
#include "../debugger/imgui_custom_widget_wrapper.h"

Timer* create_timer() {
	Timer* ret = (Timer*)malloc(sizeof(Timer));
	if (ret == NULL) {
		AddLog("Couldn't initialize Timer!\n");
		return NULL;
	}
	ret->clock = 0;
	return ret;
}

static u16 freq_divider[] = { 1024, 16, 64, 256 };

void tick(Emulator* emu, int t_cycles) {
	t_cycles = t_cycles / 4;

	u16 old_clock = emu->timer->clock;

	if ((old_clock & 0x00FF) + t_cycles > 0xFF) {
		++emu->memory->memory[DIV];
	}

	emu->timer->clock += t_cycles;
	u8 tac = emu->memory->memory[TAC];

	bool tac_enable = (tac & (1 << 2) >> 2);
	u8 tac_mode = tac & 3;

	bool should_inc_tima = false;

	if (tac_enable) {
		switch (tac_mode) {
		case MODE256:
			if (emu->timer->clock % 256 == 0) {
				should_inc_tima = true;
			}
			break;

		case MODE4:
			if (emu->timer->clock % 4 == 0) {
				should_inc_tima = true;
			}	
			break;
		case MODE16:
			if (emu->timer->clock % 16 == 0) {
				should_inc_tima = true;
			}
			break;
		case MODE64:
			if (emu->timer->clock % 64 == 0) {
				should_inc_tima = true;
			}
			break;
		}
		if (should_inc_tima) {
			++emu->memory->memory[TIMA];
			if (emu->memory->memory[TIMA] == 0) {
				emu->memory->memory[IF] |= TIMER_INTERRUPT;
				emu->memory->memory[TIMA] = emu->memory->memory[TMA];
			}
		}
	}


}