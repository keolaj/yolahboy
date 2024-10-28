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
	cpu->should_update_IME = false;
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

void update_IME(Cpu* cpu, bool value) {
	cpu->should_update_IME = true;
	cpu->update_IME_value = value;
	cpu->update_IME_counter = 1;
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

void LD_impl(Cpu* cpu, Memory* mem, Operation* op) {
	if (bit_mode_16(op)) {
		u16 source = get_source_16(cpu, mem, op);

		switch (op->dest_addr_mode) {
		case REGISTER16:
			*get_reg16_from_type(cpu, op->dest) = source;
			break;
		case MEM_READ_ADDR:
			write16(mem, read16(mem, cpu->registers.pc), source);
			cpu->registers.pc += 2;
			break;
		}

	}
	else {
		u8 source = get_source(cpu, mem, op);

		switch (op->dest_addr_mode) {
		case REGISTER:
			*get_reg_from_type(cpu, op->dest) = source;
			break;
		case ADDRESS_R16:
			write8(mem, *get_reg16_from_type(cpu, op->dest), source);
			break;
		case ADDRESS_R8_OFFSET:
			write8(mem, (0xFF00 + get_dest(cpu, mem, op)), source);
			break;
		case MEM_READ_ADDR:
			write8(mem, read16(mem, cpu->registers.pc), source);
			cpu->registers.pc += 2;
			break;
		case MEM_READ_ADDR_OFFSET: {
			u8 reg_value = get_dest(cpu, mem, op);
			u16 addr = 0xFF00 + reg_value;
			write8(mem, addr, source);
			break;
		default:
			printf("unimplemented dest write LD");
			assert(false);
		}
		}
	}
	if (op->secondary != SECONDARY_NONE) run_secondary(cpu, op);

}


void CALL_impl(Cpu* cpu, Memory* mem, Operation* op) {
	// Calls should only be u16 address mode
	if (condition_passed(cpu, op)) {
		u16 addr = get_source_16(cpu, mem, op);
		push(cpu, mem, cpu->registers.pc);
		jump(cpu, addr);
	}
	run_secondary(cpu, op);
}


void BIT_impl(Cpu* cpu, Memory* mem, Operation* op) {

	u8 test = 1 << op->dest; // for some reason it made sense for me to put the bit here instead of dest. hopefully just this once i do something like this

	alu_return alu_ret = run_alu(cpu, get_source(cpu, mem, op), test, op->type, op->flag_actions);
	cpu->registers.f = alu_ret.flags;

}


void JP_impl(Cpu* cpu, Memory* mem, Operation* op) {
	if (bit_mode_16(op)) {
		u16 addr = get_source_16(cpu, mem, op);
		if (condition_passed(cpu, op)) {
			jump(cpu, addr);
			run_secondary(cpu, op);
		}
	}
	else {
		switch (op->source_addr_mode) {
		case MEM_READ: {
			u8 relative = get_source(cpu, mem, op);
			u16 jump_to = cpu->registers.pc;
			if (relative > 127) {
				relative = ~relative + 1;
				jump_to -= relative;
			}
			else {
				jump_to += relative;
			}

			if (condition_passed(cpu, op)) {
				jump(cpu, jump_to);
				run_secondary(cpu, op);
			}
			break;
		}
		}
	}
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


void INC_impl(Cpu* cpu, Memory* mem, Operation* op) {
	switch (op->dest_addr_mode) {
	case REGISTER: {
		u8* dest_addr = get_reg_from_type(cpu, op->dest);
		alu_return alu_ret = run_alu(cpu, *dest_addr, 1, op->type, op->flag_actions);
		*dest_addr = alu_ret.result;
		cpu->registers.f = alu_ret.flags;
		break;
	}
	case REGISTER16: {
		u16* dest_addr = get_reg16_from_type(cpu, op->dest);
		*dest_addr += 1;
		break;
	}
	case ADDRESS_R16: {
		u8 prev = get_dest(cpu, mem, op);
		write_dest(cpu, mem, op->dest_addr_mode, op->dest, prev + 1);
		break;
	}
	default:
		printf("unimplemented increment");
		assert(false);
	}
}

void DEC_impl(Cpu* cpu, Memory* mem, Operation* op) {
	switch (op->dest_addr_mode) {
	case REGISTER: {
		u8* dest_addr = get_reg_from_type(cpu, op->dest);
		alu_return alu_ret = run_alu(cpu, *dest_addr, 1, op->type, op->flag_actions);
		*dest_addr = alu_ret.result;
		cpu->registers.f = alu_ret.flags;
		break;
	}
	case REGISTER16: {
		u16* dest_addr = get_reg16_from_type(cpu, op->dest);
		*dest_addr = *dest_addr - 1;
		break;
	}
	default:
		printf("unimplemented dec");
	}
}

void XOR_impl(Cpu* cpu, Memory* mem, Operation* op) {
	u8 source = get_source(cpu, mem, op);

	//switch (op->dest_addr_mode) {
	//case REGISTER: {
	//	u8* dest_addr = get_reg_from_type(cpu, op->dest);
	//	alu_return alu_ret = run_alu(cpu, *dest_addr, source, op->type, op->flag_actions);
	//	*dest_addr = alu_ret.result;
	//	cpu->registers.f = alu_ret.flags;


	//	break;
	//}
	//default:
	//	printf("unimplemented dest_addr_mode\t");
	//	print_operation(*op);
	//	assert(false);
	//}

	alu_return alu_ret = run_alu(cpu, get_dest(cpu, mem, op), source, op->type, op->flag_actions);
	write_dest(cpu, mem, op->dest_addr_mode, op->dest, alu_ret.result);
	cpu->registers.f = alu_ret.flags;

}

void SWAP_impl(Cpu* cpu, Memory* mem, Operation* op) {
	alu_return alu_ret = run_alu(cpu, get_dest(cpu, mem, op), 0, op->type, op->flag_actions);
	write_dest(cpu, mem, op->dest_addr_mode, op->dest, alu_ret.result);
	cpu->registers.f = alu_ret.flags;

}

void CP_impl(Cpu* cpu, Memory* mem, Operation* op) {
	alu_return alu_ret = run_alu(cpu, get_dest(cpu, mem, op), get_source(cpu, mem, op), op->type, op->flag_actions);
	cpu->registers.f = alu_ret.flags;
}

void SUB_impl(Cpu* cpu, Memory* mem, Operation* op) {
	alu_return alu_ret = run_alu(cpu, get_dest(cpu, mem, op), get_source(cpu, mem, op), op->type, op->flag_actions);
	write_dest(cpu, mem, op->dest_addr_mode, op->dest, alu_ret.result);
	cpu->registers.f = alu_ret.flags;
}

void ALU_impl(Cpu* cpu, Memory* mem, Operation* op) {
	if (bit_mode_16(op)) {
		alu16_return alu_ret = run_alu16(cpu, get_dest16(cpu, mem, op), get_source_16(cpu, mem, op), op->type, op->flag_actions);
		write_dest16(cpu, mem, op->dest_addr_mode, op->dest, alu_ret.result);
		cpu->registers.f = alu_ret.flags;
	}
	else {
		alu_return alu_ret = run_alu(cpu, get_dest(cpu, mem, op), get_source(cpu, mem, op), op->type, op->flag_actions);
		write_dest(cpu, mem, op->dest_addr_mode, op->dest, alu_ret.result);
		cpu->registers.f = alu_ret.flags;

	}
}

void PUSH_impl(Cpu* cpu, Memory* mem, Operation* op) {
	u16 to_push = get_source_16(cpu, mem, op);
	push(cpu, mem, to_push);
}

void POP_impl(Cpu* cpu, Memory* mem, Operation* op) {
	u16* reg = get_reg16_from_type(cpu, op->dest);
	pop(cpu, mem, reg);
}

void RL_impl(Cpu* cpu, Memory* mem, Operation* op) {
	alu_return alu_ret = run_alu(cpu, get_dest(cpu, mem, op), 0, op->type, op->flag_actions);
	write_dest(cpu, mem, op->dest_addr_mode, op->dest, alu_ret.result);
	cpu->registers.f = alu_ret.flags;
}

void RET_impl(Cpu* cpu, Memory* mem, Operation* op) {
	if (condition_passed(cpu, op)) {
		pop(cpu, mem, get_reg16_from_type(cpu, PC));
	}
}

bool should_run_interrupt(Cpu* cpu, Memory* mem) {
	if (cpu->IME && read8(mem, IF)) {
		return true;
	}
	else {
		return false;
	}
}



Cycles step_cpu(Cpu* cpu, Memory* mem, Operation op) {
	++cpu->registers.pc;

	if (cpu->registers.pc - 1 == 0x1FE) {
		printf("BREAKPOINT!!! REGISTERS: \n");
		--cpu->registers.pc;
		print_registers(cpu);
		++cpu->registers.pc;
	}

	if (cpu->registers.pc - 1 == 0x100) {
		mem->in_bios = false;
	}

	switch (op.type) {
	case NOP:
		break;
	case LD:
		LD_impl(cpu, mem, &op);
		break;

	case XOR:
		XOR_impl(cpu, mem, &op);
		break;
	case INC:
		INC_impl(cpu, mem, &op);
		break;
	case DEC:
		DEC_impl(cpu, mem, &op);
		break;
	case BIT:
		BIT_impl(cpu, mem, &op);
		break;
	case CP:
		CP_impl(cpu, mem, &op);
		break;

	case ADD:
	case SUB:
	case OR:
	case AND:
	case CPL:
		ALU_impl(cpu, mem, &op);
		break;
	case JP:
		JP_impl(cpu, mem, &op);
		break;

	case CALL:
		CALL_impl(cpu, mem, &op);
		break;
	case RET:
		RET_impl(cpu, mem, &op);
		break;
	case PUSH:
		PUSH_impl(cpu, mem, &op);
		break;
	case POP:
		POP_impl(cpu, mem, &op);
		break;

	case RLA:
	case RL:
		RL_impl(cpu, mem, &op);
		break;

	case SWAP:
		SWAP_impl(cpu, mem, &op);
		break;

	case CB: {
		Cycles cb_ret = step_cpu(cpu, mem, get_cb_operation(cpu, mem));
		op.m_cycles += cb_ret.m_cycles;
		op.t_cycles += cb_ret.t_cycles;
		break;
	}
	case EI:
		update_IME(cpu, true);
		break;
	case DI:
		update_IME(cpu, false);
		break;
	case UNIMPLEMENTED:
	default:
		--cpu->registers.pc;
		print_registers(cpu);
		printf("unimplemented operation type\t");
		print_operation(op);
		assert(false);

	}

	if (cpu->should_update_IME) {
		if (cpu->update_IME_counter == 0) {
			cpu->IME = cpu->update_IME_value;
			cpu->should_update_IME = false;
		}
		else {
			--cpu->update_IME_counter;
		}
	}

	if (should_run_interrupt(cpu, mem)) {
		run_interrupt(cpu, mem);
	}

	return (Cycles) { op.m_cycles, op.t_cycles };
}

