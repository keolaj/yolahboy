#include <stdlib.h>
#include <assert.h>
#include "cpu2.h"

void init_cpu(Cpu* cpu) {
	cpu->registers.af = 0;
	cpu->registers.bc = 0;
	cpu->registers.de = 0;
	cpu->registers.hl = 0;
	cpu->registers.sp = 0;
	cpu->registers.pc = 0;
	cpu->IME = false;
}

Cpu* create_cpu() {
	Cpu* ret = (Cpu*)malloc(sizeof(Cpu));
	assert(ret != NULL && "Cannot allocate Cpu");
	init_cpu(ret);
	return ret;
}
void print_registers(Cpu* cpu) {
	printf("af: 0x%04X\n", cpu->registers.af);
	printf("bc: 0x%04X\n", cpu->registers.bc);
	printf("de: 0x%04X\n", cpu->registers.de);
	printf("hl: 0x%04X\n", cpu->registers.hl);
	printf("sp: 0x%04X\n", cpu->registers.sp);
	printf("pc: 0x%04X\n", cpu->registers.pc);
}

Operation get_operation(Cpu* cpu, Memory* mem) {
	u8 opcode = read8(mem, cpu->registers.pc);
	Operation ret = operations[opcode];
	ret.opcode = opcode;
	return ret;
}

Operation get_cb_operation(Cpu* cpu, Memory* mem) {
	u8 cb_opcode = read8(mem, cpu->registers.pc);
	Operation ret = cb_operations[cb_opcode];
	ret.opcode = cb_opcode;
	return ret;
}

u8* get_reg_from_type(Cpu* cpu, operand_type type) {

	switch (type) {
	case A:
		return &cpu->registers.a;
	case B:
		return &cpu->registers.b;
	case C:
		return &cpu->registers.c;
	case D:
		return &cpu->registers.d;
	case E:
		return &cpu->registers.e;
	case H:
		return &cpu->registers.h;
	case L:
		return &cpu->registers.l;
	}

}

u16* get_reg16_from_type(Cpu* cpu, operand_type type) {
	switch (type) {
	case AF:
		return &cpu->registers.af;
	case BC:
		return &cpu->registers.bc;
	case DE:
		return &cpu->registers.de;
	case HL:
		return &cpu->registers.hl;
	case SP:
		return &cpu->registers.sp;
	case PC:
		return &cpu->registers.pc;
	}
}

bool bit_mode_16(Operation op) {
	if (op.dest_addr_mode == REGISTER16 || op.dest_addr_mode == MEM_READ16) {
		return true;
	}
	else {
		return false;
	}
}

void run_secondary(Cpu* cpu, Operation op) {
	u16* reg;
	switch (op.secondary) {
	case INC_R_1:
		reg = get_reg16_from_type(cpu, op.dest);
		*reg += 1;
		break;
	case DEC_R_1:
		reg = get_reg16_from_type(cpu, op.dest);
		*reg -= 1;
		break;
	case INC_R_2:
		reg = get_reg16_from_type(cpu, op.source);
		*reg += 1;
		break;
	case DEC_R_2:
		reg = get_reg16_from_type(cpu, op.source);
		*reg -= 1;
		break;
	default:
		return;
	}
}

u16 get_source_16(Cpu* cpu, Memory* mem, Operation op) {
	u16 sourceVal;
	switch (op.source_addr_mode) {
	case REGISTER16:
		sourceVal = *get_reg16_from_type(cpu, op.source);
		break;
	case MEM_READ16:
		sourceVal = read16(mem, cpu->registers.pc);
		cpu->registers.pc += 2;
		break;
	default:
		sourceVal = 0;
		printf("unimplemented 16 bit source read\n");
		print_operation(op);
		assert(false);
	}
	return sourceVal;
}

u8 get_source(Cpu* cpu, Memory* mem, Operation op) {
	u8 sourceVal;
	switch (op.source_addr_mode) {
	case REGISTER:
		sourceVal = *get_reg_from_type(cpu, op.source);
		break;
	case MEM_READ:
		sourceVal = read8(mem, cpu->registers.pc);
		++cpu->registers.pc;
		break;
	default:
		sourceVal = 0;
		printf("unimplemented 8 bit source read\n");
		print_operation(op);
		assert(false);
	}
	return sourceVal;
}

