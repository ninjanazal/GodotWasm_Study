#ifndef INSTRUC_FUNCS_H
#define INSTRUC_FUNCS_H

#include "instruction.h"

namespace TheBoy {
	class Cpu;

	namespace CpuFuncs {

	/**
	 * @brief Definition to the instruction function, this holds a pointer to a function
	 * that recieves a Cpu pointer, this function will resolve an instruction to the defined Cpu
	 */
	typedef void (*INST_FUNC)(Cpu* cpu);


	/**
	 * @brief Get the Instruct Process function to a given instruction type
	 * @param type Defined instruction type
	 * @return INST_FUNC Pointer to the instruction resolver
	 */
	INST_FUNC getInstructProcess(InstructType type);


	/**
	 * @brief Generic Jump instruction, this can be used on the JP instruction and the Call isntruction
	 * Since this is the same as JP but updates the Program Counter register
	 * @param cpu Pointer to the target Cpu object
	 * @param addre Address to be setted on the program Counter register
	 * @param pushPC Marks if should push the PC Registed to the stack pointer
	 */
	void jumpToAddress(Cpu* cpu, bit16 addr, bool pushPC);


	/**
	 * @brief Evaluates a flag condition check
	 * @param cpu Pointer to the target Cpu object
	 * @return true If the condition passes
	 * @return false If the condition fails
	 */
	bool validateCondition(Cpu* cpu);


	/**
	 * @brief Gets the Target registor from the prefix CB opCode
	 * @param cpOp Target OpCode value
	 * @return RegisterType corresponding RegisterType
	 */
	RegisterType getPREFCBRegistor(bit8 cpOp);

		
	} // namespace CpuFuncs	
} // namespace TheBoy


#endif