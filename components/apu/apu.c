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

	apu->buffer = (float*)malloc(sizeof(float) * buffer_size * 2); // Allocate main buffers

	if (apu->buffer == NULL) {
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

	memset(apu, 0, 0x40); // This resets all Apu registers 
	memset(&apu->channel, 0, sizeof(Channel) * 4);
	memset(apu->buffer, 0, buffer_size * 2 * sizeof(float));
	
	apu->buffer_size = buffer_size;
	apu->sample_rate = sample_rate;
}

void destroy_apu(Apu* apu) {
	if (apu->buffer != NULL) free(apu->buffer);
}

void div_apu_step(Apu* apu, u8 cycles) {

}

// CHANNEL FUNCTIONS
void channel_1_step(Apu* apu, u8 cycles) {
	if (apu->channel[0].enabled) {
		apu->channel[0].frequency_timer -= cycles;
		if (apu->channel[0].frequency_timer < 0) {
			apu->channel[0].frequency_timer += ((2048 - apu->channel[0].frequency) * 4);
			++apu->channel[0].wave_index;
			if (apu->channel[0].wave_index > 15) {
				apu->channel[0].wave_index = 0;
			}
		}
	}
}
float channel_1_sample(Apu* apu) {
	if (apu->channel[0].enabled) {
		return (duty_cycles[(apu->nr21 & 0b11000000) >> 6][apu->channel[0].wave_index] ? 1.0 : 0.0);
	}
	else return 0.0f;
}

void channel_2_step(Apu* apu, u8 cycles) {
	
}
void channel_2_sample(Apu* apu) {

}

void handle_sample(Apu* apu, u8 cycles) {

}

// BUFFER FUNCTIONS
void write_channels_to_buffer(Apu* apu) {
	// TODO 
	float sample = channel_1_sample(apu);
	apu->buffer[apu->buffer_position * 2] = sample;
	apu->buffer[(apu->buffer_position * 2) + 1] = sample;
	++apu->buffer_position;
}

void handle_buffers(Apu* apu, u8 cycles) {
	apu->sample_counter += (apu->sample_rate * (cycles));
	if (apu->sample_counter > 1048576 * 4) {
		apu->sample_counter -= 1048576 * 4;
		write_channels_to_buffer(apu);
	}

	if (apu->buffer_position >= apu->buffer_size) {
		apu->buffer_position = 0;
		apu->buffer_full = true;
	}
}

float* get_buffer(Apu* apu) {
	return apu->buffer;
}

void apu_step(Apu* apu, u8 cycles) {
	if (apu->nr52 & 0b10000000) { // audio enabled
		div_apu_step(apu, cycles);

		channel_1_step(apu, cycles);
		channel_2_step(apu, cycles);

		handle_buffers(apu, cycles);

		apu->clock += cycles;
	}
}

