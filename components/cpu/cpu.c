#include <stdlib.h>
#include <assert.h>
#include "cpu.h"
#include "../debugger/imgui_custom_widget_wrapper.h"


void init_cpu(Cpu* cpu) {
	cpu->registers.af = 0;
	cpu->registers.bc = 0;
	cpu->registers.de = 0;
	cpu->registers.hl = 0;
	cpu->registers.sp = 0;
	cpu->registers.pc = 0;
	cpu->IME = false;
	cpu->should_update_IME = false;
	cpu->halted = false;
}

Cpu* create_cpu() {
	Cpu* ret = (Cpu*)malloc(sizeof(Cpu));
	if (ret == NULL) {
		AddLog("could not allocate cpu");
		return NULL;
	}
	init_cpu(ret);
	return ret;
}
void print_registers(Cpu* cpu) {
	AddLog("af: 0x%04hX\n", cpu->registers.af);
	AddLog("bc: 0x%04hX\n", cpu->registers.bc);
	AddLog("de: 0x%04hX\n", cpu->registers.de);
	AddLog("hl: 0x%04hX\n", cpu->registers.hl);
	AddLog("sp: 0x%04hX\n", cpu->registers.sp);
	AddLog("pc: 0x%04hX\n", cpu->registers.pc);
}

void update_IME(Cpu* cpu, bool value) {
	cpu->should_update_IME = true;
	cpu->update_IME_value = value;
	cpu->update_IME_counter = 1;
}

