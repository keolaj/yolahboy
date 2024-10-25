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

	// JUMPS

	[0x18] = {"JR, i8", JP, ADDR_MODE_NONE, MEM_READ, OPERAND_NONE, I8, CONDITION_NONE, SECONDARY_NONE, 2, 12 } ,
	[0x20] = {"JR NZ, i8", JP, ADDR_MODE_NONE, MEM_READ, OPERAND_NONE, I8, CONDITION_NZ, ADD_T_4, 2, 8 } ,
	[0x28] = {"JR Z, i8", JP, ADDR_MODE_NONE, MEM_READ, OPERAND_NONE, I8, CONDITION_Z, ADD_T_4, 2, 8 } ,
	[0x38] = {"JR C, i8", JP, ADDR_MODE_NONE, MEM_READ, OPERAND_NONE, I8, CONDITION_C, ADD_T_4, 2, 8 } ,
	[0x30] = {"JR NC, i8", JP, ADDR_MODE_NONE, MEM_READ, OPERAND_NONE, I8, CONDITION_NC, ADD_T_4, 2, 8 } ,

	[0xC3] = {"JP u16", JP, ADDR_MODE_NONE, MEM_READ16, OPERAND_NONE, U16, CONDITION_NONE, SECONDARY_NONE, 3, 16 },
	[0xC2] = {"JP NZ u16", JP, ADDR_MODE_NONE, MEM_READ16, OPERAND_NONE, U16, CONDITION_NZ, ADD_T_4, 3, 12 },
	[0xCA] = {"JP Z u16", JP, ADDR_MODE_NONE, MEM_READ16, OPERAND_NONE, U16, CONDITION_Z, ADD_T_4, 3, 12 },
	[0xD2] = {"JP NC u16", JP, ADDR_MODE_NONE, MEM_READ16, OPERAND_NONE, U16, CONDITION_NC, ADD_T_4, 3, 12 },
	[0xDA] = {"JP C u16", JP, ADDR_MODE_NONE, MEM_READ16, OPERAND_NONE, U16, CONDITION_C, ADD_T_4, 3, 12 },
	[0xEA] = {"JP HL", JP, ADDR_MODE_NONE, REGISTER, OPERAND_NONE, HL, CONDITION_NONE, SECONDARY_NONE, 1, 4 },
	// MISC
	[0xCB] = {"PREFIX CB", CB, MEM_READ, ADDR_MODE_NONE, OPERAND_NONE, OPERAND_NONE, 0, 0, 1, 4},
};

