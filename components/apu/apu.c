#include "apu.h"
#include <string.h>

static bool duty_cycle_00[8] = { 1, 1, 1, 1, 1, 1, 1, 0};
static bool duty_cycle_01[8] = { 0, 1, 1, 1, 1, 1, 1, 0};
static bool duty_cycle_10[8] = { 0, 1, 1, 1, 1, 0, 0, 0};
static bool duty_cycle_11[8] = { 1, 0, 0, 0, 0, 0, 0, 1};

static bool duty_cycles[4][8] = { duty_cycle_00, duty_cycle_01, duty_cycle_10, duty_cycle_11 };

Apu* create_apu(int sample_rate, int buffer_size) {
	Apu* ret = (Apu*)malloc(sizeof(Apu));
	memset(ret, 0, sizeof(Apu));

	ret->buffer_size = buffer_size;

}

void destroy_apu(Apu* apu) {

}

void channel_1_step(Apu* apu, u8 cycles) {

}

void step(Apu* apu, u8 cycles) {
	if (apu->nr52 & 0b10000000) { // if audio enabled
		
	}
	else {

	}
}