void push(Cpu* cpu, Memory* mem, u16 value) {
	write16(mem, cpu->registers.sp - 2, value);
	cpu->registers.sp -= 2;
}
void pop(Cpu* cpu, Memory* mem, u16* reg, operand_type operand) {
	*reg = read16(mem, cpu->registers.sp);
	if (operand == AF) {
		cpu->registers.f &= 0xF0;
	}
	cpu->registers.sp += 2;
}
void jump(Cpu* cpu, u16 jump_to) {
	cpu->registers.pc = jump_to;
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
	if (flag_actions.zero == _IGNORE) {
		ret |= FLAG_ZERO;
	}
	if (flag_actions.sub == _IGNORE) {
		ret |= FLAG_SUB;
	}
	if (flag_actions.halfcarry == _IGNORE) {
		ret |= FLAG_HALFCARRY;
	}
	if (flag_actions.carry == _IGNORE) {
		ret |= FLAG_CARRY;
	}
	return ret;
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

alu_return run_alu(Cpu* cpu, u8 x, u8 y, instruction_type type, instruction_flags flag_actions) {
	u8 new_flags = 0;

	new_flags |= generate_set_mask(flag_actions);

	u8 result = 0;
	switch (type) {
	case AND:
		result = x & y;
		break;
	case OR:
		result = x | y;
		break;
	case XOR:
		result = x ^ y;
		break;
	case BIT:
		result = x & y;
		break;
	case CPL:
		result = ~x;
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
	case ADC: {
		u8 carry = ((cpu->registers.f & FLAG_CARRY) ? 1 : 0);
		if ((x & 0x0f) + (y & 0x0f) + carry > 0x0f) { //  half carry			
			new_flags |= FLAG_HALFCARRY;
		}
		if ((int)x + (int)y + carry > 255) {

			new_flags |= FLAG_CARRY;

		}

		result = x + y + carry;
		break;
	}

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

	case SBC: {
		u8 carry = ((cpu->registers.f & FLAG_CARRY) ? 1 : 0);
		result = x - y - carry;
		if (((y & 0x0f)) > ((x & 0x0f) - carry)) { //  half carry (there is a lot of different documentation on this so idk, this matches bgb) 
			new_flags |= FLAG_HALFCARRY;
		}
		else {
			new_flags &= ~FLAG_HALFCARRY;	
		}
		if ((int)x - (int)y - carry < 0) { //  carry
			new_flags |= FLAG_CARRY;
		}
		else {
			new_flags &= ~FLAG_CARRY;
		}
		break;
	}

	case SWAP:
		result = (x << 4) | (x >> 4);
		break;

	case SET_OP:
		result = x | (1 << y);
		break;
	case RL:
	{
		u8 new_carry = x & 0b10000000;
		result = x << 1;
		result |= ((cpu->registers.f & FLAG_CARRY) >> 4);
		new_flags |= (new_carry >> 3);
		break;
	}
	case RR:
	{
		u8 new_carry = x & 0b00000001;
		result = x >> 1;
		result |= ((cpu->registers.f & FLAG_CARRY) << 3);
		new_flags |= (new_carry << 4);
		break;
	}

	case RES:
		result = x & ~(1 << y);
		break;

	case SLA: {
		if (x & 0b10000000) new_flags |= FLAG_CARRY;
		result = x << 1;
		break;
	}
	case SRA: {
		result = (x >> 1);
		if (x & 0b10000000) result |= 0b10000000;
		if (x & 0b00000001) new_flags |= FLAG_CARRY;

		break;
	}
	case SRL: {
		if (x & 0b00000001) new_flags |= FLAG_CARRY;
		result = x >> 1;
		break;
	}
	case RLC: {
		result = x << 1;
		if (x & 0b10000000) {
			result |= 0b00000001;
			new_flags |= FLAG_CARRY;
		}
		break;
	}
	case RRC: {
		result = x >> 1;
		if (x & 0b00000001) {
			result |= 0b10000000;
			new_flags |= FLAG_CARRY;
		}
		break;
	}
	case SCF: 
		break;
	case CCF:
		if (cpu->registers.f & FLAG_CARRY) {

		}
		else {
			new_flags |= FLAG_CARRY;
		}
		break;


	}



	if (result == 0) {
		new_flags |= FLAG_ZERO;
	}

	u8 ignore_mask = generate_ignore_mask(flag_actions);
	new_flags &= ~ignore_mask;
	new_flags |= ignore_mask & cpu->registers.f;

	new_flags &= generate_reset_mask(flag_actions);


	return (alu_return) { result, new_flags };
}

alu16_return run_alu16(Cpu* cpu, u16 x, u16 y, instruction_type type, address_mode source_addr_mode, instruction_flags flag_actions) {
	u8 new_flags = 0;
	new_flags |= generate_set_mask(flag_actions);
	u16 result;
	switch (type) {
	case ADD:
		result = x + y;
		if (source_addr_mode == REGISTER16) {
			if (((x & 0x0FFF) + (y & 0x0FFF)) > 0x0FFF) {
				new_flags |= FLAG_HALFCARRY;
			}
			if ((int)x + (int)y > 0xFFFF) {
				new_flags |= FLAG_CARRY;
			}
		}
		else {
			if (((x & 0x0F) + (y & 0x0F)) > 0x0F) {
				new_flags |= FLAG_HALFCARRY;
			}
			if (((x & 0xFF) + (y & 0xFF)) > 0xFF) {
				new_flags |= FLAG_CARRY;
			}
		}
		break;

	default:
		AddLog("TODO: Unimplemented run_alu16 type");
		result = 0;
		assert(false);
		break;
	}
	if (result == 0) {
		new_flags |= FLAG_ZERO;
	}
	u8 ignore_mask = generate_ignore_mask(flag_actions);
	new_flags &= ~ignore_mask;
	new_flags |= ignore_mask & cpu->registers.f;
	new_flags &= generate_reset_mask(flag_actions);

	return (alu16_return) { result, new_flags };
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
	case SP_ADD_I8:
		return &cpu->registers.sp;
	case PC:
		return &cpu->registers.pc;
	default:
		assert(false && "Not a register");
		return NULL;
	}
}

