#include "operations.h"
#include "memory2.h"
#include <stdio.h>

Operation operations[0x100] = {
	[0x00] = {"NOP", NOP, OPERAND_NONE, OPERAND_NONE, ADDR_MODE_NONE, ADDR_MODE_NONE, 0, 0, 1, 4, },

	// 8 bit loads
	// 
	// LD A, R
	[0x7F] = {"LD A, A", LD, REGISTER, REGISTER, A, A, 0, 0, 1, 4},
	[0x78] = {"LD A, B", LD, REGISTER, REGISTER, A, B, 0, 0, 1, 4},
	[0x79] = {"LD A, C", LD, REGISTER, REGISTER, A, C, 0, 0, 1, 4},
	[0x7A] = {"LD A, D", LD, REGISTER, REGISTER, A, D, 0, 0, 1, 4},
	[0x7B] = {"LD A, E", LD, REGISTER, REGISTER, A, E, 0, 0, 1, 4},
	[0x7C] = {"LD A, H", LD, REGISTER, REGISTER, A, H, 0, 0, 1, 4},
	[0x7D] = {"LD A, L", LD, REGISTER, REGISTER, A, L, 0, 0, 1, 4},
	[0x0A] = {"LD A, (BC)", LD, REGISTER, ADDRESS_R16, A, BC, 0, 0, 1, 8},
	[0x7E] = {"LD A, (HL)", LD, REGISTER, ADDRESS_R16, A, HL, 0, 0, 1, 8},
	[0x2A] = {"LD A, (HL+)", LD, REGISTER, ADDRESS_R16, A, HL, 0, INC_R_2, 1, 8},
	[0x3A] = {"LD A, (HL-)", LD, REGISTER, ADDRESS_R16, A, HL, 0, DEC_R_2, 1, 8},
	[0x1A] = {"LD A, (DE)", LD, REGISTER, ADDRESS_R16, A, DE, 0, 0, 1, 8},
	[0xFA] = {"LD A, (u16)", LD, REGISTER, MEM_READ_ADDR, A, U16, 0, 0, 3, 16},

	[0x47] = {"LD B, A", LD, REGISTER, REGISTER, B, A, 0, 0, 1, 4},
	[0x40] = {"LD A, B", LD, REGISTER, REGISTER, B, B, 0, 0, 1, 4},
	[0x41] = {"LD A, C", LD, REGISTER, REGISTER, B, C, 0, 0, 1, 4},
	[0x42] = {"LD A, D", LD, REGISTER, REGISTER, B, D, 0, 0, 1, 4},
	[0x43] = {"LD A, E", LD, REGISTER, REGISTER, B, E, 0, 0, 1, 4},
	[0x44] = {"LD A, H", LD, REGISTER, REGISTER, B, H, 0, 0, 1, 4},
	[0x45] = {"LD A, L", LD, REGISTER, REGISTER, B, L, 0, 0, 1, 4},
	[0x46] = {"LD A, (HL)", LD, REGISTER, ADDRESS_R16, B, HL, 0, 0, 1, 8},
	[0xF0] = {"LD A, (FF00 + u8)", LD, REGISTER, ADDRESS_R8_OFFSET, A, U8, 0, 0, 2, 12},

	[0x4F] = {"LD C, A", LD, REGISTER, REGISTER, C, A, 0, 0, 1, 4},
	[0x48] = {"LD C, B", LD, REGISTER, REGISTER, C, B, 0, 0, 1, 4},
	[0x49] = {"LD C, C", LD, REGISTER, REGISTER, C, C, 0, 0, 1, 4},
	[0x4A] = {"LD C, D", LD, REGISTER, REGISTER, C, D, 0, 0, 1, 4},
	[0x4B] = {"LD C, E", LD, REGISTER, REGISTER, C, E, 0, 0, 1, 4},
	[0x4C] = {"LD C, H", LD, REGISTER, REGISTER, C, H, 0, 0, 1, 4},
	[0x4D] = {"LD C, L", LD, REGISTER, REGISTER, C, L, 0, 0, 1, 4},
	[0x4E] = {"LD C, (HL)", LD, REGISTER, ADDRESS_R16, C, HL, 0, 0, 1, 8},

	[0x57] = {"LD D, A", LD, REGISTER, REGISTER, D, A, 0, 0, 1, 4},
	[0x50] = {"LD D, B", LD, REGISTER, REGISTER, D, B, 0, 0, 1, 4},
	[0x51] = {"LD D, C", LD, REGISTER, REGISTER, D, C, 0, 0, 1, 4},
	[0x52] = {"LD D, D", LD, REGISTER, REGISTER, D, D, 0, 0, 1, 4},
	[0x53] = {"LD D, E", LD, REGISTER, REGISTER, D, E, 0, 0, 1, 4},
	[0x54] = {"LD D, H", LD, REGISTER, REGISTER, D, H, 0, 0, 1, 4},
	[0x55] = {"LD D, L", LD, REGISTER, REGISTER, D, L, 0, 0, 1, 4},
	[0x56] = {"LD D, (HL)", LD, REGISTER, ADDRESS_R16, D, HL, 0, 0, 1, 8},

	[0x5F] = {"LD E, A", LD, REGISTER, REGISTER, E, A, 0, 0, 1, 4},
	[0x58] = {"LD E, B", LD, REGISTER, REGISTER, E, B, 0, 0, 1, 4},
	[0x59] = {"LD E, C", LD, REGISTER, REGISTER, E, C, 0, 0, 1, 4},
	[0x5A] = {"LD E, D", LD, REGISTER, REGISTER, E, D, 0, 0, 1, 4},
	[0x5B] = {"LD E, E", LD, REGISTER, REGISTER, E, E, 0, 0, 1, 4},
	[0x5C] = {"LD E, H", LD, REGISTER, REGISTER, E, H, 0, 0, 1, 4},
	[0x5D] = {"LD E, L", LD, REGISTER, REGISTER, E, L, 0, 0, 1, 4},
	[0x5E] = {"LD E, (HL)", LD, REGISTER, ADDRESS_R16, E, HL, 0, 0, 1, 8},

	[0x67] = {"LD H, A", LD, REGISTER, REGISTER, H, A, 0, 0, 1, 4},
	[0x60] = {"LD H, B", LD, REGISTER, REGISTER, H, B, 0, 0, 1, 4},
	[0x61] = {"LD H, C", LD, REGISTER, REGISTER, H, C, 0, 0, 1, 4},
	[0x62] = {"LD H, D", LD, REGISTER, REGISTER, H, D, 0, 0, 1, 4},
	[0x63] = {"LD H, E", LD, REGISTER, REGISTER, H, E, 0, 0, 1, 4},
	[0x64] = {"LD H, H", LD, REGISTER, REGISTER, H, H, 0, 0, 1, 4},
	[0x65] = {"LD H, L", LD, REGISTER, REGISTER, H, L, 0, 0, 1, 4},
	[0x66] = {"LD H, (HL)", LD, REGISTER, ADDRESS_R16, H, HL, 0, 0, 1, 8},

	[0x6F] = {"LD L, A", LD, REGISTER, REGISTER, L, A, 0, 0, 1, 4},
	[0x68] = {"LD L, B", LD, REGISTER, REGISTER, L, B, 0, 0, 1, 4},
	[0x69] = {"LD L, C", LD, REGISTER, REGISTER, L, C, 0, 0, 1, 4},
	[0x6A] = {"LD L, D", LD, REGISTER, REGISTER, L, D, 0, 0, 1, 4},
	[0x6B] = {"LD L, E", LD, REGISTER, REGISTER, L, E, 0, 0, 1, 4},
	[0x6C] = {"LD L, H", LD, REGISTER, REGISTER, L, H, 0, 0, 1, 4},
	[0x6D] = {"LD L, L", LD, REGISTER, REGISTER, L, L, 0, 0, 1, 4},
	[0x6E] = {"LD L, (HL)", LD, REGISTER, ADDRESS_R16, L, HL, 0, 0, 1, 8},

	[0xE2] = {"LD (FF00 + C), A", LD, ADDRESS_R8_OFFSET, REGISTER, C, A, 0, 0, 1, 8},
	// 
	// 
	// LD X, u8
	[0x3E] = {"LD A, u8", LD, REGISTER, MEM_READ, A, U8, 0, 0, 2, 8},
	[0x06] = {"LD B, u8", LD, REGISTER, MEM_READ, B, U8, 0, 0, 2, 8},
	[0x0E] = {"LD C, u8", LD, REGISTER, MEM_READ, C, U8, 0, 0, 2, 8},
	[0x16] = {"LD D, u8", LD, REGISTER, MEM_READ, D, U8, 0, 0, 2, 8},
	[0x1E] = {"LD E, u8", LD, REGISTER, MEM_READ, E, U8, 0, 0, 2, 8},
	[0x26] = {"LD H, u8", LD, REGISTER, MEM_READ, H, U8, 0, 0, 2, 8},
	[0x2E] = {"LD L, u8", LD, REGISTER, MEM_READ, L, U8, 0, 0, 2, 8},

	[0xE0] = {"LD (FF00 + u8), A", LD, MEM_READ_ADDR_OFFSET, REGISTER, U8, A, 0, 0, 2, 12},

	// LD (HL)
	[0x12] = {"LD (DE), A", LD, ADDRESS_R16, REGISTER, DE, A, 0, SECONDARY_NONE, 1, 8 },

	[0x77] = {"LD (HL), A", LD, ADDRESS_R16, REGISTER, HL, A, 0, SECONDARY_NONE, 1, 8 },
	[0x32] = {"LD (HL-), A", LD, ADDRESS_R16, REGISTER, HL, A, 0, DEC_R_1, 1, 8 },
	[0x22] = {"LD (HL+), A", LD, ADDRESS_R16, REGISTER, HL, A, 0, INC_R_1, 1, 8 },
	[0x70] = {"LD (HL), B", LD, ADDRESS_R16, REGISTER, HL, B, 0, SECONDARY_NONE, 1, 8 },
	[0x71] = {"LD (HL), C", LD, ADDRESS_R16, REGISTER, HL, C, 0, SECONDARY_NONE, 1, 8 },
	[0x72] = {"LD (HL), D", LD, ADDRESS_R16, REGISTER, HL, D, 0, SECONDARY_NONE, 1, 8 },
	[0x73] = {"LD (HL), E", LD, ADDRESS_R16, REGISTER, HL, E, 0, SECONDARY_NONE, 1, 8 },
	[0x36] = {"LD (HL), u8", LD, ADDRESS_R16, MEM_READ, HL, U8, 0, 0, 2, 12},

	// 16 bit loads

	[0xEA] = {"LD (u16), A", LD, MEM_READ_ADDR, REGISTER, U16, A, 0, 0, 3, 16},
	[0x08] = {"LD (u16), SP", LD, MEM_READ_ADDR, REGISTER16, U16, SP, 0, 0, 3, 16},

	[0x01] = {"LD BC, u16", LD, REGISTER16, MEM_READ16, BC, U16, 0, 0, 3, 12},
	[0x11] = {"LD DE, u16", LD, REGISTER16, MEM_READ16, DE, U16, 0, 0, 3, 12},

	// LD SP
	[0x31] = {"LD SP, u16", LD, REGISTER16, MEM_READ16, SP, U16, 0, 0, 3, 12, },
	[0xF9] = {"LD SP, HL", LD, REGISTER16, REGISTER16, SP, HL, 0, 0, 1, 8, },

	// LD HL
[0x21] = { "LD HL, u16", LD, REGISTER16, MEM_READ16, HL, U16, 0, 0, 3, 12, },
[0xF8] = { "LD HL, SP + i8", LD, REGISTER16, REGISTER16, HL, SP_ADD_I8, 0, 0, 3, 12, },

// ALU
// INC r8
[0x3C] = { "INC A", INC, REGISTER, ADDR_MODE_NONE, A, OPERAND_NONE, 0, 0, 1, 4, {DEPENDENT, RESET, DEPENDENT, _IGNORE} },
[0x04] = { "INC B", INC, REGISTER, ADDR_MODE_NONE, B, OPERAND_NONE, 0, 0, 1, 4, {DEPENDENT, RESET, DEPENDENT, _IGNORE} },
[0x0C] = { "INC C", INC, REGISTER, ADDR_MODE_NONE, C, OPERAND_NONE, 0, 0, 1, 4, {DEPENDENT, RESET, DEPENDENT, _IGNORE} },
[0x14] = { "INC D", INC, REGISTER, ADDR_MODE_NONE, D, OPERAND_NONE, 0, 0, 1, 4, {DEPENDENT, RESET, DEPENDENT, _IGNORE} },
[0x1C] = { "INC E", INC, REGISTER, ADDR_MODE_NONE, E, OPERAND_NONE, 0, 0, 1, 4, {DEPENDENT, RESET, DEPENDENT, _IGNORE} },
[0x24] = { "INC H", INC, REGISTER, ADDR_MODE_NONE, H, OPERAND_NONE, 0, 0, 1, 4, {DEPENDENT, RESET, DEPENDENT, _IGNORE} },
[0x2C] = { "INC L", INC, REGISTER, ADDR_MODE_NONE, L, OPERAND_NONE, 0, 0, 1, 4, {DEPENDENT, RESET, DEPENDENT, _IGNORE} },
[0x34] = { "INC (HL)", INC, ADDRESS_R16, ADDR_MODE_NONE, HL, OPERAND_NONE, 0, 0, 1, 4, {DEPENDENT, RESET, DEPENDENT, _IGNORE} },

// DEC R8
[0x3D] = { "DEC A", DEC, REGISTER, ADDR_MODE_NONE, A, OPERAND_NONE, 0, 0, 1, 4, {DEPENDENT, SET, DEPENDENT, _IGNORE} },
[0x05] = { "DEC B", DEC, REGISTER, ADDR_MODE_NONE, B, OPERAND_NONE, 0, 0, 1, 4, {DEPENDENT, SET, DEPENDENT, _IGNORE} },
[0x0D] = { "DEC C", DEC, REGISTER, ADDR_MODE_NONE, C, OPERAND_NONE, 0, 0, 1, 4, {DEPENDENT, SET, DEPENDENT, _IGNORE} },
[0x15] = { "DEC D", DEC, REGISTER, ADDR_MODE_NONE, D, OPERAND_NONE, 0, 0, 1, 4, {DEPENDENT, SET, DEPENDENT, _IGNORE} },
[0x1D] = { "DEC E", DEC, REGISTER, ADDR_MODE_NONE, E, OPERAND_NONE, 0, 0, 1, 4, {DEPENDENT, SET, DEPENDENT, _IGNORE} },
[0x25] = { "DEC H", DEC, REGISTER, ADDR_MODE_NONE, H, OPERAND_NONE, 0, 0, 1, 4, {DEPENDENT, SET, DEPENDENT, _IGNORE} },
[0x2D] = { "DEC L", DEC, REGISTER, ADDR_MODE_NONE, L, OPERAND_NONE, 0, 0, 1, 4, {DEPENDENT, SET, DEPENDENT, _IGNORE} },

[0x35] = { "DEC (HL)", DEC, ADDRESS_R16, ADDR_MODE_NONE, HL, OPERAND_NONE, 0, 0, 1, 4, {DEPENDENT, SET, DEPENDENT, _IGNORE} },

// ADD
[0x87] = { "ADD A, A", ADD, REGISTER, REGISTER, A, A, 0, 0, 1, 4, {DEPENDENT, RESET, DEPENDENT, DEPENDENT} },
[0x80] = { "ADD A, B", ADD, REGISTER, REGISTER, A, B, 0, 0, 1, 4, {DEPENDENT, RESET, DEPENDENT, DEPENDENT} },
[0x81] = { "ADD A, C", ADD, REGISTER, REGISTER, A, C, 0, 0, 1, 4, {DEPENDENT, RESET, DEPENDENT, DEPENDENT} },
[0x82] = { "ADD A, D", ADD, REGISTER, REGISTER, A, D, 0, 0, 1, 4, {DEPENDENT, RESET, DEPENDENT, DEPENDENT} },
[0x83] = { "ADD A, E", ADD, REGISTER, REGISTER, A, E, 0, 0, 1, 4, {DEPENDENT, RESET, DEPENDENT, DEPENDENT} },
[0x84] = { "ADD A, H", ADD, REGISTER, REGISTER, A, H, 0, 0, 1, 4, {DEPENDENT, RESET, DEPENDENT, DEPENDENT} },
[0x85] = { "ADD A, L", ADD, REGISTER, REGISTER, A, L, 0, 0, 1, 4, {DEPENDENT, RESET, DEPENDENT, DEPENDENT} },
[0x86] = { "ADD A, (HL)", ADD, REGISTER, ADDRESS_R16, A, HL, 0, 0, 1, 8, {DEPENDENT, RESET, DEPENDENT, DEPENDENT} },
[0xC6] = { "ADD A, u8", ADD, REGISTER, MEM_READ, A, U8, 0, 0, 2, 8, {DEPENDENT, RESET, DEPENDENT, DEPENDENT} },

[0x89] = { "ADC A, C", ADC, REGISTER, REGISTER, A, C, 0, 0, 1, 4, {DEPENDENT, RESET, DEPENDENT, DEPENDENT} },
[0xCE] = { "ADC A, u8", ADC, REGISTER, MEM_READ, A, U8, 0, 0, 2, 8, {DEPENDENT, RESET, DEPENDENT, DEPENDENT} },
// SUB
[0x97] = { "SUB A, A", SUB, REGISTER, REGISTER, A, A, 0, 0, 1, 4, {DEPENDENT, SET, DEPENDENT, DEPENDENT} },
[0x90] = { "SUB A, B", SUB, REGISTER, REGISTER, A, B, 0, 0, 1, 4, {DEPENDENT, SET, DEPENDENT, DEPENDENT} },
[0x96] = { "SUB A, (HL)", SUB, REGISTER, ADDRESS_R16, A, HL, 0, 0, 2, 8, {DEPENDENT, SET, DEPENDENT, DEPENDENT} },
[0xD6] = { "SUB A, u8", SUB, REGISTER, MEM_READ, A, U8, 0, 0, 2, 8, {DEPENDENT, SET, DEPENDENT, DEPENDENT} },

[0x99] = {"SBC A, C", SBC, REGISTER, REGISTER, A, C, 0, 0, 1, 4, {DEPENDENT, SET, DEPENDENT, DEPENDENT}}, 
[0xDE] = {"SBC A, u8", SBC, REGISTER, MEM_READ, A, U8, 0, 0, 2, 8, {DEPENDENT, SET, DEPENDENT, DEPENDENT}}, 

// CP
[0xB8] = { "CP A, B", CP, REGISTER, REGISTER, A, B, 0, 0, 2, 8, {DEPENDENT, SET, DEPENDENT, DEPENDENT} },
[0xB9] = { "CP A, C", CP, REGISTER, REGISTER, A, C, 0, 0, 2, 8, {DEPENDENT, SET, DEPENDENT, DEPENDENT} },
[0xBE] = { "CP A, (HL)", CP, REGISTER, ADDRESS_R16, A, HL, 0, 0, 1, 8, {DEPENDENT, SET, DEPENDENT, DEPENDENT} },
[0xFE] = { "CP A, u8", CP, REGISTER, MEM_READ, A, U8, 0, 0, 2, 8, {DEPENDENT, SET, DEPENDENT, DEPENDENT} },

//INC r16
[0x03] = { "INC BC", INC, REGISTER16, ADDR_MODE_NONE, BC, OPERAND_NONE, 0, 0, 1, 8 },
[0x13] = { "INC DE", INC, REGISTER16, ADDR_MODE_NONE, DE, OPERAND_NONE, 0, 0, 1, 8 },
[0x23] = { "INC HL", INC, REGISTER16, ADDR_MODE_NONE, HL, OPERAND_NONE, 0, 0, 1, 8 },
[0x33] = { "INC SP", INC, REGISTER16, ADDR_MODE_NONE, SP, OPERAND_NONE, 0, 0, 1, 8 },

// DEC r16
[0x0B] = { "DEC BC", DEC, REGISTER16, ADDR_MODE_NONE, BC, OPERAND_NONE, 0, 0, 1, 8 },
[0x1B] = { "DEC DE", DEC, REGISTER16, ADDR_MODE_NONE, DE, OPERAND_NONE, 0, 0, 1, 8 },
[0x2B] = { "DEC HL", DEC, REGISTER16, ADDR_MODE_NONE, HL, OPERAND_NONE, 0, 0, 1, 8 },
[0x3B] = { "DEC SP", DEC, REGISTER16, ADDR_MODE_NONE, SP, OPERAND_NONE, 0, 0, 1, 8 },

[0x09] = { "ADD HL, BC", ADD, REGISTER16, REGISTER16, HL, BC, 0, 0, 1, 8, {_IGNORE, RESET, DEPENDENT, DEPENDENT} },
[0x19] = { "ADD HL, DE", ADD, REGISTER16, REGISTER16, HL, DE, 0, 0, 1, 8, {_IGNORE, RESET, DEPENDENT, DEPENDENT} },
[0x29] = { "ADD HL, HL", ADD, REGISTER16, REGISTER16, HL, HL, 0, 0, 1, 8, {_IGNORE, RESET, DEPENDENT, DEPENDENT} },
[0x39] = { "ADD HL, SP", ADD, REGISTER16, REGISTER16, HL, SP, 0, 0, 1, 8, {_IGNORE, RESET, DEPENDENT, DEPENDENT} },

[0xE8] = { "ADD SP, i8", LD, REGISTER16, REGISTER16, SP, SP_ADD_I8, 0, 0, 2, 16, {RESET, RESET, DEPENDENT, DEPENDENT} }, // TRUST ME THIS IS THE EASIEST WAY

[0x17] = { "RLA", RL, REGISTER, ADDR_MODE_NONE, A, OPERAND_NONE, 0, 0, 1, 4, {DEPENDENT, RESET, RESET, DEPENDENT} },

// AND
[0xA7] = { "AND A, A", AND, REGISTER, REGISTER, A, A, 0, 0, 2, 8, {DEPENDENT, RESET, SET, RESET} },
[0xA0] = { "AND A, B", AND, REGISTER, REGISTER, A, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, RESET} },
[0xA1] = { "AND A, C", AND, REGISTER, REGISTER, A, C, 0, 0, 2, 8, {DEPENDENT, RESET, SET, RESET} },
[0xA2] = { "AND A, D", AND, REGISTER, REGISTER, A, D, 0, 0, 2, 8, {DEPENDENT, RESET, SET, RESET} },
[0xA3] = { "AND A, E", AND, REGISTER, REGISTER, A, E, 0, 0, 2, 8, {DEPENDENT, RESET, SET, RESET} },
[0xA4] = { "AND A, H", AND, REGISTER, REGISTER, A, H, 0, 0, 2, 8, {DEPENDENT, RESET, SET, RESET} },
[0xA5] = { "AND A, L", AND, REGISTER, REGISTER, A, L, 0, 0, 2, 8, {DEPENDENT, RESET, SET, RESET} },
[0xE6] = { "AND A, u8", AND, REGISTER, MEM_READ, A, U8, 0, 0, 2, 8, {DEPENDENT, RESET, SET, RESET} },

