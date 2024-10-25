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
	[0x7C] = {"BIT 7, H"},

};


void print_operation(Operation op) {
	printf("0x%02X: \t%s\n", op.opcode, op.mnemonic);
}