void write_dest(Cpu* cpu, Memory* mem, Operation* op, u8 value) {
	switch (op->dest_addr_mode) {
	case REGISTER:
		*get_reg_from_type(cpu, op->dest) = value;
		break;
	case ADDRESS_R16:
		write8(mem, *get_reg16_from_type(cpu, op->dest), value);
		break;
		//case MEM_READ16:
		//	write8(mem, read8(mem, cpu->registers.pc), value);
		//	cpu->registers.pc += 2;
		//	break;
	case ADDRESS_R8_OFFSET:
		write8(mem, (0xFF00 + get_dest(cpu, mem, op)), value);
		break;
	case MEM_READ_ADDR:
		write8(mem, read16(mem, cpu->registers.pc), value);
		cpu->registers.pc += 2;
		break;
	case MEM_READ_ADDR_OFFSET: {
		u8 offset = read8(mem, cpu->registers.pc);
		++cpu->registers.pc;
		write8(mem, (0xFF00 + offset), value);
		break;
	}
	case OPERAND_NONE:
		break;
	default:
		AddLog("unimplemented write dest addr mode");
		assert(false);
	}
}

void write_dest16(Cpu* cpu, Memory* mem, address_mode mode, operand_type dest, u16 value) {
	switch (mode) {
	case REGISTER16:
		*get_reg16_from_type(cpu, dest) = value;
		break;
	default:
		AddLog("unimplemented write dest16 type");
		assert(false);
		break;
	}
}

bool bit_mode_16(Operation* op) {
	if (op->dest_addr_mode == REGISTER16 || op->dest_addr_mode == MEM_READ16 || op->source_addr_mode == MEM_READ16 || op->source_addr_mode == REGISTER16) {
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
		break;
	default:
		return;
	}
}

u16 get_source_16(Cpu* cpu, Memory* mem, Operation* op) {
	u16 sourceVal;
	switch (op->source_addr_mode) {
	case REGISTER16:
		if (op->source == SP_ADD_I8) {
			u8 val = read8(mem, cpu->registers.pc++);
			i8 relative;

			if (val > 127) {
				val = ~val + 1;
				relative = -*(i8*)&val;
			}
			else {
				relative = *(i8*)&val;
			}


			alu16_return alu_ret = run_alu16(cpu, *get_reg16_from_type(cpu, op->source), (i16)relative, ADD, MEM_READ, op->flag_actions);
			sourceVal = alu_ret.result;
			cpu->registers.f = alu_ret.flags;
			break;
		}
		sourceVal = *get_reg16_from_type(cpu, op->source);
		break;
	case MEM_READ16:
		sourceVal = read16(mem, cpu->registers.pc);
		cpu->registers.pc += 2;
		break;
	case MEM_READ: {
		switch (op->source) {
		case I8: {
			u8 value = read8(mem, cpu->registers.pc++);
			i8 relative;
			if (value > 127) {
				relative = -(~value + 1);
			}
			else {
				relative = value;
			}
			sourceVal = (u16)relative;
			break;
		}
		}
		break;
	}
	default:
		sourceVal = 0;
		AddLog("unimplemented 16 bit source read\n");
		print_operation(*op);
		assert(false);
	}
	return sourceVal;
}

i16 unsigned_to_relative16(u8 x) {
	i8 relative;
	if (x > 127) {
		relative = -(~x + 1);
	}
	else relative = x;

	return (i16)relative;
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
		sourceVal = read8(mem, 0xff00 + *get_reg_from_type(cpu, op->source));
		break;
	case MEM_READ_ADDR:
		sourceVal = read8(mem, read16(mem, cpu->registers.pc));
		cpu->registers.pc += 2;
		break;
	case MEM_READ_ADDR_OFFSET:
		sourceVal = read8(mem, 0xFF00 + read8(mem, cpu->registers.pc++));
		break;
	case ADDR_MODE_NONE:
		sourceVal = op->source; // this should be fine
		break;
	default:
		sourceVal = 0;
		AddLog("unimplemented 8 bit source read\n");
		print_operation(*op);
		assert(false);
	}
	return sourceVal;
}

