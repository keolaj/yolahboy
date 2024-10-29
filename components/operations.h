#pragma once
#include "global_definitions.h"
#include "operation_defitions.h"
#include <assert.h>


void push(Cpu* cpu, Memory* mem, u16 value);
void pop(Cpu* cpu, Memory* mem, u16* reg);
void jump(Cpu* cpu, u16 jump_to);

bool condition_passed(Cpu* cpu, Operation* op);
u8* get_reg_from_type(Cpu* cpu, operand_type type);
u16* get_reg16_from_type(Cpu* cpu, operand_type type);
u16* get_reg16_from_type(Cpu* cpu, operand_type type);
void write_dest16(Cpu* cpu, Memory* mem, address_mode mode, operand_type dest, u16 value);
bool bit_mode_16(Operation* op);
void run_secondary(Cpu* cpu, Operation* op);
u16 get_source_16(Cpu* cpu, Memory* mem, Operation* op);
u8 get_source(Cpu* cpu, Memory* mem, Operation* op);
u8 get_source16(Cpu* cpu, Memory* mem, Operation* op);
u8 get_dest(Cpu* cpu, Memory* mem, Operation* op);
u16 get_dest16(Cpu* cpu, Memory* mem, Operation* op);
void write_dest(Cpu* cpu, Memory* mem, Operation*, u8 value);
void run_interrupt(Cpu* cpu, Memory* mem);

alu_return run_alu(Cpu* cpu, u8 x, u8 y, instruction_type type, instruction_flags flag_actions);
alu16_return run_alu16(Cpu* cpu, u16 x, u16 y, instruction_type type, instruction_flags flag_actions);
void print_operation(Operation op);