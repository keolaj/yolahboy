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

	u16 old_clock = emu->timer->clock;

	if ((old_clock & 0xFF) + t_cycles > 0xFF) {
		++(emu->mmu.Mmu[DIV]);
	}

	emu->timer->clock += t_cycles;
	u8 tac = emu->mmu.Mmu[TAC];

	bool tac_enable = tac & (1 << 2);
	u8 tac_mode = tac & 3;

	bool should_inc_tima = false;
	int bit_check = 0;

	switch (tac_mode) {
	case 0:
		bit_check = 9;
		break;
	case 1:
		bit_check = 3;
		break;
	case 2:
		bit_check = 5;
		break;
	case 3:
		bit_check = 7;
		break;
	}

	bool and_result = tac_enable && (emu->timer->clock & (1 << bit_check));

	if (emu->timer->old_and && !and_result) {
		should_inc_tima = true;
	}

	emu->timer->old_and = and_result;

	if (tac_enable && should_inc_tima) {
		if (emu->mmu.Mmu[TIMA] == 255) {
			emu->mmu.Mmu[IF] |= TIMER_INTERRUPT;
			emu->mmu.Mmu[TIMA] = emu->mmu.Mmu[TMA];
		}
		else {
			(emu->mmu.Mmu[TIMA])++;
		}
	}


}