u8 get_dest(Cpu* cpu, Memory* mem, Operation* op) {

	u8 destVal;
	switch (op->dest_addr_mode) {
	case ADDRESS_R8_OFFSET:
	case REGISTER:
		destVal = *get_reg_from_type(cpu, op->dest);
		break;
	case ADDRESS_R16:
		destVal = read8(mem, *get_reg16_from_type(cpu, op->dest));
		break;
	case MEM_READ_ADDR_OFFSET:
	case MEM_READ:
		destVal = read8(mem, cpu->registers.pc);
		++cpu->registers.pc;
		break;
	case OPERAND_NONE:
		destVal = 0;
		break;
	default:
		destVal = 0;
		AddLog("unimplemented 8 bit dest read\n");
		print_operation(*op);
		assert(false);
	}
	return destVal;
}
u16 get_dest16(Cpu* cpu, Memory* mem, Operation* op) {
	u16 dest_val;
	switch (op->dest_addr_mode) {
	case REGISTER16:
		dest_val = *get_reg16_from_type(cpu, op->dest);
		break;
	case ADDRESS_R16:
		dest_val = read16(mem, *get_reg16_from_type(cpu, op->dest));
		break;
	default:
		AddLog("unimplemented get_dest16");
		assert(false);
		break;
	}
	return dest_val;
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

		//case REGISTER:
		//	*get_reg_from_type(cpu, op->dest) = source;
		//	break;
		//case ADDRESS_R16:
		//	write8(mem, *get_reg16_from_type(cpu, op->dest), source);
		//	break;
		//case ADDRESS_R8_OFFSET:
		//	write8(mem, (0xFF00 + get_dest(cpu, mem, op)), source);
		//	break;
		//case MEM_READ_ADDR:
		//	write8(mem, read16(mem, cpu->registers.pc), source);
		//	cpu->registers.pc += 2;
		//	break;
		//case MEM_READ_ADDR_OFFSET: {
		//	u8 reg_value = get_dest(cpu, mem, op);
		//	u16 addr = 0xFF00 + reg_value;
		//	write8(mem, addr, source);
		//	break;
		write_dest(cpu, mem, op, source);

	}
	if (op->secondary != SECONDARY_NONE) run_secondary(cpu, op);

}


void CALL_impl(Cpu* cpu, Memory* mem, Operation* op) {
	// Calls should only be u16 address mode
	u16 addr = get_source_16(cpu, mem, op);
	if (condition_passed(cpu, op)) {
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
		case REGISTER16: {
			jump(cpu, cpu->registers.hl);
			break;
		}
		default:
			AddLog("unimplemented jump");
			assert(false);
		}
	}
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
		alu_return alu_ret = run_alu(cpu, prev, 1, op->type, op->flag_actions);
		write_dest(cpu, mem, op, alu_ret.result);
		cpu->registers.f = alu_ret.flags;
		break;
	}
	default:
		AddLog("unimplemented increment");
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
	case ADDRESS_R16: {
		u8 prev = get_dest(cpu, mem, op);
		alu_return alu_ret = run_alu(cpu, prev, 1, op->type, op->flag_actions);
		write_dest(cpu, mem, op, alu_ret.result);
		cpu->registers.f = alu_ret.flags;
		break;
	}
	default:
		AddLog("unimplemented dec");
	}
}

void XOR_impl(Cpu* cpu, Memory* mem, Operation* op) {
	u8 source = get_source(cpu, mem, op);
	alu_return alu_ret = run_alu(cpu, get_dest(cpu, mem, op), source, op->type, op->flag_actions);
	write_dest(cpu, mem, op, alu_ret.result);
	cpu->registers.f = alu_ret.flags;

}

