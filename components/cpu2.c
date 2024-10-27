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

bool condition_passed(Cpu* cpu, Operation* op) {
	switch (op->condition) {
	case CONDITION_Z:
		if ((cpu->registers.f & FLAG_ZERO) == FLAG_ZERO) {
			return true;
		}
		break;
	case CONDITION_NZ:
		if ((cpu->registers.f & FLAG_ZERO) != FLAG_ZERO) {
			return true;
		}
		break;
	case CONDITION_C:
		if ((cpu->registers.f & FLAG_CARRY) == FLAG_CARRY) {
			return true;
		}
		break;
	case CONDITION_NC:
		if ((cpu->registers.f & FLAG_CARRY) != FLAG_CARRY) {
			return true;
		}
		break;
	default:
		// why get here
		return true;
	}
	return false;
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
	default:
		assert(false && "Not a register");
		return NULL;
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
	default:
		assert(false && "Not a register");
		return NULL;
	}
}

void write_dest(Cpu* cpu, Memory* mem, address_mode mode, operand_type dest, u8 value) {
	switch (mode) {
	case REGISTER:
		*get_reg_from_type(cpu, dest) = value;
		break;
	case ADDRESS_R16:
		write8(mem, *get_reg16_from_type(cpu, dest), value);
		break;
	case MEM_READ16:
		write8(mem, read8(mem, cpu->registers.pc), value);
		cpu->registers.pc += 2;
		break;
	}
}

bool bit_mode_16(Operation* op) {
	if (op->dest_addr_mode == REGISTER16 || op->dest_addr_mode == MEM_READ16) {
		return true;
	}
	else {
		return false;
	}
}

void run_secondary(Cpu* cpu, Operation* op) {
	u16* reg;
	switch (op->secondary) {
	case INC_R_1:
		reg = get_reg16_from_type(cpu, op->dest);
		*reg = *reg + 1;
		break;
	case DEC_R_1:
		reg = get_reg16_from_type(cpu, op->dest);
		*reg -= 1;
		break;
	case INC_R_2:
		reg = get_reg16_from_type(cpu, op->source);
		*reg += 1;
		break;
	case DEC_R_2:
		reg = get_reg16_from_type(cpu, op->source);
		*reg -= 1;
		break;
	case ADD_T_4:
		op->t_cycles += 4;
		break;
	case ADD_T_12:
		op->t_cycles += 12;
	default:
		return;
	}
}

u16 get_source_16(Cpu* cpu, Memory* mem, Operation* op) {
	u16 sourceVal;
	switch (op->source_addr_mode) {
	case REGISTER16:
		sourceVal = *get_reg16_from_type(cpu, op->source);
		break;
	case MEM_READ16:
		sourceVal = read16(mem, cpu->registers.pc);
		cpu->registers.pc += 2;
		break;
	default:
		sourceVal = 0;
		printf("unimplemented 16 bit source read\n");
		print_operation(*op);
		assert(false);
	}
	return sourceVal;
}

u8 get_source(Cpu* cpu, Memory* mem, Operation* op) { // maybe I'll change this to be able to read dest too at some point. Might make life easier
	u8 sourceVal;
	switch (op->source_addr_mode) {
	case REGISTER:
		sourceVal = *get_reg_from_type(cpu, op->source);
		break;
	case MEM_READ:
		sourceVal = read8(mem, cpu->registers.pc);
		++cpu->registers.pc;
		break;
	case ADDRESS_R16:
		sourceVal = read8(mem, *get_reg16_from_type(cpu, op->source));
		break;
	case ADDRESS_R8_OFFSET:
		sourceVal = read8(mem, 0xff00 + read8(mem, cpu->registers.pc));
		++cpu->registers.pc;
		break;
	default:
		sourceVal = 0;
		printf("unimplemented 8 bit source read\n");
		print_operation(*op);
		assert(false);
	}
	return sourceVal;
}

u8 get_source16(Cpu* cpu, Memory* mem, Operation* op) {
	// TODO
	return 0;
}