// OR
[0xB7] = { "OR A, A", OR, REGISTER, REGISTER, A, A, 0, 0, 1, 4, {DEPENDENT, RESET, RESET, RESET} },
[0xB0] = { "OR A, B", OR, REGISTER, REGISTER, A, B, 0, 0, 1, 4, {DEPENDENT, RESET, RESET, RESET} },
[0xB1] = { "OR A, C", OR, REGISTER, REGISTER, A, C, 0, 0, 1, 4, {DEPENDENT, RESET, RESET, RESET} },
[0xB2] = { "OR A, D", OR, REGISTER, REGISTER, A, D, 0, 0, 1, 4, {DEPENDENT, RESET, RESET, RESET} },
[0xB3] = { "OR A, E", OR, REGISTER, REGISTER, A, E, 0, 0, 1, 4, {DEPENDENT, RESET, RESET, RESET} },
[0xB4] = { "OR A, H", OR, REGISTER, REGISTER, A, H, 0, 0, 1, 4, {DEPENDENT, RESET, RESET, RESET} },
[0xB5] = { "OR A, L", OR, REGISTER, REGISTER, A, L, 0, 0, 1, 4, {DEPENDENT, RESET, RESET, RESET} },
[0xB6] = { "OR A, (HL)", OR, REGISTER, ADDRESS_R16, A, HL, 0, 0, 1, 8, {DEPENDENT, RESET, RESET, RESET} },
[0xF6] = { "OR A, U8", OR, REGISTER, MEM_READ, A, U8, 0, 0, 1, 4, {DEPENDENT, RESET, RESET, RESET} },
// XOR
[0xAF] = { "XOR A, A", XOR, REGISTER, REGISTER, A, A, 0, 0, 1, 4, {DEPENDENT, RESET, RESET, RESET} },
[0xA8] = { "XOR A, B", XOR, REGISTER, REGISTER, A, B, 0, 0, 1, 4, {DEPENDENT, RESET, RESET, RESET} },
[0xA9] = { "XOR A, C", XOR, REGISTER, REGISTER, A, C, 0, 0, 1, 4, {DEPENDENT, RESET, RESET, RESET} },
[0xAA] = { "XOR A, D", XOR, REGISTER, REGISTER, A, D, 0, 0, 1, 4, {DEPENDENT, RESET, RESET, RESET} },
[0xAB] = { "XOR A, E", XOR, REGISTER, REGISTER, A, E, 0, 0, 1, 4, {DEPENDENT, RESET, RESET, RESET} },
[0xAC] = { "XOR A, H", XOR, REGISTER, REGISTER, A, H, 0, 0, 1, 4, {DEPENDENT, RESET, RESET, RESET} },
[0xAD] = { "XOR A, L", XOR, REGISTER, REGISTER, A, L, 0, 0, 1, 4, {DEPENDENT, RESET, RESET, RESET} },
[0xAE] = { "XOR A, (HL)", XOR, REGISTER, ADDRESS_R16, A, HL, 0, 0, 1, 8, {DEPENDENT, RESET, RESET, RESET} },
[0xEE] = { "XOR A, u8", XOR, REGISTER, MEM_READ, A, U8, 0, 0, 2, 8, {DEPENDENT, RESET, RESET, RESET} },