void LD_impl(Cpu* cpu, Memory* mem, Operation op) {
	if (bit_mode_16(op)) {
		u16 source = get_source_16(cpu, mem, op);

		switch (op.dest_addr_mode) {
		case REGISTER16:
			*get_reg16_from_type(cpu, op.dest) = source;
			break;
		case MEM_READ_ADDR:
			write16(mem, read16(mem, cpu->registers.pc), source);
			cpu->registers.pc += 2;
			break;
		}

	} // 16 bit Load
	else {
		u8 source = get_source(cpu, mem, op);

		switch (op.dest_addr_mode) {
		case REGISTER:
			*get_reg_from_type(cpu, op.dest) = source;
			break;
		case ADDRESS_R16:
			write8(mem, *get_reg16_from_type(cpu, op.dest), source);
			break;
		case MEM_READ_ADDR:
			write8(mem, read16(mem, cpu->registers.pc), source);
			cpu->registers.pc += 2;
			break;
		}
	}
	if (op.secondary != SECONDARY_NONE) run_secondary(cpu, op);

}

void BIT_impl(Cpu* cpu, Memory* mem, Operation op) {
	
}

u8 generate_set_mask(instruction_flags flag_actions) {
	u8 ret = 0;
	if (flag_actions.zero == SET) {
		ret |= FLAG_ZERO;
	}
	if (flag_actions.sub == SET) {
		ret |= FLAG_SUB;
	}
	if (flag_actions.halfcarry == SET) {
		ret |= FLAG_HALFCARRY;
	}
	if (flag_actions.carry == SET) {
		ret |= FLAG_CARRY;
	}
	return ret;
}

u8 generate_reset_mask(instruction_flags flag_actions) {
	u8 ret = 0xFF;
	if (flag_actions.zero == RESET) {
		ret &= ~FLAG_ZERO;
	}
	if (flag_actions.sub == RESET) {
		ret &= ~FLAG_SUB;
	}
	if (flag_actions.halfcarry == RESET) {
		ret &= ~FLAG_HALFCARRY;
	}
	if (flag_actions.carry == RESET) {
		ret &= ~FLAG_CARRY;
	}
	return ret;
}

u8 generate_ignore_mask(instruction_flags flag_actions) {
	u8 ret = 0;
	if (flag_actions.zero == IGNORE) {
		ret |= FLAG_ZERO;
	}
	if (flag_actions.sub == IGNORE) {
		ret |= FLAG_SUB;
	}
	if (flag_actions.halfcarry == IGNORE) {
		ret |= FLAG_HALFCARRY;
	}
	if (flag_actions.carry == IGNORE) {
		ret |= FLAG_CARRY;
	}
	return ret;

}

alu_return run_alu(Cpu* cpu, u8 x, u8 y, instruction_type type, instruction_flags flag_actions) {
	u8 new_flags = 0;

	new_flags |= generate_set_mask(flag_actions);

	u8 result = 0;
	switch (type) {
	case XOR:
		result = x ^ y;
		break;
	}

	if (result == 0) {
		new_flags |= FLAG_ZERO;
	}

	new_flags |= (generate_ignore_mask(flag_actions) & cpu->registers.f);
	new_flags &= generate_reset_mask(flag_actions);


	return (alu_return) { result, new_flags };
}

void XOR_impl(Cpu* cpu, Memory* mem, Operation op) {
	u8 source = get_source(cpu, mem, op);

	switch (op.dest_addr_mode) {
	case REGISTER: {
		u8* destAddr = get_reg_from_type(cpu, op.dest);
		alu_return alu_ret = run_alu(cpu, *destAddr, source, op.type, op.flag_actions);
		*destAddr = alu_ret.result;
		cpu->registers.f = alu_ret.flags;
		break;
	}
	default:
		printf("unimplemented dest_addr_mode\t");
		print_operation(op);
		assert(false);
	}
}

Cycles step_cpu(Cpu* cpu, Memory* mem, Operation op) {

	++cpu->registers.pc;
	Cycles ret;
	switch (op.type) {
	case LD:
		LD_impl(cpu, mem, op);
		break;

	case XOR:
		XOR_impl(cpu, mem, op);
		break;

	case CB: {
		Cycles cb_ret = step_cpu(cpu, mem, get_cb_operation(cpu, mem));
		op.m_cycles += cb_ret.m_cycles;
		op.t_cycles += cb_ret.t_cycles;
		break;
	}
	default:
		printf("unimplemented operation type\t");
		print_operation(op);
		assert(false);

	}
	return (Cycles) { op.m_cycles, op.t_cycles };
}