u8 get_dest(Cpu* cpu, Memory* mem, Operation* op) { // nvm I made it it's own function. DONT CALL THIS WITHOUT TESTING
	u8 destVal;
	switch (op->dest_addr_mode) {
	case REGISTER:
		destVal = *get_reg_from_type(cpu, op->dest);
		break;
	case MEM_READ_ADDR_OFFSET:
	case MEM_READ:
		destVal = read8(mem, cpu->registers.pc);
		++cpu->registers.pc;
		break;
	default:
		destVal = 0;
		printf("unimplemented 8 bit dest read\n");
		print_operation(*op);
		assert(false);
	}
	return destVal;
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
		case MEM_READ_ADDR:
			write8(mem, read16(mem, cpu->registers.pc), source);
			cpu->registers.pc += 2;
			break;
		case MEM_READ_ADDR_OFFSET: {
			u8 reg_value = get_dest(cpu, mem, op);
			u16 addr = 0xFF00 + reg_value;
			write8(mem, addr, source);
			break;
		}
		}
	}
	if (op->secondary != SECONDARY_NONE) run_secondary(cpu, op);

}

void push(Cpu* cpu, Memory* mem, u16 value) {
	write16(mem, cpu->registers.sp - 2, value);
	cpu->registers.sp -= 2;
}
void pop(Cpu* cpu, Memory* mem, u16* reg) {
	*reg = read16(mem, cpu->registers.sp);
	cpu->registers.sp += 2;
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

void jump(Cpu* cpu, u16 jump_to) {
	cpu->registers.pc = jump_to;
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
			i8 relative = (i8)get_source(cpu, mem, op);
			if (condition_passed(cpu, op)) {
				jump(cpu, (u8)(cpu->registers.pc + relative));
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

alu_return run_alu(Cpu* cpu, u8 x, u8 y, instruction_type type, instruction_flags flag_actions) {
	u8 new_flags = 0;

	new_flags |= generate_set_mask(flag_actions);

	u8 result = 0;
	switch (type) {
	case OR:
		result = x | y;
		break;
	case XOR:
		result = x ^ y;
		break;
	case BIT:
		result = x & y;
		break;
	case INC:
	case ADD:
		result = x + y;
		if (((x & 0x0f) + (y & 0x0f)) > 0x0f) { //  half carry			
			new_flags |= FLAG_HALFCARRY;
		}
		if ((int)x + (int)y > 255) {
			new_flags |= FLAG_CARRY;
		}

		break;
	case SUB:
	case CP:
	case DEC:
		result = x - y;
		// new_flags |= FLAG_HALFCARRY | FLAG_CARRY;
		if ((y & 0x0f) > (x & 0x0f)) { //  half carry (there is a lot of different documentation on this so idk, this matches bgb) 
			new_flags |= FLAG_HALFCARRY;
		}
		if ((int)x - (int)y < 0) { //  carry
			new_flags |= FLAG_CARRY;
		}
		break;

	case SWAP:
		result = (x << 4) | (x >> 4);
		break;

	case RL:
	{
		u8 new_carry = x & 0b10000000;
		result = x << 1;
		result |= ((cpu->registers.f & FLAG_CARRY) >> 4);
		new_flags |= (new_carry >> 3);
		break;
	}
	}



	if (result == 0) {
		new_flags |= FLAG_ZERO;
	}

	new_flags |= (generate_ignore_mask(flag_actions) & cpu->registers.f);
	new_flags &= generate_reset_mask(flag_actions);


	return (alu_return) { result, new_flags };
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
	alu_return alu_ret = run_alu(cpu, get_dest(cpu, mem, op), get_source(cpu, mem, op), op->type, op->flag_actions);
	write_dest(cpu, mem, op->dest_addr_mode, op->dest, alu_ret.result);
	cpu->registers.f = alu_ret.flags;

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
	pop(cpu, mem, get_reg16_from_type(cpu, PC));
}

Cycles step_cpu(Cpu* cpu, Memory* mem, Operation op) {
	++cpu->registers.pc;

	if (cpu->registers.pc - 1 == 0x5f && false) {
		printf("BREAKPOINT!!! REGISTERS: \n");
		--cpu->registers.pc;
		print_registers(cpu);
		++cpu->registers.pc;
	}


	switch (op.type) {
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


	case SUB:
	case OR:
		ALU_impl(cpu, mem, &op);

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
	default:
		--cpu->registers.pc;
		print_registers(cpu);
		printf("unimplemented operation type\t");
		print_operation(op);
		assert(false);

	}

	return (Cycles) { op.m_cycles, op.t_cycles };
}

