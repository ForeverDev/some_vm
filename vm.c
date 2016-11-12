#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "vm.h"

static void vm_die(VMState_t*, const char*, ...);
static int64_t vm_read_int(VMState_t*);
static double vm_read_float(VMState_t*);
static void vm_fetch_instruction(VMState_t*, VMInstruction_t**, uint8_t*);
static int64_t vm_dereference_int(VMState_t*, uint64_t);
static double vm_dereference_float(VMState_t*, uint64_t);

static void print_instruction(const VMInstruction_t*);

static const VMModeSpecification_t mode_map[] = {
	{.mode_num = 0, .operands = {OPERAND_NOP}},									/* () */
	{.mode_num = 1, .operands = {OPERAND_INT8, OPERAND_INT64}},					/* (REG, INT64) */
	{.mode_num = 2, .operands = {OPERAND_INT8, OPERAND_FLOAT64}},				/* (REG, FLOAT64) */
	{.mode_num = 3, .operands = {OPERAND_INT8, OPERAND_INT8}},					/* (REG, REG) */
	{.mode_num = 4, .operands = {OPERAND_INT8, OPERAND_INT8, OPERAND_INT64}},	/* (REG, [REG + INT64]) */
	{.mode_num = 5, .operands = {OPERAND_INT8, OPERAND_INT64, OPERAND_INT8}},	/* ([REG + INT64], REG */
	{.mode_num = 6, .operands = {OPERAND_INT8, OPERAND_INT64, OPERAND_INT64}},	/* ([REG + INT64], INT64 */
	{.mode_num = 7, .operands = {OPERAND_INT8, OPERAND_INT64, OPERAND_FLOAT64}} /* ([REG + INT64], FLOAT64 */
};

static VMInstruction_t instruction_map[] = {
	{.identifier = "NOP", .opcode = 0x00, .has_mode = 0},
	{.identifier = "MOV", .opcode = 0x01, .has_mode = 1}
};

static void
vm_die(VMState_t* vm, const char* format, ...) {
	va_list list;
	va_start(list, format);
	printf("\n\n**** VM DEATH ****\n");
	vprintf(format, list);
	printf("\n\n");
	va_end(list);	
	exit(1);
}

/* reads a 64 bit integer from code memory */
static inline int64_t
vm_read_int(VMState_t* vm) {
	int64_t ret = *(int64_t *)&vm->code[vm->reg_int[REG_IP]];
	vm->reg_int[REG_IP] += 8;
	return ret;
}

/* reads a 64 bit float from code memory */
static inline double
vm_read_float(VMState_t* vm) {
	double ret = *(double *)&vm->code[vm->reg_int[REG_IP]];
	vm->reg_int[REG_IP] += 8;
	return ret;
}

static inline uint8_t
vm_read_byte(VMState_t* vm) {
	return vm->code[vm->reg_int[REG_IP]++];
}

/* advances instruction accordingly and points out_ins and out_mode */
static void
vm_fetch_instruction(VMState_t* vm, VMInstruction_t** out_ins, uint8_t* out_mode) {
	*out_ins = &instruction_map[vm->code[vm->reg_int[REG_IP]++]]; 
	if ((*out_ins)->has_mode) {
		*out_mode = vm->code[vm->reg_int[REG_IP]++];
	}
}

static inline void
print_instruction(const VMInstruction_t* instruction) {
	printf("%s\n", instruction->identifier);
}

void
vm_dump_registers(VMState_t* vm) {

	static const char* vm_names[] = {"IP", "SP", "BP", "A", "B", "C", "D", "E"};

	printf("REGISTERS:\n");
	for (int i = 0; i < 8; i++) {
		printf("\tREG_%s:\t%lld\n", vm_names[i], vm->reg_int[i]);
	}
}

static inline int64_t
vm_deference_int(VMState_t* vm, uint64_t addr) {
	return *(int64_t *)&vm->memory[addr];
}

static inline double
vm_dereference_float(VMState_t* vm, uint64_t addr) {
	return *(double *)&vm->memory[addr];
}

VMState_t* 
vm_init() {
	VMState_t* vm = malloc(sizeof(VMState_t));
	if (!vm) {
		printf("couldn't allocate memory for VM\n");
	}	
	/* might as well allocate memory now... */
	vm->memory = malloc(SIZE_MEMORY);
	return vm;
}

void
vm_execute(VMState_t* vm, uint8_t* code) {
	
	/* headers in code memory */	
	uint32_t data_size = *(uint32_t *)&code[0];
	
	/* instruction data */
	uint8_t mode;
	VMInstruction_t* current_instruction;
	
	/* clear registers */
	memset(vm->reg_int, 0, sizeof(int64_t) * 8);
	memset(vm->reg_float, 0, sizeof(double) * 8);

	/* initialize ip */
	vm->reg_int[REG_IP] = 0;
	vm->code = &code[4];
	
	while (1) {
		/* TODO add mode handeling to fetch_instruction */
		vm_fetch_instruction(vm, &current_instruction, &mode);

		if (current_instruction->opcode == 0) {
			break;
		}

		/* now we know the mode, assign properly */
		uint8_t a8, b8, c8;
		int64_t a64, b64, c64;
		double af, bf, cf;
		switch (mode) {
			case 0x0: break;
			case 0x1:		
				a8 = vm_read_byte(vm);
				a64 = vm_read_int(vm);
				break;
			case 0x2:
				a8 = vm_read_byte(vm);
				af = vm_read_float(vm);
				break;
			case 0x3:
				a8 = vm_read_byte(vm);
				b8 = vm_read_byte(vm);
				break;
			case 0x4:
				a8 = vm_read_byte(vm);
				b8 = vm_read_byte(vm);
				a64 = vm_read_int(vm);
				break;
		}
		
		switch (current_instruction->opcode) {
			/* MOV */
			case 0x01: 
				switch (mode) {
					case 0x0: break;
					case 0x1: 
						vm->reg_int[a8] = a64;
						break;
					case 0x2:
						vm->reg_float[a8] = af;
						break;
					case 0x3:
						vm->reg_int[a8] = vm->reg_int[b8];
						break;
					case 0x4:
						vm->reg_int[a8] = dereference_int(vm, a64 + vm->reg_int[b8]);	
						break;	
				}
				break;
		}

	};

}
