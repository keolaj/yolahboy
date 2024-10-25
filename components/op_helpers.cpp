#include "op_helpers.h"

uint8_t calculateFlags(uint8_t x, uint8_t y, bool sub) {
	uint8_t ret = 0;
	uint8_t result; // we have to do this because of stupid compiler
	if (sub) {
		result = x - y;
		ret |= 0b01000000; // set subtraction flag
		if (result == 0) {
			ret |= 0b10000000;
		}
		if (((x & 0b00001111) - (y & 0b00001111) & 0x10) == 0x10) {
			ret |= 0b00100000;
		}
		if ((int)x - (int)(y) < 0) {
			ret |= 0b00010000;
		}

		return ret;
	}
	result = x + y;

	if (result == 0) {
		ret |= 0b10000000; // set zero flag
	}
	if (((x & 0b00001111) + (y & 0b00001111) & 0x10) == 0x10) {
		ret |= 0b00100000; // set half carry flag
	}
	if ((int)x + (int)y > 255) {
		ret |= 0b00010000; // set carry flag
	}
	return ret;
}
uint8_t calculateFlags16(uint16_t x, uint16_t y, bool sub) {
	uint8_t ret = 0;

	if (x + y == 0) {
		// ret |= 0b10000000; // set zero flag
	}
	if (sub) {
		if (((x & 0b0000000011111111) - (y & 0b0000000011111111) & 0x100) == 0x100) {
			ret |= 0b00100000; // set half carry flag
		}
		if ((int)x - (int)y < 0) {
			ret |= 0b00010000; // set carry flag
		}
		return ret;

	}

	if (((x & 0b0000000011111111) + (y & 0b0000000011111111) & 0x100) == 0x100) {
		ret |= 0b00100000; // set half carry flag
	}
	if ((int)x + (int)y >= 0xFFFF) {
		ret |= 0b00010000; // set carry flag
	}
	return ret;

}

CBRet SRL_X(uint8_t reg) {
	CBRet ret{ 0, 0, {2, 8} };
	if (reg & 1) {
		ret.flags |= 0b00010000;
	}
	ret.reg = reg >> 1;
	if (ret.reg == 0) {
		ret.flags |= Flags::ZERO;
	}
	return ret;
}

CBRet SLA_R(uint8_t reg) {
	CBRet ret{ 0, 0, {0, 0} };

	uint8_t highBit = reg & 0b10000000;

	ret.reg = reg << 1;
	ret.flags = highBit >> 3;
	if (ret.reg == 0) {
		ret.flags |= 0b10000000;
	}
	ret.cycles.m_cycles = 2;
	ret.cycles.t_cycles = 8;

	return ret;
}
CBRet SETX_R(int bit, uint8_t reg, uint8_t flags) {
	CBRet ret{ 0, 0, {0, 0} };
	ret.reg = reg |= 1 << bit;
	ret.cycles.m_cycles = 2;
	ret.cycles.t_cycles = 8;
	ret.flags = 0b00100000 | (flags & 0b00010000); // half carry is always set (according to docs) // also keep carry flag here
	if (ret.reg == 0) {
		ret.flags |= 0b10000000;
	}
	return ret;
}
CBRet BITX_R(int bit, uint8_t reg, uint8_t flags) {
	CBRet ret{ 0, 0, {0, 0} };
	ret.reg = reg;
	ret.cycles.m_cycles = 2;
	ret.cycles.t_cycles = 8;
	ret.flags = 0b00100000 | (flags & 0b00010000); // half carry is always set (according to docs) // also keep carry flag here
	uint8_t mask = 1 << bit;
	if ((ret.reg & mask) == 0) {
		ret.flags |= 0b10000000;
	}
	return ret;
}
CBRet RL_X(uint8_t reg, uint8_t flags) {
	CBRet ret{ 0, 0, {0, 0} };
	ret.reg = reg << 1 | ((flags & 0b00010000) >> 4);
	ret.flags |= ((reg & 0b10000000) >> 3);
	if (ret.reg == 0) {
		ret.flags |= 0b10000000;
	}
	ret.cycles.m_cycles = 2;
	ret.cycles.t_cycles = 8;
	return ret;
}
CBRet RLC_X(uint8_t reg, uint8_t flags) {
	CBRet ret{ 0, 0, {2, 8} };
	uint8_t highBit = ((reg & 0b10000000) >> 7);

	ret.reg = reg << 1 | highBit;
	if (ret.reg == 0) {
		ret.flags |= Flags::ZERO;
	}
	if (highBit) {
		ret.flags |= Flags::CARRY;
	}
	return ret;
}
CBRet RR_X(uint8_t reg, uint8_t flags) {
	CBRet ret{ 0,0,{2,8} };
	ret.reg = reg >> 1 | ((flags & Flags::CARRY) << 3);
	ret.flags |= (reg & 0b00000001) << 4;
	if (ret.reg == 0) {
		ret.flags |= 0b10000000;
	}
	return ret;
}
CBRet RRC_X(uint8_t reg, uint8_t flags) {
	CBRet ret{ 0, 0, {1, 4} };
	uint8_t lowBit = reg & 1;

	ret.reg = reg >> 1 & (lowBit << 7);
	if (ret.reg == 0) {
		ret.flags |= Flags::ZERO;
	}
	if (lowBit) {
		ret.flags |= Flags::CARRY;
	}
	return ret;
}

