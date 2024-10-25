#pragma once
#include <stdbool.h>
#include <stdio.h>
#include "cpu_definitions.h"
#include "memory2.h"
#include "operations.h"

Cpu* create_cpu();
Cycles step_cpu(Cpu* cpu, Memory* mem, Operation op);
Operation get_operation(Cpu* cpu, Memory* mem);

typedef struct {
	u8 result;
	u8 flags;
} alu_return;

void print_registers(Cpu* cpu);