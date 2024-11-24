#pragma once
#include "../global_definitions.h"

void step_apu(Apu* apu, u8 cycles);
Apu* create_apu(int sample_rate, int buffer_size);
void destroy_apu(Apu* apu);
float* get_buffer(Apu* apu);
