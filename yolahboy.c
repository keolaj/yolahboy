#include <Windows.h>
#include "yolahboy.h"
#include "emulator_main.h"
#include "components/emulator.h"

Emulator emu;
LPHANDLE emu_breakpoint_event;
CRITICAL_SECTION emu_crit;

ATOM Init_App_Window_class(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	//
	//HANDLE memReadPipe, memWritePipe;
	//SECURITY_ATTRIBUTES mem_pipe_atts = {
	//	sizeof(SECURITY_ATTRIBUTES), NULL, true
	//};
	//if (CreatePipe(memReadPipe, memWritePipe, &mem_pipe_atts, sizeof(Memory*)) == false) {
	//	printf("could not create memory pipe");
	//	return -1;
	//}

	MSG m;
	Init_App_Window_class(hInstance);
	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	InitializeCriticalSection(&emu_crit);
	emu_breakpoint_event = CreateEventExA(NULL, TEXT("INITIALIZED EMULATOR"), 0, EVENT_MODIFY_STATE | SYNCHRONIZE);

	args* rom_args = (args*)malloc(sizeof(args));
	if (rom_args == NULL) {
		printf("could not init rom args");
		return -1;
	}

	char* rom_files[] = { "", "D:\\Downloads\\bootix_dmg.bin", "D:\\Downloads\\tetris\\tetris.gb" };

	rom_args->argc = 2;
	rom_args->argv = rom_files;
	rom_args->breakpoint_arr = (u16*)malloc(sizeof(u16) * MAX_BREAKPOINTS);
	if (rom_args->breakpoint_arr == NULL) {
		printf("Couldn't allocate breakpoint array");
		return -1;
	}
	memset(rom_args->breakpoint_arr, 0, sizeof(u16) * MAX_BREAKPOINTS);
	rom_args->breakpoint_arr[0] = 0x100;


	SECURITY_ATTRIBUTES emu_thread_atts = {
		sizeof(SECURITY_ATTRIBUTES), NULL, false
	};

	SECURITY_ATTRIBUTES event_atts = {
		sizeof(SECURITY_ATTRIBUTES), NULL, false
	};

	HANDLE emulator_thread = CreateThread(&emu_thread_atts, 0, run_emulator, rom_args, 0, NULL);
	if (emulator_thread == NULL) {
		printf("could not start emulator thread");
		return -1;
	}


	bool quit = false;
	while (!quit) {


		switch (WaitForSingleObject(emu_breakpoint_event, 1)) {
		case WAIT_OBJECT_0:
			EnterCriticalSection(&emu_crit);
			if (emu.cpu == NULL) {
				quit = true;
				continue;
			}
			print_registers(emu.cpu);
			// emu.should_quit = true;
			// quit = true;
			LeaveCriticalSection(&emu_crit);
			ResumeThread(emulator_thread);
			break;
		case WAIT_TIMEOUT:
			break;
		}
	}
end:

	WaitForSingleObject(emulator_thread, INFINITE);

	DWORD emu_exit_code;

	GetExitCodeThread(emulator_thread, &emu_exit_code);
	CloseHandle(emulator_thread);

	free(rom_args);

	return emu_exit_code;
}

ATOM Init_App_Window_class(HINSTANCE hInstance) {
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WinProc;                     //Points to the Window Procedure" LRESULT CALLBACK WinProc (,,,,);"
	wc.cbClsExtra = 0;                                    //No Extra Class Memory
	wc.cbWndExtra = 0;                                    //No Extra Window Memory
	wc.hInstance = hInstance;                            //Handle to the instance
	wc.hIcon = NULL;                                 //No Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);           //Predefined Arrow
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //Back ground Color
	wc.lpszMenuName = NULL;                                 //No Menu
	wc.lpszClassName = "YolahBoy Debugger";                             //Name of Window Class
	wc.hIconSm = NULL;

	return RegisterClassEx(&wc);

}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	HWND hWnd;					// Handle for the window

	hWnd = CreateWindow("YolahBoy Debugger",            // Window Class Name
		"YolahBoy Debugger",            // Title Bar
		WS_OVERLAPPEDWINDOW, // Window style
		CW_USEDEFAULT,       // x position of Window
		CW_USEDEFAULT,       // y position of Window
		500,                 // Width of the Window
		400,                 // Height of the Window
		NULL,                // Parent Window
		NULL,                // Menu
		hInstance,           // Application Instance
		NULL);              // Window parameters

	if (!hWnd)          //Was there an error in creating a window ? ie hWnd = 0;           
		return FALSE;

	ShowWindow(hWnd, nCmdShow);  //show the Window 
	UpdateWindow(hWnd);         //update the window inresponse to events
	return TRUE;
}

LRESULT CALLBACK WinProc(HWND hWnd,           //Window handle 
	UINT message,        //messages from Windows like WM_DESTROY,WM_PAINT
	WPARAM wParam,       //
	LPARAM lParam)
{
	switch (message)
	{

	case WM_DESTROY: PostQuitMessage(0);
		break;

	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