CBRet SWAP_X(uint8_t reg) {
	CBRet ret{ 0, 0, {0, 0} };
	ret.reg = (reg << 4) | (reg & 0x0f);
	if (ret.reg == 0) {
		ret.flags |= 0b10000000;
	}
	ret.cycles.m_cycles = 2;
	ret.cycles.t_cycles = 8;
	return ret;
}

CBRet CPR_X(uint8_t reg, uint8_t val, uint8_t flags) {
	CBRet ret{ 0, 0, {1, 4} };
	ret.reg = reg;
	ret.flags = calculateFlags(reg, val, true);

	return ret;
}
CBRet ANDR_X(uint8_t reg, uint8_t value) {
	CBRet ret{ 0, 0, {1, 4} };
	ret.reg = reg & value;
	ret.flags |= 0b00100000;
	if (ret.reg == 0) {
		ret.flags |= 0b10000000;
	}
	return ret;
}
CBRet INCR(uint8_t reg, uint8_t flags) { // confirmed working
	CBRet ret{ 0, 0, {1, 4} };
	uint8_t newFlags = calculateFlags(reg, 1, false);
	ret.reg = ++reg;
	ret.flags = (newFlags & 0b11100000);
	ret.flags |= (flags & 0b00010000);
	return ret;

}
CBRet DECR(uint8_t reg, uint8_t flags) { // confirmed working
	CBRet ret{ 0, 0, {1, 4} };
	uint8_t newFlags = calculateFlags(reg, 1, true);
	ret.reg = --reg;
	ret.flags = (newFlags & 0b11100000);
	ret.flags |= (flags & 0b00010000);
	return ret;
}

CBRet RESX_R(int bit, uint8_t reg) {
	CBRet ret{ 0, 0, {2, 8} }; // 16t for (HL) dont forget to change in cpu.h
	uint8_t mask = 1 << bit;
	ret.reg = reg & ~mask; // dont need to change flags
	return ret;
}


void printRegistersH(const Registers& reg) {
	std::cout << "af: " << std::bitset<16>(reg.af) << " : 0x" << (int)reg.af << "\n";
	std::cout << "bc: " << std::bitset<16>(reg.bc) << " : 0x" << (int)reg.bc << "\n";
	std::cout << "de: " << std::bitset<16>(reg.de) << " : 0x" << (int)reg.de << "\n";
	std::cout << "hl: " << std::bitset<16>(reg.hl) << " : 0x" << (int)reg.hl << "\n";
	std::cout << "sp: " << std::bitset<16>(reg.sp) << " : 0x" << (int)reg.sp << "\n";
	std::cout << "pc: " << std::bitset<16>(reg.pc) << " : 0x" << (int)reg.pc << "\n";
}

