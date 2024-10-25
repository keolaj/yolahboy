#pragma once
#include "global_definitions.h"
#include <stdbool.h>

typedef struct {
	union {
		struct {
			u8 f;
			u8 a;
		};
		u16 af;
	};
	union {
		struct {
			u8 c;
			u8 b;
		};
		u16 bc;
	};
	union {
		struct {
			u8 e;
			u8 d;
		};
		u16 de;
	};
	union {
		struct {
			u8 l;
			u8 h;
		};
		u16 hl;
	};
	u16 sp;
	u16 pc;
} Registers;

typedef struct {
	Registers registers;
	bool IME;
} Cpu;

typedef struct {
	u8 m_cycles;
	u8 t_cycles;
} Cycles;