[0x2F] = { "CPL A", CPL, REGISTER, ADDR_MODE_NONE, A, OPERAND_NONE, 0, 0, 1, 4, {_IGNORE, SET, SET, _IGNORE} },

// PUSH
[0xF5] = { "PUSH AF", PUSH, ADDR_MODE_NONE, REGISTER16, OPERAND_NONE, AF, 0, 0, 1, 16 },
[0xC5] = { "PUSH BC", PUSH, ADDR_MODE_NONE, REGISTER16, OPERAND_NONE, BC, 0, 0, 1, 16 },
[0xD5] = { "PUSH DE", PUSH, ADDR_MODE_NONE, REGISTER16, OPERAND_NONE, DE, 0, 0, 1, 16 },
[0xE5] = { "PUSH HL", PUSH, ADDR_MODE_NONE, REGISTER16, OPERAND_NONE, HL, 0, 0, 1, 16 },

// POP
[0xC1] = { "POP BC", POP, REGISTER16, ADDR_MODE_NONE, BC, OPERAND_NONE, 0, 0, 1, 12 },
[0xD1] = { "POP DE", POP, REGISTER16, ADDR_MODE_NONE, DE, OPERAND_NONE, 0, 0, 1, 12 },
[0xE1] = { "POP HL", POP, REGISTER16, ADDR_MODE_NONE, HL, OPERAND_NONE, 0, 0, 1, 12 },
[0xF1] = { "POP AF", POP, REGISTER16, ADDR_MODE_NONE, AF, OPERAND_NONE, 0, 0, 1, 12 },


