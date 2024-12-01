#pragma once
#include "../mmu/mmu.h"
#include "operations.h"

Cycles cpu_step(Emulator* emu, Operation op);
Operation get_operation(Cpu* cpu, Mmu* mem);
void jump(Cpu* cpu, u16 address);
void print_registers(Cpu* cpu);
void update_IME(Cpu* cpu, bool value);
Cycles run_halted(Cpu* cpu, Mmu* mem);
bool should_run_interrupt(Cpu* cpu, Mmu* mem);
