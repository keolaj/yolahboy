#pragma once
#include "../global_definitions.h"

void apu_step(Apu* apu, u8 cycles);
Apu* create_apu(int sample_rate, int buffer_size);
void init_apu(Apu* apu);
void destroy_apu(Apu* apu);
float* get_buffer(Apu* apu);
void trigger_channel(Channel* channel);
