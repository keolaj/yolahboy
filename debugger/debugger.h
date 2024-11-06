#pragma once
#include <Windows.h>

typedef struct {
	// HANDLE mem_pipe_handle;
	int argc;
	char** argv;
	int* breakpoint_arr;

} args;

int debugger_run(HANDLE emulator_thread, args* t_args);