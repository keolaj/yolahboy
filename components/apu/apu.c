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
	memset(apu, 0, sizeof(Apu));
	if (apu == NULL) {
		AddLog("Couldn't allocate Apu!\n");
		return NULL;
	}

	apu->buffer_size = buffer_size;
	apu->sample_rate = sample_rate;

	apu->lfsr = 0xF3C9;

	apu->buffer = (float*)malloc(sizeof(float) * buffer_size * 2); // Allocate main buffers

	if (apu->buffer == NULL) {
		AddLog("Couldn't allocate Apu buffer\n");
		destroy_apu(apu);
		return NULL;
	}

	return apu;
}

void init_apu(Apu* apu) {
	int buffer_size = apu->buffer_size;
	int sample_rate = apu->sample_rate;

	memset(apu, 0, 0x30); // This resets all Apu registers and controls
	
	memset(&apu->channel, 0, sizeof(Channel) * 4);
	memset(apu->buffer, 0, buffer_size * 2 * sizeof(float));
	
	apu->buffer_size = buffer_size;
	apu->sample_rate = sample_rate;
}

void destroy_apu(Apu* apu) {
	if (apu->buffer != NULL) free(apu->buffer);
}

void div_apu_step(Apu* apu, u8 cycles) {
	apu->div_apu_internal += cycles;
	if (apu->div_apu_internal >= 8192) {
		apu->div_apu_internal -= 8192;
		++apu->div_apu_counter;

		// Timer Controls
		if ((apu->div_apu_counter + 1) % 2 == 0) { // length timer
			for (int i = 0; i < 4; ++i) {
				if (apu->channel[i].length_enabled) {
					++apu->channel[i].length_timer;
				}
			}
		}
		if (apu->div_apu_counter == 7) { // envelope timer
			for (int i = 0; i < 4; ++i) {
				if (apu->channel[i].env_sweep_pace == 0) {
					continue;
				}
				--apu->channel[i].env_timer;
				if (apu->channel[i].env_timer == 0) {
					if (!apu->channel[i].env_dir) { // decreasing volume envelope
						if (apu->channel[i].volume == 0) {
							apu->channel[i].enabled = false;
						}
						--apu->channel[i].volume;
						apu->channel[i].env_timer = apu->channel[i].env_sweep_pace;
					}
					else {
						++apu->channel[i].volume;
						apu->channel[i].env_timer = apu->channel[i].env_sweep_pace;
						if (apu->channel[i].volume == 0xF) {
							apu->channel[i].enabled = false;
						}
					}
				}
			}
		}
		if (apu->div_apu_counter == 2 || apu->div_apu_counter == 6) {
			for (int i = 0; i < 4; ++i) {
				--apu->channel[i].freq_sweep_timer;
			}
		}

		if (apu->div_apu_counter == 8) apu->div_apu_counter = 0;
	}
}

// CHANNEL FUNCTIONS
void trigger_channel(Channel* channel) {
	channel->enabled = true;
	channel->frequency_timer = channel->frequency;
	channel->volume = channel->env_initial_volume;
	channel->length_timer = channel->length;
	channel->env_timer = channel->env_sweep_pace;
}

void channel_1_step(Apu* apu, u8 cycles) {
	if (apu->channel[0].enabled) {
		apu->channel[0].frequency_timer -= cycles;
		if (apu->channel[0].length_enabled) {
			if (apu->channel[0].length_timer == 64) {
				apu->channel[0].enabled = false;
			}
		}
		if (apu->channel[0].frequency_timer < 0) {
			apu->channel[0].frequency_timer += apu->channel[0].frequency;
			++apu->channel[0].wave_index;
			if (apu->channel[0].wave_index > 15) {
				apu->channel[0].wave_index = 0;
			}
		}
	}
}

float channel_1_sample(Apu* apu) {
	if (apu->channel[0].enabled) {
		return (duty_cycles[(apu->channel[0].wave_select & 0b11000000) >> 6][apu->channel[0].wave_index] ? 1.0 : 0.0) * (apu->channel[0].volume / (float)0xf);
	}
	else return 0.0f;
}

void channel_2_step(Apu* apu, u8 cycles) {
	if (apu->channel[1].enabled) {
		apu->channel[1].frequency_timer -= cycles;
		if (apu->channel[1].length_enabled) {
			if (apu->channel[1].length_timer == 64) {
				apu->channel[1].enabled = false;
			}
		}
		if (apu->channel[1].frequency_timer < 0) {
			apu->channel[1].frequency_timer += apu->channel[1].frequency;
			++apu->channel[1].wave_index;
			if (apu->channel[1].wave_index > 15) {
				apu->channel[1].wave_index = 0;
			}
		}
	}

}
float channel_2_sample(Apu* apu) {
	if (apu->channel[1].enabled) {
		return (duty_cycles[(apu->channel[1].wave_select & 0b11000000) >> 6][apu->channel[1].wave_index] ? 1.0 : 0.0) * (apu->channel[1].volume / (float)0xf);
	}
	else return 0.0f;

}

void step_lfsr(Apu* apu) {
	bool result = ((apu->lfsr & 2) >> 1) != (apu->lfsr & 1);
	apu->lfsr >>= 1;
	apu->lfsr |= result << 15;
	
	if (apu->lfsr_width) {
		apu->lfsr |= result << 7;
	}
}

void channel_4_step(Apu* apu, u8 cycles) {
	if (apu->channel[3].enabled) {
		apu->channel[3].frequency_timer -= cycles;
		if (apu->channel[3].length_enabled) {
			if (apu->channel[3].length_timer == 64) {
				apu->channel[3].enabled = false;
			}
		}
		if (apu->channel[3].frequency_timer < 0) {
			apu->channel[3].frequency_timer = apu->channel[3].frequency;
			step_lfsr(apu);
		}
	}
}

float channel_4_sample(Apu* apu) {
	return (((bool)(apu->lfsr & 0x8000)) ? (1.0) : (0.0)) * (float)apu->channel[3].volume / 0xF;
}

void handle_sample(Apu* apu, u8 cycles) {

}

// BUFFER FUNCTIONS
void write_channels_to_buffer(Apu* apu) {
	// TODO 
	float ch1_sample = channel_1_sample(apu) / 3;
	float ch2_sample = channel_2_sample(apu) / 3;
	float ch4_sample = channel_4_sample(apu) / 5;

	apu->buffer[apu->buffer_position * 2] = ch1_sample + ch2_sample + ch4_sample;
	apu->buffer[(apu->buffer_position * 2) + 1] = ch1_sample + ch2_sample + ch4_sample;
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
		channel_4_step(apu, cycles);

		handle_buffers(apu, cycles);
		apu->clock += cycles;

	}
}