void SWAP_impl(Cpu* cpu, Memory* mem, Operation* op) {
	alu_return alu_ret = run_alu(cpu, get_dest(cpu, mem, op), 0, op->type, op->flag_actions);
	write_dest(cpu, mem, op, alu_ret.result);
	cpu->registers.f = alu_ret.flags;

}

void CP_impl(Cpu* cpu, Memory* mem, Operation* op) {
	alu_return alu_ret = run_alu(cpu, get_dest(cpu, mem, op), get_source(cpu, mem, op), op->type, op->flag_actions);
	cpu->registers.f = alu_ret.flags;
}

void SUB_impl(Cpu* cpu, Memory* mem, Operation* op) {
	alu_return alu_ret = run_alu(cpu, get_dest(cpu, mem, op), get_source(cpu, mem, op), op->type, op->flag_actions);
	write_dest(cpu, mem, op, alu_ret.result);
	cpu->registers.f = alu_ret.flags;
}

void ALU_impl(Cpu* cpu, Memory* mem, Operation* op) {
	if (bit_mode_16(op)) {
		alu16_return alu_ret = run_alu16(cpu, get_dest16(cpu, mem, op), get_source_16(cpu, mem, op), op->type, op->source_addr_mode, op->flag_actions);
		write_dest16(cpu, mem, op->dest_addr_mode, op->dest, alu_ret.result);
		cpu->registers.f = alu_ret.flags;
	}
	else {
		alu_return alu_ret = run_alu(cpu, get_dest(cpu, mem, op), get_source(cpu, mem, op), op->type, op->flag_actions);
		write_dest(cpu, mem, op, alu_ret.result);
		cpu->registers.f = alu_ret.flags;

	}
}

void PUSH_impl(Cpu* cpu, Memory* mem, Operation* op) {
	u16 to_push = get_source_16(cpu, mem, op);
	push(cpu, mem, to_push);
}

void POP_impl(Cpu* cpu, Memory* mem, Operation* op) {
	u16* reg = get_reg16_from_type(cpu, op->dest);

	pop(cpu, mem, reg, op->dest);
}

void RL_impl(Cpu* cpu, Memory* mem, Operation* op) {
	alu_return alu_ret = run_alu(cpu, get_dest(cpu, mem, op), 0, op->type, op->flag_actions);
	write_dest(cpu, mem, op, alu_ret.result);
	cpu->registers.f = alu_ret.flags;
}

void RET_impl(Cpu* cpu, Memory* mem, Operation* op) {
	if (condition_passed(cpu, op)) {
		pop(cpu, mem, get_reg16_from_type(cpu, PC), 0);
		run_secondary(cpu, op);
	}
}

void RST_impl(Cpu* cpu, Memory* mem, Operation* op) {
	push(cpu, mem, cpu->registers.pc);
	jump(cpu, op->dest);
}

void DAA_impl(Cpu* cpu, Memory* mem, Operation* op) {

	u8 offset = 0;
	u8 a = cpu->registers.a;
	u8 flags = cpu->registers.f;
	cpu->registers.f = 0;
	if ((flags & FLAG_HALFCARRY) || (!(flags & FLAG_SUB) && (a & 0xf) > 0x09)) {
		offset |= 0x06;
	}
	if ((flags & FLAG_CARRY) || (!(flags & FLAG_SUB) && (a > 0x99))) {
		offset |= 0x60;
		cpu->registers.f |= FLAG_CARRY;
	}

	if (flags & FLAG_SUB) {
		cpu->registers.a = a - offset;
	}
	else {
		cpu->registers.a = a + offset;
	}

	if (flags & FLAG_SUB) cpu->registers.f |= FLAG_SUB;
	if (cpu->registers.a == 0) cpu->registers.f |= FLAG_ZERO;

}

void CCF_impl(Cpu* cpu, Memory* mem, Operation* op) {
	u8 new_carry = ~cpu->registers.f & FLAG_CARRY;
	u8 old_zero = cpu->registers.f & FLAG_ZERO;
	cpu->registers.f |= new_carry | old_zero;
	cpu->registers.f &= (FLAG_CARRY & FLAG_ZERO);
}

