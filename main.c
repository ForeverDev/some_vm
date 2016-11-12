#include <stdio.h>
#include <stdlib.h>
#include "vm.h"

int main(int argc, char** argv) {

	static uint8_t code[] = {
		/* DATA HEADER */
		0x0, 0x0, 0x0, 0x0,

		/* MOV A, 42 */
		0x01, 0x01, REG_A, 42, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		/* MOV B, A */
		0x01, 0x03, REG_B, REG_A,

		0x0
	};

	VMState_t* vm = vm_init();	
	vm_execute(vm, code);
	vm_dump_registers(vm);

	return 0;

}	
