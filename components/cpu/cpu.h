#pragma once
#include <stdbool.h>
#include <stdio.h>
#include "cpu_definitions.h"
#include "../memory/memory.h"
#include "operations.h"


Cpu* create_cpu();
Cycles cpu_step(Cpu* cpu, Memory* mem, Operation op);
Operation get_operation(Cpu* cpu, Memory* mem);
void jump(Cpu* cpu, u16 address);
void print_registers(Cpu* cpu);
void update_IME(Cpu* cpu, bool value);
Cycles run_halted(Cpu* cpu, Memory* mem);
bool should_run_interrupt(Cpu* cpu, Memory* mem);

void destroy_cpu(Cpu* cpu);