Cycles cpu_step(Cpu* cpu, Memory* mem, Operation op) {
	++cpu->registers.pc;

	if (cpu->registers.pc == 0x101) {
		mem->in_bios = false;
	}

	switch (op.type) {
	case NOP:
		break;
	case HALT:
		cpu->halted = true;
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
	case ADC:
	case SUB:
	case SBC:
	case SET_OP:
	case OR:
	case AND:
	case CPL:
	case RES:
	case SLA:
	case SRL:
	case SRA:
	case RR:
	case RLC:
	case RRC:
	case SCF:
	case CCF:
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
	case RETI:
		RET_impl(cpu, mem, &op);
		update_IME(cpu, true);
		break;

	case RST:
		RST_impl(cpu, mem, &op);
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
		Cycles cb_ret = cpu_step(cpu, mem, get_cb_operation(cpu, mem));
		if (cb_ret.t_cycles == -1) {
			AddLog("Unimplemented CB op\n");
			return (Cycles) { -1, -1 };
		}
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

	case DAA:
		DAA_impl(cpu, mem, &op);
		break;
	case UNIMPLEMENTED:
	default:
		--cpu->registers.pc;
		AddLog("\nunimplemented operation\t");
		print_operation(op);
		return (Cycles) { -1, -1 };
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

		op.m_cycles += 5;
		op.t_cycles += 20;
	}

	return (Cycles) { op.m_cycles, op.t_cycles };
}

void destroy_cpu(Cpu* cpu) {
	if (cpu == NULL) return;
	free(cpu);
}

Cycles run_halted(Cpu* cpu, Memory* mem) {
	Cycles cycles = { 1, 4 };

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
		cycles.m_cycles += 5;
		cycles.t_cycles += 20;
	}
	return cycles;
}

u16 interrupt_address_from_flag(u8 flag) {
	switch (flag) {
	case VBLANK_INTERRUPT:
		return VBLANK_ADDRESS;
	case STAT_INTERRUPT:
		return LCDSTAT_ADDRESS;
	case TIMER_INTERRUPT:
		return TIMER_ADDRESS;
	case SERIAL_INTERRUPT:
		return SERIAL_ADDRESS;
	case JOYPAD_INTERRUPT:
		return JOYPAD_ADDRESS;
	default:
		AddLog("how did we get here: 0x%02X", flag);
		return 0;
	}
}

u16 interrupt_priority(Cpu* cpu, Memory* mem, u8 interrupt_flag) {
	for (int i = 0; i < 5; ++i) {
		u8 itX = interrupt_flag & (1 << i);
		itX = (read8(mem, IE) & itX);
		if (itX) {
			write8(mem, IF, interrupt_flag & ~itX);
			return interrupt_address_from_flag(itX);
		}
	}
	return 0;
}

void run_interrupt(Cpu* cpu, Memory* mem) {
	u16 jump_to = interrupt_priority(cpu, mem, read8(mem, IF));
	if (jump_to != 0) {
		
		cpu->IME = false;
		push(cpu, mem, cpu->registers.pc);
		jump(cpu, jump_to);
	}
}

bool should_run_interrupt(Cpu* cpu, Memory* mem) {
	u8 j_ret = joypad_return(mem->controller, mem->memory[0xFF00]);
	if ((~j_ret & 0b00001111)) mem->memory[IF] = mem->memory[IF] | JOYPAD_INTERRUPT; // I think this is right
	u8 interrupt_flag = mem->memory[IF];
	u8 interrupt_enable = mem->memory[IE];
	
	if ((interrupt_flag & interrupt_enable)) {
		cpu->halted = false;
		if (cpu->IME) return true;
		else return false;
	}
	else {
		return false;
	}
}
