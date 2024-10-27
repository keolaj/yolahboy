#pragma once
#include <stdbool.h>
#include <stdio.h>
#include "cpu_definitions.h"
#include "memory2.h"
#include "operations.h"

typedef struct {
	u8 result;
	u8 flags;
} alu_return;

typedef struct {
	u16 result;
	u8 flags;
} alu16_return;

Cpu* create_cpu();
Cycles step_cpu(Cpu* cpu, Memory* mem, Operation op);
Operation get_operation(Cpu* cpu, Memory* mem);
alu_return run_alu(Cpu* cpu, u8 x, u8 y, instruction_type type, instruction_flags flag_actions);
void jump(Cpu* cpu, u16 address);
void print_registers(Cpu* cpu);