// JUMPS

[0x18] = { "JR, i8", JP, ADDR_MODE_NONE, MEM_READ, OPERAND_NONE, I8, CONDITION_NONE, SECONDARY_NONE, 2, 12 },
[0x20] = { "JR NZ, i8", JP, ADDR_MODE_NONE, MEM_READ, OPERAND_NONE, I8, CONDITION_NZ, ADD_T_4, 2, 8 },
[0x28] = { "JR Z, i8", JP, ADDR_MODE_NONE, MEM_READ, OPERAND_NONE, I8, CONDITION_Z, ADD_T_4, 2, 8 },
[0x38] = { "JR C, i8", JP, ADDR_MODE_NONE, MEM_READ, OPERAND_NONE, I8, CONDITION_C, ADD_T_4, 2, 8 },
[0x30] = { "JR NC, i8", JP, ADDR_MODE_NONE, MEM_READ, OPERAND_NONE, I8, CONDITION_NC, ADD_T_4, 2, 8 },

[0xC3] = { "JP u16", JP, ADDR_MODE_NONE, MEM_READ16, OPERAND_NONE, U16, CONDITION_NONE, SECONDARY_NONE, 3, 16 },
[0xC2] = { "JP NZ u16", JP, ADDR_MODE_NONE, MEM_READ16, OPERAND_NONE, U16, CONDITION_NZ, ADD_T_4, 3, 12 },
[0xCA] = { "JP Z u16", JP, ADDR_MODE_NONE, MEM_READ16, OPERAND_NONE, U16, CONDITION_Z, ADD_T_4, 3, 12 },
[0xD2] = { "JP NC u16", JP, ADDR_MODE_NONE, MEM_READ16, OPERAND_NONE, U16, CONDITION_NC, ADD_T_4, 3, 12 },
[0xDA] = { "JP C u16", JP, ADDR_MODE_NONE, MEM_READ16, OPERAND_NONE, U16, CONDITION_C, ADD_T_4, 3, 12 },
[0xE9] = { "JP HL", JP, ADDR_MODE_NONE, REGISTER16, OPERAND_NONE, HL, CONDITION_NONE, SECONDARY_NONE, 1, 4 },

