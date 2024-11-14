#pragma once
#include "cpu_definitons.h"
#include <iostream>
#include <bitset>

uint8_t calculateFlags(uint8_t x, uint8_t y, bool sub);
uint8_t calculateFlags16(uint16_t x, uint16_t y, bool sub);

CBRet SLA_R(uint8_t reg);
CBRet SETX_R(int bit, uint8_t reg, uint8_t flags);
CBRet BITX_R(int bit, uint8_t reg, uint8_t flags);
CBRet RESX_R(int bit, uint8_t reg);
CBRet RL_X(uint8_t reg, uint8_t flags); // Rotate left carry
CBRet RLC_X(uint8_t reg, uint8_t flags); // Rotate left circular
CBRet RR_X(uint8_t reg, uint8_t flags);
CBRet RRC_X(uint8_t reg, uint8_t flags);
CBRet SRL_X(uint8_t reg); // Shift right into carry
CBRet CPR_X(uint8_t reg, uint8_t val, uint8_t flags);
CBRet SWAP_X(uint8_t reg);
CBRet ANDR_X(uint8_t reg, uint8_t val);
CBRet DECR(uint8_t reg, uint8_t flags);
CBRet INCR(uint8_t reg, uint8_t flags);


void printRegistersH(const Registers& reg);