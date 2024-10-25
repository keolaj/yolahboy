#include "yolahboy.h"

int main(int argc, char* argv[]) {

	Emulator emu;
	init_emulator(&emu, argv[1], argv[2]);

	while (true) {
		Operation to_execute = get_operation(emu.cpu, emu.memory);
		print_operation(to_execute);
		step_cpu(emu.cpu, emu.memory, to_execute);
		print_registers(emu.cpu);

	}

	return 0;
}