// CALLS
[0xCD] = { "CALL u16", CALL, ADDR_MODE_NONE, MEM_READ16, OPERAND_NONE, U16, CONDITION_NONE, SECONDARY_NONE, 3, 24 },
[0xC4] = { "CALL NZ u16", CALL, ADDR_MODE_NONE, MEM_READ16, OPERAND_NONE, U16, CONDITION_NONE, ADD_T_12, 3, 12 },

// RST (store rst jump in dest)
[0xCF] = { "RST 08", RST, ADDR_MODE_NONE, ADDR_MODE_NONE, 0x8, OPERAND_NONE, CONDITION_NONE, SECONDARY_NONE, 1, 16 },
[0xDF] = { "RST 18", RST, ADDR_MODE_NONE, ADDR_MODE_NONE, 0x18, OPERAND_NONE, CONDITION_NONE, SECONDARY_NONE, 1, 16 },
[0xEF] = { "RST 28", RST, ADDR_MODE_NONE, ADDR_MODE_NONE, 0x28, OPERAND_NONE, CONDITION_NONE, SECONDARY_NONE, 1, 16 },
[0xFF] = { "RST 38", RST, ADDR_MODE_NONE, ADDR_MODE_NONE, 0x38, OPERAND_NONE, CONDITION_NONE, SECONDARY_NONE, 1, 16 },

