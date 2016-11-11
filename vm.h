#ifndef VM_H
#define VM_H

#include <stdint.h>

#define SIZE_STACK	0x1000
#define SIZE_MEMORY 0x10000 /* includes SIZE_STACK and data section */

#define NUM_REG_INT		8 /* includes IP, SP, BP */
#define NUM_REG_FLOAT	8

#define REG_IP 0
#define REG_SP 1
#define REG_BP 2
#define REG_A  3
#define REG_B  4
#define REG_C  5
#define REG_D  6
#define REG_E  7

/*
 * memory mapping:
 *	 [0, X]							data section
 *	 [X+1, X+SIZE_STACK+1]			stack
 *	 [X+SIZE_STACK+2, SIZE_MEMORY]	heap
 *
 * code mapping:
 *   uint32_t	total_bytes_data
 *   uint8_t[]	code
 *
 * instruction format:
 *	 byte 0: opcode
 *	 byte 1: mode (optional ?)
 *	 byte 2+: operands (optional)
*/   

typedef struct VMModeSpecification {
	uint8_t mode_num;
	enum {
		OPERAND_NOP = 0,
		OPERAND_INT8 = 1,
		OPERAND_INT16 = 2,
		OPERAND_INT32 = 3,
		OPERAND_INT64 = 4,
		OPERAND_FLOAT32 = 5,
		OPERAND_FLOAT64 = 6,
		OPERAND_32 = 7, /* either int or float */
		OPERAND_64 = 8  /* either int or float */
	} operands[4];
} VMModeSpecification_t;

typedef struct VMInstruction {
	const char* identifier; /* might be useful for debugging */
	uint8_t	opcode;
	uint8_t has_mode;
} VMInstruction_t;

typedef struct VMState {
	uint8_t* code;
	uint8_t* memory;
	int64_t  reg_int[NUM_REG_INT];
	double	 reg_float[NUM_REG_FLOAT];
} VMState_t;

VMState_t* vm_init();
void vm_execute(VMState_t*, uint8_t*);

#endif
