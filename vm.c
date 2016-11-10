#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "vm.h"

static void vm_die(VMState_t*, const char*, ...);
static int64_t vm_read_int(VMState_t*);
static double vm_read_float(VMState_t*);

static const VMModeSpecification_t mode_map[] = {
	{.mode_num = 0, .operands = {OPERAND_NOP}},									/* () */
	{.mode_num = 1, .operands = {OPERAND_INT8, OPERAND_64}},					/* (REG, IMM64) */
	{.mode_num = 2, .operands = {OPERAND_INT8, OPERAND_INT8}},					/* (REG, REG) */
	{.mode_num = 3, .operands = {OPERAND_INT8, OPERAND_INT8, OPERAND_INT64}},	/* (REG, [REG + INT64]) */
	{.mode_num = 4, .operands = {OPERAND_INT8, OPERAND_INT64, OPERAND_INT8}},	/* ([REG + IMM64], REG */
	{.mode_num = 5, .operands = {OPERAND_INT8, OPERAND_INT64, OPERAND_INT64}}	/* ([REG + IMM64], IMM64 */
};

static const VMInstruction_t instruction_map[] = {
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
	int64_t ret = *(int64_t *)vm->reg_int[REG_IP];
	vm->reg_int[REG_IP] += 8;
	return ret;
}

/* reads a 64 bit float from code memory */
static inline double
vm_read_float(VMState_t* vm) {
	double ret = *(double *)vm->reg_int[REG_IP];
	vm->reg_int[REG_IP] += 8;
	return ret;
}

VMState_t* vm_init() {
	VMState_t* vm = malloc(sizeof(VMState_t));
	if (!vm) {
		printf("couldn't allocate memory for VM\n");
	}	
	/* might as well allocate memory now... */
	vm->memory = malloc(SIZE_MEMORY);
}

void
vm_execute(VMState_t* vm, uint8_t* code) {
	
	/* headers in code memory */	
	uint32_t data_section_length = *(uint32_t *)&code[0];
	uint32_t data_size = *(uint32_t *)&code[4];
	
	/* first instruction at code[8] */
	vm->reg_int[REG_IP] = (int64_t)&code[8];

	vm->code = code;

}