Operation cb_operations[0x100] = {

	// BIT B, X // PUT THE BIT YOU ARE COMPARING INTO DEST AND REGISTER INTO SOURCE AND DONT ASK ANY QUESTIONS (if you think about it this actually makes more sense)	
	[0x47] = { "BIT 0, B", BIT, ADDR_MODE_NONE, REGISTER, 0, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x40] = { "BIT 0, C", BIT, ADDR_MODE_NONE, REGISTER, 0, C, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x41] = { "BIT 0, D", BIT, ADDR_MODE_NONE, REGISTER, 0, D, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x42] = { "BIT 0, E", BIT, ADDR_MODE_NONE, REGISTER, 0, E, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x43] = { "BIT 0, H", BIT, ADDR_MODE_NONE, REGISTER, 0, H, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x44] = { "BIT 0, L", BIT, ADDR_MODE_NONE, REGISTER, 0, L, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x45] = { "BIT 0, (HL)", BIT, ADDR_MODE_NONE, ADDRESS_R16, 0, HL, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },

	[0x4F] = { "BIT 1, A", BIT, ADDR_MODE_NONE, REGISTER, 1, A, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x48] = { "BIT 1, B", BIT, ADDR_MODE_NONE, REGISTER, 1, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x49] = { "BIT 1, C", BIT, ADDR_MODE_NONE, REGISTER, 1, C, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x4A] = { "BIT 1, D", BIT, ADDR_MODE_NONE, REGISTER, 1, D, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x4B] = { "BIT 1, E", BIT, ADDR_MODE_NONE, REGISTER, 1, E, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x4C] = { "BIT 1, H", BIT, ADDR_MODE_NONE, REGISTER, 1, H, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x4D] = { "BIT 1, L", BIT, ADDR_MODE_NONE, REGISTER, 1, L, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x4E] = { "BIT 1, (HL)", BIT, ADDR_MODE_NONE, ADDRESS_R16, 1, HL, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },

	[0x57] = { "BIT 2, A", BIT, ADDR_MODE_NONE, REGISTER, 2, A, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x50] = { "BIT 2, B", BIT, ADDR_MODE_NONE, REGISTER, 2, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x51] = { "BIT 2, C", BIT, ADDR_MODE_NONE, REGISTER, 2, C, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x52] = { "BIT 2, D", BIT, ADDR_MODE_NONE, REGISTER, 2, D, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x53] = { "BIT 2, E", BIT, ADDR_MODE_NONE, REGISTER, 2, E, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x54] = { "BIT 2, H", BIT, ADDR_MODE_NONE, REGISTER, 2, H, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x55] = { "BIT 2, L", BIT, ADDR_MODE_NONE, REGISTER, 2, L, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x56] = { "BIT 2, (HL)", BIT, ADDR_MODE_NONE, ADDRESS_R16, 2, HL, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },

	[0x5F] = { "BIT 3, A", BIT, ADDR_MODE_NONE, REGISTER, 3, A, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} }, [0x78] = { "BIT 7, B", BIT, ADDR_MODE_NONE, REGISTER, 7, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x58] = { "BIT 3, B", BIT, ADDR_MODE_NONE, REGISTER, 3, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x59] = { "BIT 3, C", BIT, ADDR_MODE_NONE, REGISTER, 3, C, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x5A] = { "BIT 3, D", BIT, ADDR_MODE_NONE, REGISTER, 3, D, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x5B] = { "BIT 3, E", BIT, ADDR_MODE_NONE, REGISTER, 3, E, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x5C] = { "BIT 3, H", BIT, ADDR_MODE_NONE, REGISTER, 3, H, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x5D] = { "BIT 3, L", BIT, ADDR_MODE_NONE, REGISTER, 3, L, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x5E] = { "BIT 3, (HL)", BIT, ADDR_MODE_NONE, ADDRESS_R16, 3, HL, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },

	[0x67] = { "BIT 4, B", BIT, ADDR_MODE_NONE, REGISTER, 4, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x60] = { "BIT 4, B", BIT, ADDR_MODE_NONE, REGISTER, 4, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x61] = { "BIT 4, C", BIT, ADDR_MODE_NONE, REGISTER, 4, C, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x62] = { "BIT 4, D", BIT, ADDR_MODE_NONE, REGISTER, 4, D, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x63] = { "BIT 4, E", BIT, ADDR_MODE_NONE, REGISTER, 4, E, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x64] = { "BIT 4, H", BIT, ADDR_MODE_NONE, REGISTER, 4, H, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x65] = { "BIT 4, L", BIT, ADDR_MODE_NONE, REGISTER, 4, L, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x66] = { "BIT 4, (HL)", BIT, ADDR_MODE_NONE, ADDRESS_R16, 4, HL, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },

	[0x6F] = { "BIT 5, A", BIT, ADDR_MODE_NONE, REGISTER, 5, A, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} }, [0x78] = { "BIT 7, B", BIT, ADDR_MODE_NONE, REGISTER, 7, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x68] = { "BIT 5, B", BIT, ADDR_MODE_NONE, REGISTER, 5, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x69] = { "BIT 5, C", BIT, ADDR_MODE_NONE, REGISTER, 5, C, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x6A] = { "BIT 5, D", BIT, ADDR_MODE_NONE, REGISTER, 5, D, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x6B] = { "BIT 5, E", BIT, ADDR_MODE_NONE, REGISTER, 5, E, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x6C] = { "BIT 5, H", BIT, ADDR_MODE_NONE, REGISTER, 5, H, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x6D] = { "BIT 5, L", BIT, ADDR_MODE_NONE, REGISTER, 5, L, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x6E] = { "BIT 5, (HL)", BIT, ADDR_MODE_NONE, ADDRESS_R16, 5, HL, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },

	[0x77] = { "BIT 6, B", BIT, ADDR_MODE_NONE, REGISTER, 6, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x70] = { "BIT 6, B", BIT, ADDR_MODE_NONE, REGISTER, 6, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x71] = { "BIT 6, C", BIT, ADDR_MODE_NONE, REGISTER, 6, C, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x72] = { "BIT 6, D", BIT, ADDR_MODE_NONE, REGISTER, 6, D, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x73] = { "BIT 6, E", BIT, ADDR_MODE_NONE, REGISTER, 6, E, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x74] = { "BIT 6, H", BIT, ADDR_MODE_NONE, REGISTER, 6, H, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x75] = { "BIT 6, L", BIT, ADDR_MODE_NONE, REGISTER, 6, L, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
	[0x76] = { "BIT 6, (HL)", BIT, ADDR_MODE_NONE, ADDRESS_R16, 6, HL, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },


	[0x7F] = { "BIT 7, A", BIT, ADDR_MODE_NONE, REGISTER, 7, A, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} }, [0x78] = { "BIT 7, B", BIT, ADDR_MODE_NONE, REGISTER, 7, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, IGNORE} },
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