// RET
[0xC9] = {"RET", RET, ADDR_MODE_NONE, ADDR_MODE_NONE, OPERAND_NONE, OPERAND_NONE, CONDITION_NONE, SECONDARY_NONE, 1, 16},
[0xC8] = {"RET Z", RET, ADDR_MODE_NONE, ADDR_MODE_NONE, OPERAND_NONE, OPERAND_NONE, CONDITION_Z, SECONDARY_NONE, 1, 16},
[0xC0] = {"RET NZ", RET, ADDR_MODE_NONE, ADDR_MODE_NONE, OPERAND_NONE, OPERAND_NONE, CONDITION_NZ, SECONDARY_NONE, 1, 16},
[0xD8] = {"RET C", RET, ADDR_MODE_NONE, ADDR_MODE_NONE, OPERAND_NONE, OPERAND_NONE, CONDITION_C, SECONDARY_NONE, 1, 16},
[0xD0] = {"RET NC", RET, ADDR_MODE_NONE, ADDR_MODE_NONE, OPERAND_NONE, OPERAND_NONE, CONDITION_NC, SECONDARY_NONE, 1, 16},

[0xD9] = {"RETI", RETI, ADDR_MODE_NONE, ADDR_MODE_NONE, OPERAND_NONE, OPERAND_NONE, CONDITION_NONE, SECONDARY_NONE, 1, 16},
// MISC
[0xCB] = {"PREFIX CB", CB, MEM_READ, ADDR_MODE_NONE, OPERAND_NONE, OPERAND_NONE, 0, 0, 1, 4},
[0x27] = {"DAA", DAA, 0, 0, 0, 0, 0, 0, 1, 4}, 
[0x3F] = {"CCF", CCF, 0, 0, 0, 0, 0, 0, 1, 4}, 
[0x1F] = {"RRA", RR, REGISTER, ADDR_MODE_NONE, A, OPERAND_NONE, 0, 0, 1, 4, {RESET, RESET, RESET, DEPENDENT}}, 
[0x07] = {"RLCA", RLC, REGISTER, ADDR_MODE_NONE, A, OPERAND_NONE, 0, 0, 1, 4, {RESET, RESET, RESET, DEPENDENT}},

[0xFB] = {"EI", EI, 0, 0, 0, 0, 0, 0, 1, 4},
[0xF3] = {"DI", DI, 0, 0, 0, 0, 0, 0, 1, 4},
};

