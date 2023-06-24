#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include "common.h"

namespace TheBoy {
	
	/*
	OPCODES exemple
	-------------
	|	INS reg | 	← Instruction mnemonic (Instruction type + operation mode)
	|	2  8	| 	← Not using (length in bytes + cpu cycles)
	|	Z N H C	| 	← Affected flags
	------------

	[0x00] - NOP (no operation)
		-> {INST_NOP, OPMODE_NONE} 
	[0x01] - LD BC,d16 (load 16bit from address to register BC)
		-> {INST_LD, OPMODE_R_V16, REG_BC}
	*/


	/**
	 * @brief Defined enumeration for the available instructions types
	 * Exemple, load, Jump, Add operations
	 * This is the first element of an instruction
	 */
	typedef enum {
		INST_NONE,
		INST_NOP,
		INST_LD,
		INST_DEC,
		INST_JP,
		INST_DI,
		INST_EI,
		INST_INC,
		INST_HALT,	// Halt until interrupt occurs (low power)
		INST_LDH,	// Load to high RAM
		INST_POP,	// Pop/push instructions are only for 16bit values
		INST_PUSH,
		INST_CALL,
		INST_RET, 	// Upon returning from CALL
		INST_RETI,	// Upon returning from interrupt
		INST_JR,	// Jump Relative
		INST_RST,	// Unconditional function call to the absolute fixed address defined by the opcode
		INST_ADD,
		INST_ADC, 	// Add with carry instruction
		INST_SUB,
		INST_SBC,	// Subtract with carry instruction
		INST_AND,	// Logic Operations instructions
		INST_XOR,
		INST_OR,
		INST_CP,
		INST_PRECB,
		INST_RLCA,	// Rotate Left Accumulator with carry flag
		INST_RRCA,	// Rotate Right Accumulator with carry flag
		INST_RLA,	// Rotate A left through carry
		INST_RRA,	// Rotate A right through carry
		INST_STOP,	// Low power standby mode (VERY low power)
		INST_DAA,	// Decimal adjust A
		INST_CPL,	// A = A xor FF
		INST_SCF,	// cy=1
		INST_CCF	// cy=cy xor 1
	} InstructType;


	/**
	 * @brief Holds enumeration for the availble registers
	 */
	typedef enum {
		REG_NONE,
		REG_A,
		REG_F,
		REG_B,
		REG_C,
		REG_D,
		REG_E,
		REG_H,
		REG_L,
		REG_AF,
		REG_BC,
		REG_DE,
		REG_HL,
		REG_SP,
		REG_PC
	} RegisterType;


	/**
	 * @brief Defines how the Instruction type should interact with memory
	 * EX {LD BC, D16} -> for this load operation a value with 16bit size is loaded to the register BC
	 * OPMODE_R_V16 -> _R (Registor) _V16 (16bit value)
	 */
	typedef enum {
		OPMODE_NONE,
		OPMODE_R,
		OPMODE_V16,
		OPMODE_V8,
		OPMODE_R_V16,
		OPMODE_R_V8,
		OPMODE_R_R,
		OPMODE_AR_R,
		OPMODE_R_AR,
		OPMODE_R_HLI,	// HLI operation over HL Register and increment
		OPMODE_R_HLD,	// HLD operation over HL Register and decrement
		OPMODE_HLI_R,	// HLI operation over register and increment
		OPMODE_HLD_R,	// HLD operation over register and decrement
		OPMODE_R_A8,
		OPMODE_R_A16,
		OPMODE_A8_R,
		OPMODE_HL_SPR,	// Unique operation {F8}, loads the stack pointer to hl and increment by r8
		OPMODE_A16_R,
		OPMODE_AR_V8,
		OPMODE_AR
	} OperationMode;


	/**
	 * @brief Defines the available conditions, some opertions have a flag validations
	 * EX {JP NZ,a16} -> for this jump operation not Z flag is checked
	 */
	typedef enum {
		CONDTYPE_NONE,
		CONDTYPE_NZ,
		CONDTYPE_Z,
		CONDTYPE_NC,
		CONDTYPE_C
	} ConditionType;


	/**
	 * @brief Defines the instruction logic structure
	 */
	typedef struct {
		InstructType insType;
		OperationMode opMode;
		RegisterType regTypeL;
		RegisterType regTypeR;
		ConditionType conType;
		bit8 param_val;
	} Instruc;


	/**
	 * @brief Get the By Opcode object
	 * @param opCd Opcode value for the instructions
	 * @return Instruc* Pointer to Defined instruction value
	 */
	Instruc* getByOpcode(bit8 opCd);

} // namespace TheBoy


#endif