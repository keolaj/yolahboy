//#include <SDL3/SDL.h>
//
//#include <Windows.h>
//#include "components/global_definitions.h"
//
//#include "emulator_main.h"
//#include "components/global_definitions.h"
//#include "components/gpu_definitions.h"
//#include "components/emulator.h"
//#include "components/operations.h"
//#include "components/operation_defitions.h"
//#include "components/cpu.h"
//#include "components/memory.h"
//#include "components/gpu.h"
//
//// extern Emulator emu;
//extern LPHANDLE emu_breakpoint_event;
//extern LPHANDLE emu_draw_event;
//extern CRITICAL_SECTION emu_crit;
//
//
//int run_emulator(LPVOID t_args) {
//
//	int c = 0;
//	bool quit = false;
//	while (!quit) {
//		EnterCriticalSection(&emu_crit);
//		for (int i = 0; i < MAX_BREAKPOINTS; ++i) {
//			if ((int)emu.cpu->registers.pc == emu.breakpoints[i]) {
//				printf("BREAKPOINT!!\n");
//				LeaveCriticalSection(&emu_crit);
//				SetEvent(emu_breakpoint_event);
//				// print_registers(emu.cpu);
//
//
//				SuspendThread(GetCurrentThread());
//				EnterCriticalSection(&emu_crit);
//				break;
//			}
//		}
//
//		Operation to_execute = get_operation(emu.cpu, emu.memory);
//		// print_operation(to_execute);
//		Cycles clock = step_cpu(emu.cpu, emu.memory, to_execute);
//		if (clock.m_cycles == -1 && clock.t_cycles == -1) {
//			emu.should_quit = true;
//		}
//		// print_registers(emu.cpu);
//		c += clock.t_cycles;
//		step_gpu(emu.gpu, clock.t_cycles);
//		if (c > 29780) {
//
//			LeaveCriticalSection(&emu_crit);
//			SetEvent(emu_draw_event);
//			SuspendThread(GetCurrentThread());
//			EnterCriticalSection(&emu_crit);
//			c = 0;
//			// Sleep(5);
//		}
//		if (emu.should_quit) {
//			quit = true;
//		}
//		LeaveCriticalSection(&emu_crit);
//	}
//
//cleanup:
//	SetEvent(emu_breakpoint_event);
//	return 0;
//}