Operation cb_operations[0x100] = {

	// BIT B, X // PUT THE BIT YOU ARE COMPARING INTO DEST AND REGISTER INTO SOURCE AND DONT ASK ANY QUESTIONS (if you think about it this actually makes more sense)	
	[0x47] = { "BIT 0, B", BIT, ADDR_MODE_NONE, REGISTER, 0, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x40] = { "BIT 0, C", BIT, ADDR_MODE_NONE, REGISTER, 0, C, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x41] = { "BIT 0, D", BIT, ADDR_MODE_NONE, REGISTER, 0, D, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x42] = { "BIT 0, E", BIT, ADDR_MODE_NONE, REGISTER, 0, E, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x43] = { "BIT 0, H", BIT, ADDR_MODE_NONE, REGISTER, 0, H, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x44] = { "BIT 0, L", BIT, ADDR_MODE_NONE, REGISTER, 0, L, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x45] = { "BIT 0, (HL)", BIT, ADDR_MODE_NONE, ADDRESS_R16, 0, HL, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },

	[0x4F] = { "BIT 1, A", BIT, ADDR_MODE_NONE, REGISTER, 1, A, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x48] = { "BIT 1, B", BIT, ADDR_MODE_NONE, REGISTER, 1, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x49] = { "BIT 1, C", BIT, ADDR_MODE_NONE, REGISTER, 1, C, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x4A] = { "BIT 1, D", BIT, ADDR_MODE_NONE, REGISTER, 1, D, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x4B] = { "BIT 1, E", BIT, ADDR_MODE_NONE, REGISTER, 1, E, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x4C] = { "BIT 1, H", BIT, ADDR_MODE_NONE, REGISTER, 1, H, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x4D] = { "BIT 1, L", BIT, ADDR_MODE_NONE, REGISTER, 1, L, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x4E] = { "BIT 1, (HL)", BIT, ADDR_MODE_NONE, ADDRESS_R16, 1, HL, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },

	[0x57] = { "BIT 2, A", BIT, ADDR_MODE_NONE, REGISTER, 2, A, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x50] = { "BIT 2, B", BIT, ADDR_MODE_NONE, REGISTER, 2, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x51] = { "BIT 2, C", BIT, ADDR_MODE_NONE, REGISTER, 2, C, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x52] = { "BIT 2, D", BIT, ADDR_MODE_NONE, REGISTER, 2, D, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x53] = { "BIT 2, E", BIT, ADDR_MODE_NONE, REGISTER, 2, E, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x54] = { "BIT 2, H", BIT, ADDR_MODE_NONE, REGISTER, 2, H, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x55] = { "BIT 2, L", BIT, ADDR_MODE_NONE, REGISTER, 2, L, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x56] = { "BIT 2, (HL)", BIT, ADDR_MODE_NONE, ADDRESS_R16, 2, HL, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },

	[0x5F] = { "BIT 3, A", BIT, ADDR_MODE_NONE, REGISTER, 3, A, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },[0x78] = { "BIT 7, B", BIT, ADDR_MODE_NONE, REGISTER, 7, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x58] = { "BIT 3, B", BIT, ADDR_MODE_NONE, REGISTER, 3, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x59] = { "BIT 3, C", BIT, ADDR_MODE_NONE, REGISTER, 3, C, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x5A] = { "BIT 3, D", BIT, ADDR_MODE_NONE, REGISTER, 3, D, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x5B] = { "BIT 3, E", BIT, ADDR_MODE_NONE, REGISTER, 3, E, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x5C] = { "BIT 3, H", BIT, ADDR_MODE_NONE, REGISTER, 3, H, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x5D] = { "BIT 3, L", BIT, ADDR_MODE_NONE, REGISTER, 3, L, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x5E] = { "BIT 3, (HL)", BIT, ADDR_MODE_NONE, ADDRESS_R16, 3, HL, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },

	[0x67] = { "BIT 4, B", BIT, ADDR_MODE_NONE, REGISTER, 4, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x60] = { "BIT 4, B", BIT, ADDR_MODE_NONE, REGISTER, 4, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x61] = { "BIT 4, C", BIT, ADDR_MODE_NONE, REGISTER, 4, C, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x62] = { "BIT 4, D", BIT, ADDR_MODE_NONE, REGISTER, 4, D, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x63] = { "BIT 4, E", BIT, ADDR_MODE_NONE, REGISTER, 4, E, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x64] = { "BIT 4, H", BIT, ADDR_MODE_NONE, REGISTER, 4, H, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x65] = { "BIT 4, L", BIT, ADDR_MODE_NONE, REGISTER, 4, L, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x66] = { "BIT 4, (HL)", BIT, ADDR_MODE_NONE, ADDRESS_R16, 4, HL, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },

	[0x6F] = { "BIT 5, A", BIT, ADDR_MODE_NONE, REGISTER, 5, A, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },[0x78] = { "BIT 7, B", BIT, ADDR_MODE_NONE, REGISTER, 7, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x68] = { "BIT 5, B", BIT, ADDR_MODE_NONE, REGISTER, 5, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x69] = { "BIT 5, C", BIT, ADDR_MODE_NONE, REGISTER, 5, C, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x6A] = { "BIT 5, D", BIT, ADDR_MODE_NONE, REGISTER, 5, D, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x6B] = { "BIT 5, E", BIT, ADDR_MODE_NONE, REGISTER, 5, E, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x6C] = { "BIT 5, H", BIT, ADDR_MODE_NONE, REGISTER, 5, H, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x6D] = { "BIT 5, L", BIT, ADDR_MODE_NONE, REGISTER, 5, L, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x6E] = { "BIT 5, (HL)", BIT, ADDR_MODE_NONE, ADDRESS_R16, 5, HL, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },

	[0x77] = { "BIT 6, B", BIT, ADDR_MODE_NONE, REGISTER, 6, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x70] = { "BIT 6, B", BIT, ADDR_MODE_NONE, REGISTER, 6, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x71] = { "BIT 6, C", BIT, ADDR_MODE_NONE, REGISTER, 6, C, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x72] = { "BIT 6, D", BIT, ADDR_MODE_NONE, REGISTER, 6, D, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x73] = { "BIT 6, E", BIT, ADDR_MODE_NONE, REGISTER, 6, E, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x74] = { "BIT 6, H", BIT, ADDR_MODE_NONE, REGISTER, 6, H, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x75] = { "BIT 6, L", BIT, ADDR_MODE_NONE, REGISTER, 6, L, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x76] = { "BIT 6, (HL)", BIT, ADDR_MODE_NONE, ADDRESS_R16, 6, HL, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },


	[0x7F] = { "BIT 7, A", BIT, ADDR_MODE_NONE, REGISTER, 7, A, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x78] = { "BIT 7, B", BIT, ADDR_MODE_NONE, REGISTER, 7, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x78] = { "BIT 7, B", BIT, ADDR_MODE_NONE, REGISTER, 7, B, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x79] = { "BIT 7, C", BIT, ADDR_MODE_NONE, REGISTER, 7, C, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x7A] = { "BIT 7, D", BIT, ADDR_MODE_NONE, REGISTER, 7, D, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x7B] = { "BIT 7, E", BIT, ADDR_MODE_NONE, REGISTER, 7, E, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x7C] = { "BIT 7, H", BIT, ADDR_MODE_NONE, REGISTER, 7, H, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x7D] = { "BIT 7, L", BIT, ADDR_MODE_NONE, REGISTER, 7, L, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },
	[0x7E] = { "BIT 7, (HL)", BIT, ADDR_MODE_NONE, ADDRESS_R16, 7, HL, 0, 0, 2, 8, {DEPENDENT, RESET, SET, _IGNORE} },

	// RL
	[0x10] = {"RL B", RL, REGISTER, ADDR_MODE_NONE, B, OPERAND_NONE, 0, 0, 2, 8, {DEPENDENT, RESET, RESET, DEPENDENT}},

	[0x1F] = {"RR A", RR, REGISTER, ADDR_MODE_NONE, A, OPERAND_NONE, 0, 0, 2, 8, {DEPENDENT, RESET, RESET, DEPENDENT}},
	[0x19] = {"RR C", RR, REGISTER, ADDR_MODE_NONE, C, OPERAND_NONE, 0, 0, 2, 8, {DEPENDENT, RESET, RESET, DEPENDENT}},
	[0x1A] = {"RR D", RR, REGISTER, ADDR_MODE_NONE, D, OPERAND_NONE, 0, 0, 2, 8, {DEPENDENT, RESET, RESET, DEPENDENT}},
	[0x1B] = {"RR E", RR, REGISTER, ADDR_MODE_NONE, E, OPERAND_NONE, 0, 0, 2, 8, {DEPENDENT, RESET, RESET, DEPENDENT}},

	// SLA
	[0x27] = {"SLA A", SLA, REGISTER, ADDR_MODE_NONE, A, OPERAND_NONE, 0, 0, 2, 8, {DEPENDENT, RESET, RESET, DEPENDENT}},
	
	// SRA
	[0x3A] = {"SRA D", SRA, REGISTER, ADDR_MODE_NONE, D, OPERAND_NONE, 0, 0, 2, 8, {DEPENDENT, RESET, RESET, DEPENDENT}}, 

	// SRL
	[0x3F] = {"SRL A", SRL, REGISTER, ADDR_MODE_NONE, A, OPERAND_NONE, 0, 0, 2, 8, {DEPENDENT, RESET, RESET, DEPENDENT}}, 
	[0x38] = {"SRL B", SRL, REGISTER, ADDR_MODE_NONE, B, OPERAND_NONE, 0, 0, 2, 8, {DEPENDENT, RESET, RESET, DEPENDENT}}, 

	// RES
	[0x87] = {"RES 0, A", RES, REGISTER, ADDR_MODE_NONE, A, 0, 0, 0, 2, 8, {_IGNORE, _IGNORE, _IGNORE, _IGNORE}},
	[0x86] = {"RES 0, (HL)", RES, ADDRESS_R16, ADDR_MODE_NONE, HL, 0, 0, 0, 2, 16, {_IGNORE, _IGNORE, _IGNORE, _IGNORE}},
	
	[0xBE] = {"RES 7, (HL)", RES, ADDRESS_R16, ADDR_MODE_NONE, HL, 7, 0, 0, 2, 16, {_IGNORE, _IGNORE, _IGNORE, _IGNORE}},

	// SWAP
	[0x37] = {"SWAP A", SWAP, REGISTER, ADDR_MODE_NONE, A, OPERAND_NONE, 0, 0, 2, 8, {DEPENDENT, RESET, RESET, RESET}},
	[0x30] = {"SWAP B", SWAP, REGISTER, ADDR_MODE_NONE, B, OPERAND_NONE, 0, 0, 2, 8, {DEPENDENT, RESET, RESET, RESET}},
	[0x31] = {"SWAP C", SWAP, REGISTER, ADDR_MODE_NONE, C, OPERAND_NONE, 0, 0, 2, 8, {DEPENDENT, RESET, RESET, RESET}},
	[0x32] = {"SWAP D", SWAP, REGISTER, ADDR_MODE_NONE, D, OPERAND_NONE, 0, 0, 2, 8, {DEPENDENT, RESET, RESET, RESET}},
	[0x33] = {"SWAP E", SWAP, REGISTER, ADDR_MODE_NONE, E, OPERAND_NONE, 0, 0, 2, 8, {DEPENDENT, RESET, RESET, RESET}},
	[0x34] = {"SWAP H", SWAP, REGISTER, ADDR_MODE_NONE, H, OPERAND_NONE, 0, 0, 2, 8, {DEPENDENT, RESET, RESET, RESET}},
	[0x35] = {"SWAP L", SWAP, REGISTER, ADDR_MODE_NONE, L, OPERAND_NONE, 0, 0, 2, 8, {DEPENDENT, RESET, RESET, RESET}},

	// SET
	[0xFE] = {"SET 7, (HL)", SET_OP, ADDRESS_R16, ADDR_MODE_NONE, HL, 7, 0, 0, 2, 16, }, 

};

