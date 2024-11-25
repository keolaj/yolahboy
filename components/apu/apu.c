#include <string.h>
#include <stdlib.h>
#include "apu.h"
#include "../debugger/imgui_custom_widget_wrapper.h"

static bool duty_cycles[4][16] = {
	{ 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0 },
	{ 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0 },
	{ 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0 },
	{ 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1 }
};

// APU SETUP FUNCTIONS
Apu* create_apu(int sample_rate, int buffer_size) {
	Apu* apu = (Apu*)malloc(sizeof(Apu));
	if (apu == NULL) {
		AddLog("Couldn't allocate Apu!\n");
		return NULL;
	}

	apu->buffer_size = buffer_size;
	apu->sample_rate = sample_rate;

	for (int i = 0; i < 4; ++i) { // Allocate buffers for each channel
		apu->channel[i].left_buffer = (float*)malloc(sizeof(float) * buffer_size);
		apu->channel[i].right_buffer = (float*)malloc(sizeof(float) * buffer_size);
		if (apu->channel[i].left_buffer == NULL || apu->channel[i].right_buffer == NULL) {
			AddLog("Couldn't allocate Channel %d buffer!\n", i);
			destroy_apu(apu);
			return NULL;
		}
	}
	apu->buffer1 = (float*)malloc(sizeof(float) * buffer_size * 2); // Allocate main buffers
	apu->buffer2 = (float*)malloc(sizeof(float) * buffer_size * 2);

	if (apu->buffer1 == NULL || apu->buffer2 == NULL) {
		AddLog("Couldn't allocate Apu buffer\n");
		destroy_apu(apu);
		return NULL;
	}

	init_apu(apu);

	return apu;
}

void init_apu(Apu* apu) {
	int buffer_size = apu->buffer_size;
	int sample_rate = apu->sample_rate;

	memset(apu, 0, 0x40); // This resets all registers while leaving channel and buffers intact
	for (int i = 0; i < 4; ++i) {
		memset(&apu->channel[i], 0, 0x8); // This resets channel and leaves buffers intact
		memset(apu->channel[i].left_buffer, 0.0f, buffer_size * sizeof(float));
		memset(apu->channel[i].right_buffer, 0.0f, buffer_size * sizeof(float));
	}
	memset(apu->buffer1, 0, buffer_size * 2 * sizeof(float));
	memset(apu->buffer2, 0, buffer_size * 2 * sizeof(float));
	
	apu->buffer_size = buffer_size;
	apu->sample_rate = sample_rate;
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

void div_apu_step(Apu* apu, u8 cycles) {

}

// CHANNEL FUNCTIONS
void channel_1_step(Apu* apu, u8 cycles) {
	if (apu->channel[0].enabled) {

	}
}
void channel_1_sample(Apu* apu) {
	apu->channel[0].left_buffer[apu->buffer_position]  = duty_cycles[(apu->nr21 & 0b11000000) >> 6][apu->channel[0].wave_index] ? (float)1 : (float)0;
	apu->channel[0].right_buffer[apu->buffer_position] = duty_cycles[(apu->nr21 & 0b11000000) >> 6][apu->channel[0].wave_index] ? (float)1 : (float)0;
}

void channel_2_step(Apu* apu, u8 cycles) {
	
}
void channel_2_sample(Apu* apu) {

}

// BUFFER FUNCTIONS
void write_channels_to_buffer(Apu* apu) {
	float* buf = apu->use_buffer1 ? apu->buffer2 : apu->buffer1;
	for (int i = 0; i < (apu->buffer_size * 2); i += 2) { // interleave left and right channels
		buf[i] = apu->channel[0].left_buffer[i / 2];
		buf[i + 1] = apu->channel[0].right_buffer[i / 2];
	}
}

void handle_buffers(Apu* apu) {
	if (apu->buffer_position >= apu->buffer_size) {
		write_channels_to_buffer(apu);
		apu->buffer_position = 0;
		apu->use_buffer1 = !apu->use_buffer1;
		apu->buffer_full = true;
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

void apu_step(Apu* apu, u8 cycles) {
	if (apu->nr52 & 0b10000000) { // audio enabled
		div_apu_step(apu, cycles);

		channel_1_step(apu, cycles);
		channel_2_step(apu, cycles);

		handle_buffers(apu);

		apu->clock += cycles;
	}
}

