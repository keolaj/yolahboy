#include <string.h>
#include <stdlib.h>
#include "apu.h"
#include "../debugger/imgui_custom_widget_wrapper.h"

static bool duty_cycle_00[16] = { 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0};
static bool duty_cycle_01[16] = { 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0};
static bool duty_cycle_10[16] = { 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0};
static bool duty_cycle_11[16] = { 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1};

static bool duty_cycles[4][16] = { 
	{ 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0},
	{ 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0},
	{ 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
	{ 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1}
};

Apu* create_apu(int sample_rate, int buffer_size) {
	Apu* ret = (Apu*)malloc(sizeof(Apu));
	if (ret == NULL) {
		AddLog("Couldn't allocate Apu!\n");
		return NULL;
	}
	memset(ret, 0, sizeof(Apu));

	ret->buffer_size = buffer_size;
	ret->sample_rate = sample_rate;
	for (int i = 0; i < 4; ++i) {
		ret->channel[i].left_buffer = (float*)malloc(sizeof(float) * buffer_size);
		ret->channel[i].right_buffer = (float*)malloc(sizeof(float) * buffer_size);
		if (ret->channel[i].left_buffer == NULL || ret->channel[i].right_buffer == NULL) {
			AddLog("Couldn't allocate Channel %d buffer!\n", i);
			destroy_apu(ret);
			return NULL;
		}
		memset(ret->channel[i].left_buffer, 0, buffer_size);
		memset(ret->channel[i].right_buffer, 0, buffer_size);
	}
	ret->buffer1 = (float*)malloc(sizeof(float) * buffer_size * 2);
	ret->buffer2 = (float*)malloc(sizeof(float) * buffer_size * 2);

	if (ret->buffer1 == NULL || ret->buffer2 == NULL) {
		AddLog("Couldn't allocate Apu buffer\n");
		destroy_apu(ret);
		return NULL;
	}
	memset(ret->buffer1, 0, buffer_size * 2);
	memset(ret->buffer2, 0, buffer_size * 2);

	return ret;
}

void destroy_apu(Apu* apu) {
	for (int i = 0; i < 4; ++i) {
		if (apu->channel[i].left_buffer != NULL) {
			free(apu->channel[i].left_buffer);
		}
		if (apu->channel[i].right_buffer != NULL) {
			free(apu->channel[i].right_buffer);
		}
	}
	if (apu->buffer1 != NULL) free(apu->buffer1);
	if (apu->buffer2 != NULL) free(apu->buffer2);
}

float* swap_buffer(Apu* apu) {
	if (apu->use_buffer1) {
		apu->use_buffer1 = !apu->use_buffer1;
		return apu->buffer1;
	}
	else {
		apu->use_buffer1 = !apu->use_buffer1;
		return apu->buffer2;
	}
}

float* get_buffer(Apu* apu) {
	if (apu->use_buffer1) {
		return apu->buffer1;
	}
	else {
		return apu->buffer2;
	}
}

void channel_1_step(Apu* apu, u8 cycles) {
	apu->channel[0].divider -= cycles;
	apu->channel[0].sample_counter += cycles * apu->sample_rate;
	if (apu->channel[0].sample_counter > 1048576) {
		apu->channel[0].sample_counter -= 1048576;
		apu->channel[0].left_buffer[apu->buffer_position++] = duty_cycles[(apu->nr21 & 0b11000000) >> 6][apu->channel[0].wave_index] ? (float)1 : (float)0;
		apu->channel[0].sample_counter = 0;
	}
	if (apu->channel[0].divider < 0) {
		apu->channel[0].wave_index++;
		if (apu->channel[0].wave_index > 15) apu->channel[0].wave_index = 0;
		apu->channel[0].divider = (2048 - apu->channel[0].frequency_timer) + apu->channel[0].divider;
	}

}

void write_channel_buffers_to_buffer(Apu* apu) {
	float* buf = apu->use_buffer1 ? apu->buffer2 : apu->buffer1;
	for (int i = 0; i < (apu->buffer_size * 2); i += 2) {
		buf[i] = apu->channel[0].left_buffer[i / 2];
		buf[i + 1] = apu->channel[0].left_buffer[i / 2];
	}
}

void step_apu(Apu* apu, u8 cycles) {
	if (apu->nr52 & 0b10000000) { // if audio enabled
		
		for (int i = 0; i < cycles; ++i) { // every m cycle
			if (apu->channel[0].enabled) {
				channel_1_step(apu, 1);
			}
		}

		if (apu->buffer_position >= apu->buffer_size) {
			write_channel_buffers_to_buffer(apu);
			apu->buffer_position = 0;
			apu->use_buffer1 = !apu->use_buffer1;
			apu->buffer_full = true;
		}
		apu->clock += cycles;
	}
	else {
		apu->clock = 0;
	}
}

