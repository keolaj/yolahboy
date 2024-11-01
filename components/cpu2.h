#pragma once
#include <stdbool.h>
#include <stdio.h>
#include "cpu_definitions.h"
#include "memory2.h"
#include "operations.h"
#include "controller.h"


Cpu* create_cpu();
Cycles step_cpu(Cpu* cpu, Memory* mem, Operation op);
Operation get_operation(Cpu* cpu, Memory* mem);
void jump(Cpu* cpu, u16 address);
void print_registers(Cpu* cpu);
void update_IME(Cpu* cpu, bool value);