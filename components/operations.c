#include "operations.h"

Operation operations[0x100] = {
	[0x00] = {"NOP", NOP, OPERAND_NONE, OPERAND_NONE, ADDR_MODE_NONE, ADDR_MODE_NONE, 0, 0, 1, 4, },

	// 8 bit loads

	// LD (HL)
	[0x32] = {"LD (HL-), A", LD, ADDRESS_R16, REGISTER, HL, A, 0, DEC_R_1, 1, 8 },

	// 16 bit loads

	// LD SP
	[0x31] = {"LD SP, u16", LD, REGISTER16, MEM_READ16, SP, U16, 0, 0, 3, 12, },
	[0xF9] = {"LD SP, HL", LD, REGISTER16, REGISTER16, SP, HL, 0, 0, 1, 8, },

	// LD HL
	[0x21] = {"LD HL, u16", LD, REGISTER16, MEM_READ16, HL, U16, 0, 0, 3, 12, },

	// XOR
	[0xAF] = {"XOR A, A", XOR, REGISTER, REGISTER, A, A, 0, 0, 1, 4, {DEPENDENT, RESET, RESET, RESET} },

	// MISC
	[0xCB] = {"PREFIX CB", CB, MEM_READ, ADDR_MODE_NONE, OPERAND_NONE, OPERAND_NONE, 0, 0, 1, 4},
};

Operation cb_operations[0x100] = {
	[0x7F] = { "BIT 7, A", BIT, ADDR_MODE_NONE, REGISTER, 7, A, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x78] = { "BIT 7, B", BIT, ADDR_MODE_NONE, REGISTER, 7, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x79] = { "BIT 7, C", BIT, ADDR_MODE_NONE, REGISTER, 7, C, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x7A] = { "BIT 7, D", BIT, ADDR_MODE_NONE, REGISTER, 7, D, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x7B] = { "BIT 7, E", BIT, ADDR_MODE_NONE, REGISTER, 7, E, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x7C] = { "BIT 7, H", BIT, ADDR_MODE_NONE, REGISTER, 7, H, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x7D] = { "BIT 7, L", BIT, ADDR_MODE_NONE, REGISTER, 7, L, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x7E] = { "BIT 7, (HL)", BIT, ADDR_MODE_NONE, ADDRESS_R16, 7, HL, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },

};


void print_operation(Operation op) {
	printf("0x%02X: \t%s\n", op.opcode, op.mnemonic);
}