void print_operation(Operation op) {
	printf("0x%02X: \t%s\n", op.opcode, op.mnemonic);
}

void push(Cpu* cpu, Memory* mem, u16 value) {
	write16(mem, cpu->registers.sp - 2, value);
	cpu->registers.sp -= 2;
}
void pop(Cpu* cpu, Memory* mem, u16* reg) {
	*reg = read16(mem, cpu->registers.sp);
	cpu->registers.sp += 2;
}
void jump(Cpu* cpu, u16 jump_to) {
	cpu->registers.pc = jump_to;
}


u16 interrupt_address_from_flag(u8 flag) {
	switch (flag) {
	case VBLANK_INTERRUPT:
		return VBLANK_ADDRESS;
	case LCDSTAT_INTERRUPT:
		return LCDSTAT_ADDRESS;
	case TIMER_INTERRUPT:
		return TIMER_ADDRESS;
	case SERIAL_INTERRUPT:
		return SERIAL_ADDRESS;
	case JOYPAD_INTERRUPT:
		return JOYPAD_ADDRESS;
	default:
		printf("how did we get here: 0x%02X", flag);
		return 0;
	}
}

u16 interrupt_priority(Cpu* cpu, Memory* mem, u8 interrupt_flag) {
	for (int i = 0; i < 5; ++i) {
		u8 itX = interrupt_flag & (1 << i);
		itX &= (read8(mem, IE) & itX);
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

		push(cpu, mem, cpu->registers.pc);
		jump(cpu, jump_to);
		cpu->IME = false;
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
	case ADC:
		if (cpu->registers.f & FLAG_CARRY) {
			y += 1;
		}
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

	case SBC:
		if (cpu->registers.f & FLAG_CARRY) {
			y -= 1;
		}
		result = x - y;
		if ((y & 0x0f) > (x & 0x0f)) { //  half carry (there is a lot of different documentation on this so idk, this matches bgb) 
			new_flags &= ~FLAG_HALFCARRY;
		}
		else {
			new_flags |= FLAG_HALFCARRY;
		}
		if ((int)x - (int)y < 0) { //  carry
			new_flags &= ~FLAG_CARRY;
		}
		else {
			new_flags |= FLAG_CARRY;
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
		result = x > 1;
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
		if (cpu->registers.f & FLAG_CARRY) result |= 0b10000000;
		if (x & 0b00000001) new_flags |= FLAG_CARRY;
		result |= (x >> 1);
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


	}



	if (result == 0) {
		new_flags |= FLAG_ZERO;
	}

	new_flags |= (generate_ignore_mask(flag_actions) & cpu->registers.f);
	new_flags &= generate_reset_mask(flag_actions);


	return (alu_return) { result, new_flags };
}

alu16_return run_alu16(Cpu* cpu, u16 x, u16 y, instruction_type type, instruction_flags flag_actions) {
	u8 new_flags = 0;
	new_flags |= generate_set_mask(flag_actions);
	u16 result;
	switch (type) {
	case ADD:
		result = x + y;
		if ((int)x + (int)y > 0xFFFF) {
			new_flags |= FLAG_CARRY;
		}
		if (((x & 0xff) + (y & 0xff)) > 0xff) {
			new_flags |= FLAG_HALFCARRY;
		}
		break;

	default:
		printf("TODO: Unimplemented run_alu16 type");
		result = 0;
		assert(false);
		break;
	}
	if (result == 0) {
		new_flags |= FLAG_ZERO;
	}
	new_flags |= (generate_ignore_mask(flag_actions) & cpu->registers.f);
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
	default:
		printf("unimplemented write dest addr mode");
		assert(false);
	}
}

void write_dest16(Cpu* cpu, Memory* mem, address_mode mode, operand_type dest, u16 value) {
	switch (mode) {
	case REGISTER16:
		*get_reg16_from_type(cpu, dest) = value;
		break;
	default:
		printf("unimplemented write dest16 type");
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
	default:
		return;
	}
}

u16 get_source_16(Cpu* cpu, Memory* mem, Operation* op) {
	u16 sourceVal;
	switch (op->source_addr_mode) {
	case REGISTER16:
		if (op->source == SP_ADD_I8) {
			u8 value = read8(mem, cpu->registers.pc++);
			if (value > 127) {
				sourceVal = *get_reg16_from_type(cpu, op->source) - (~value + 1);
			}
			else {
				sourceVal = *get_reg16_from_type(cpu, op->source) + value;
			}
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
		default:
			printf("What are we doing here");
			assert(false);
		}
		break;
	}
	default:
		sourceVal = 0;
		printf("unimplemented 16 bit source read\n");
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
		sourceVal = read8(mem, 0xff00 + read8(mem, cpu->registers.pc));
		++cpu->registers.pc;
		break;
	case MEM_READ_ADDR:
		sourceVal = read8(mem, read16(mem, cpu->registers.pc));
		cpu->registers.pc += 2;
		break;
	case ADDR_MODE_NONE:
		sourceVal = op->source; // this should be fine
		break;
	default:
		sourceVal = 0;
		printf("unimplemented 8 bit source read\n");
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
	default:
		destVal = 0;
		printf("unimplemented 8 bit dest read\n");
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
		printf("unimplemented get_dest16");
		assert(false);
		break;
	}
	return dest_val;
}


