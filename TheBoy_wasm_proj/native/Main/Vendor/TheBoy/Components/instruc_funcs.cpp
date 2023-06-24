#include "instruc_funcs.h"
#include "cpu.h"

namespace TheBoy{
	namespace CpuFuncs {

		/**
		 * @brief On a Instruction None resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instNone(Cpu* cpu){
			std::cout << "[INSTFUNCS] ::: Not a valid Instructions" << std::endl;
		}


		/**
		 * @brief On a Instruction NOP resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instNOP(Cpu* cpu) {
#if VERBOSE
			std::cout << "[INSTFUNCS] ::: NOP Operation, continue" << std::endl;
#endif
		}

		/**
		 * @brief On a Instruction JP resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instJP(Cpu* cpu){
			jumpToAddress(cpu, cpu->getFetchedData(), false);
		}

		/**
		 * @brief On a Instruction JR resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instJR(Cpu* cpu){
			// Jump relative can be a increment/decrement for the current Value, and the bit8 definition is unsigned
			// casting to char
			// fetchdata holds a 2Byte value, and only 1B should be used
			char relative = (char)(cpu->getFetchedData() & 0xFF);
			bit16 addr = cpu->getRegisterValue(REG_PC) + relative;
			jumpToAddress(cpu, addr, false);
		}


		/**
		 * @brief On a Instruction CALL resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instCALL(Cpu* cpu){
			jumpToAddress(cpu, cpu->getFetchedData(), true);
		}


		/**
		 * @brief On a Instruction RET resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instRET(Cpu* cpu){
			// check if is a return with conditions
			if (cpu->getCurrInstruct()->conType != CONDTYPE_NONE) {
				cpu->requestCycles(1);
			}

			if(validateCondition(cpu)){
				bit16 lo = cpu->pop();
				cpu->requestCycles(1);

				bit16 hi = cpu->pop();
				cpu->requestCycles(1);

				bit16 val = (hi << 8) | lo;
				cpu->setRegisterValue(REG_PC, val);
				cpu->requestCycles(1);
			}
		}

		/**
		 * @brief On a Instruction RETI resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instRETI(Cpu* cpu){
			// Reactivate the master interrupt flag
			cpu->setInterruptMasterState(true);
			// Same as the default RET
			instRET(cpu);
		}


		/**
		 * @brief On a Instruction DI resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instDI(Cpu* cpu) {
			cpu->setInterruptMasterState(false);
		}


		/**
		 * @brief On a Instruction DI resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instEI(Cpu* cpu) {
			cpu->enablingIME();
		}


		/**
		 * @brief On a Instruction INC resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instINC(Cpu* cpu) {
			bit16 val = cpu->getFetchedData() + 0x1;

			// For 16bit registors
			if(cpu->getCurrInstruct()->regTypeL >= RegisterType::REG_AF) {
				cpu->requestCycles(1);
			}

			// For the specific INC (HL) Operation {34}
			if(cpu->getCurrInstruct()->regTypeL == REG_HL &&
					cpu->getCurrInstruct()->opMode == OPMODE_AR){
				val = cpu->requestBusRead(cpu->getRegisterValue(REG_HL)) + 0x1;
				val &= 0xFF;
				// Write the incremented value, write uses 8bit
				// Preventing overflow
				cpu->requestBusWrite(cpu->getRegisterValue(REG_HL), static_cast<bit8>(val));
			} else {
				cpu->setRegisterValue(cpu->getCurrInstruct()->regTypeL, val);
				// Redefine for check 
				val = cpu->getRegisterValue(cpu->getCurrInstruct()->regTypeL);
			}

			// Only INC opCode instructions with a 0x03 termination dont update any flags
			// EX {03, 13, 23 & 33}
			if((cpu->getCurrentOPCode() & 0x03) == 0x03) {
				return;
			} 
				// Flags check Z 0 H -
				cpu->setFlags(val == 0, 0, (val & 0x0F) == 0, -1);
		}


		/**
		 * @brief On a Instruction HALT resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instHALT(Cpu* cpu) {
			cpu->setHaltedValue(true);
		}



		/**
		 * @brief On a Instruction DEC resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instDEC(Cpu* cpu) {
			bit16 val = cpu->getRegisterValue(cpu->getCurrInstruct()->regTypeL) - 0x1;

			// For 16bit registors
			if(cpu->getCurrInstruct()->regTypeL >= RegisterType::REG_AF) {
				cpu->requestCycles(1);
			}

			// For the specific INC (HL) Operation {34}
			if(cpu->getCurrInstruct()->regTypeL == REG_HL &&
						cpu->getCurrInstruct()->opMode == OPMODE_AR){
				val = cpu->requestBusRead(cpu->getRegisterValue(REG_HL)) - 0x1;
				// Write the incremented value, write uses 8bit
				// Preventing overflow
				cpu->requestBusWrite(cpu->getRegisterValue(REG_HL), static_cast<bit8>(val));
			} else {
				cpu->setRegisterValue(cpu->getCurrInstruct()->regTypeL, val);
				// Redefine for check 
				val = cpu->getRegisterValue(cpu->getCurrInstruct()->regTypeL);
			}

			// Only INC opCode instructions with a 0x0B termination dont update any flags
			// EX {0B, 1B, 2B & 3B}
			if((cpu->getCurrentOPCode() & 0x0B) != 0x0B) {
				// Flags check Z 0 H -
				cpu->setFlags(val == 0, 1, (val & 0x0F) == 0x0F, -1);
			}
		}


		/**
		 * @brief On a Instruction Load resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instLD(Cpu* cpu){
			if(cpu->getDestenyIsMem()){
				// Form loads where the target is a memory location ex {0x02}
				// if is a 16bit value
				if(cpu->getCurrInstruct()->regTypeR >= RegisterType::REG_AF){
					cpu->requestCycles(1);
					cpu->requestBusWrite16(cpu->getMemoryDest(), cpu->getFetchedData());
				}
				else{
					// for 8bit writes
					cpu->requestBusWrite(cpu->getMemoryDest(), static_cast<bit8>(cpu->getFetchedData()));
				}

				cpu->requestCycles(1);
				return;
			}

			// For the Unique operation {F8}, loads the stack pointer to hl and increment by r8 
			if(cpu->getCurrInstruct()->opMode == OPMODE_HL_SPR){
				// Since this operation has the H & C flag set to flip on demand
				// Evaluates the Half Carry flag status, if adding the 
				bit8 hFlag = (cpu->getRegisterValue(cpu->getCurrInstruct()->regTypeR) & 0xF) +
						(cpu->getFetchedData() & 0xF) >= 0x10;
				// Evaluates the carry flag status
				bit8 cFlag = (cpu->getRegisterValue(cpu->getCurrInstruct()->regTypeR) & 0xFF) +
						(cpu->getFetchedData() & 0xFF) >= 0x100;

				cpu->setFlags(0, 0, hFlag, cFlag);
				// Defines the HL Registor to the value of the StackPointer Reg + the 8bit fetched data
				cpu->setRegisterValue(
					cpu->getCurrInstruct()->regTypeL,
					cpu->getRegisterValue(cpu->getCurrInstruct()->regTypeR) + (char)cpu->getFetchedData()
				);

				return;
			}
			cpu->setRegisterValue(cpu->getCurrInstruct()->regTypeL, cpu->getFetchedData());
		}


		/**
		 * @brief On a Instruction LDH resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instLDH(Cpu* cpu){
			// For the operation {F0}
			if(cpu->getCurrInstruct()->regTypeL == REG_A){
				// Loads from high ram, since this operation has a 8bit memory address on the 2nd operand,
				// and high Ram, a or opperation with 0xFF00 needs to be done to place correctly this pointer
				cpu->setRegisterValue( REG_A,
					cpu->requestBusRead(0xFF00 | cpu->getFetchedData())
				);
			}
			// For the operation {E0}
			else {
				// Loads to the highRam, the value on the defined register, since the fetch for this operation is a 8Bit
				// value and to be setted on the high RAM range, a 0xFF00 or operation is need 
				cpu->requestBusWrite(cpu->getMemoryDest(), 
					static_cast<bit8>(cpu->getRegisterValue(REG_A))
				);
			}
			cpu->requestCycles(1);
		}


		/**
		 * @brief On a Instruction POP resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instPOP(Cpu* cpu){
			bit16 l = cpu->pop();
			cpu->requestCycles(1);
			bit16 h = cpu->pop();
			cpu->requestCycles(1);

			bit16 val = (h << 8) | l;

			// For the Operation {F1}
			if(cpu->getCurrInstruct()->regTypeL == REG_AF) {
				/*
					POP AF completely replaces the F register
					value, so all flags are changed based on the 8-bit data that is read from memor
					This if is just a fail safe condition
				*/
				val &= 0xFFF0;
			}
			cpu->setRegisterValue(cpu->getCurrInstruct()->regTypeL, val);
		}


		/**
		 * @brief On a Instruction PUSH resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instPUSH(Cpu* cpu){
			// Keep in mind the order of write/read, left to right
			// push uses a 8bit value, soo 
			// EX REG_BC -> getCurrInst = bit16 BC
			// Writing (left to right) B needs to go first so BC >> 8 == 0x00(B) & 0xFF = (B)
			bit16 hi = (cpu->getRegisterValue(cpu->getCurrInstruct()->regTypeL) >> 8) & 0xFF;
			cpu->requestCycles(1);
			cpu->push(hi);

			bit16 lo = cpu->getRegisterValue(cpu->getCurrInstruct()->regTypeL) & 0xFF;
			cpu->requestCycles(1);
			cpu->push(lo);

			cpu->requestCycles(1);
		}


		/**
		 * @brief On a Instruction PUSH resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instRST(Cpu* cpu) {
			jumpToAddress(cpu, cpu->getCurrInstruct()->param_val, true);
		}

		/**
		 * @brief On a Instruction ADD resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instADD(Cpu* cpu) {
			bit32 val = cpu->getRegisterValue(cpu->getCurrInstruct()->regTypeL) + cpu->getFetchedData();

			// For 16bit add Instruction
			if(cpu->getCurrInstruct()->regTypeL >= RegisterType::REG_AF){
				cpu->requestCycles(1);
			}

			// For the spetial ADD instruction opCode {E8} where a value is added to the Stack pointer
			if(cpu->getCurrInstruct()->regTypeL == REG_SP) {
				// add a value to the SP for the current Value, and the bit8 ndefinition is unsigned
				// casting to char, can be a negative value
				val = cpu->getRegisterValue(REG_SP) + (char)(cpu->getFetchedData());
			}

			// Using 16bit value on val
			// Flags for a 8bit value
			int z = (val & 0xFF) == 0;
			int h = (cpu->getRegisterValue(cpu->getCurrInstruct()->regTypeL) & 0xF) + (cpu->getFetchedData() & 0xF) >= 0x10;
			// For overflow sanity check, casting to int
			int c = (int)(cpu->getRegisterValue(cpu->getCurrInstruct()->regTypeL) & 0xFF) + (int)(cpu->getFetchedData() & 0xFF) >= 0x100;

			// Flags For 16bit values OPCODES {09, 19, 29 & 39}
			if(cpu->getCurrInstruct()->regTypeL >= RegisterType::REG_AF) {
				z = -1;
				h = (cpu->getRegisterValue(cpu->getCurrInstruct()->regTypeL) & 0xFFF) + (cpu->getFetchedData() & 0xFFF) >= 0x1000; 

				// Using 32bit value to check 16bit overflow
				bit32 cVal = ((bit32)cpu->getRegisterValue(cpu->getCurrInstruct()->regTypeL)) + ((bit32)(cpu->getFetchedData()));
				c = cVal >= 0x10000;
			}

			// for the OPCode {E8}
			if(cpu->getCurrInstruct()->regTypeL == REG_SP) {
				z = 0;
				h = (cpu->getRegisterValue(cpu->getCurrInstruct()->regTypeL) & 0xF) + (cpu->getFetchedData() & 0xF) >= 0x10;

				// For overflow sanity check, casting to int
				c = (int)(cpu->getRegisterValue(cpu->getCurrInstruct()->regTypeL) & 0xFF) + (int)(cpu->getFetchedData() & 0xFF) >= 0x100;
			}

			cpu->setRegisterValue(cpu->getCurrInstruct()->regTypeL, val & 0xFFFF);
			cpu->setFlags(z, 0, h, c);

		}


		/**
		 * @brief On a Instruction ADC resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instADC(Cpu* cpu) {
			// Holds the initial register and c flag value
			bit16 RegA = cpu->getRegisterValue(REG_A);
			bit16 cFlag = GETBIT(cpu->getRegisterValue(REG_F), 4);

			bit16 calR = (RegA + cpu->getFetchedData() + static_cast<bit16>(GETBIT(cpu->getRegisterValue(REG_F), 4))) & 0xFF;
			// Add to the A registor the fetch data and the carry flag
			// and operation for a 8bit value
			cpu->setRegisterValue( REG_A, calR);

			cpu->setFlags(calR == 0, 0,
				((RegA & 0xF) + (cpu->getFetchedData() & 0xF) + static_cast<bit16>(GETBIT(cpu->getRegisterValue(REG_F), 4))) > 0xF,
				(RegA + cpu->getFetchedData() + static_cast<bit16>(GETBIT(cpu->getRegisterValue(REG_F), 4))) > 0xFF
			);
		}


		/**
		 * @brief On a Instruction SUB resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instSUB(Cpu* cpu) {
			bit16 value = cpu->getRegisterValue(cpu->getCurrInstruct()->regTypeL) - cpu->getFetchedData();

			// Since values can be negative, cast to int
			cpu->setFlags(
				value == 0, 1,
				((int)cpu->getRegisterValue(cpu->getCurrInstruct()->regTypeL) & 0xF) - ((int)cpu->getFetchedData() & 0xF) < 0,
				((int)cpu->getRegisterValue(cpu->getCurrInstruct()->regTypeL)) - ((int)cpu->getFetchedData()) < 0
			);
			cpu->setRegisterValue(cpu->getCurrInstruct()->regTypeL, value);
		}

		/**
		 * @brief On a Instruction SBC resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instSBC(Cpu* cpu) {
			int regFC = static_cast<int>(GETBIT(cpu->getRegisterValue(REG_F), 4));
			bit16 lInstRegVal = cpu->getRegisterValue(cpu->getCurrInstruct()->regTypeL);
			bit8 value = cpu->getFetchedData() + regFC;
			
			int zFlag = lInstRegVal - value == 0;
			
			// casting to int since values can be negative
			int hFlag = (static_cast<int>(lInstRegVal) & 0xF) -
				(static_cast<int>(cpu->getFetchedData()) & 0xF) - regFC < 0;

			int cFlag = static_cast<int>(lInstRegVal) -
				static_cast<int>(cpu->getFetchedData()) - regFC < 0;

			
			cpu->setRegisterValue(cpu->getCurrInstruct()->regTypeL, lInstRegVal - value);
			cpu->setFlags(zFlag, 1, hFlag, cFlag);
		}


		/**
		 * @brief On a Instruction AND resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instAND(Cpu* cpu) {
			cpu->setRegisterValue( REG_A,
				cpu->getRegisterValue(REG_A) & cpu->getFetchedData()
			);

			cpu->setFlags( cpu->getRegisterValue(REG_A) == 0,
				0, 1, 0
			);
		}


		/**
		 * @brief On a Instruction XOR resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instXOR(Cpu* cpu) {
			cpu->setRegisterValue(REG_A,
				cpu->getRegisterValue(REG_A) ^ cpu->getFetchedData() & 0xFF
			);

			cpu->setFlags( cpu->getRegisterValue(REG_A) == 0,
				0, 0, 0
			);
		}


		/**
		 * @brief On a Instruction OR resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instOR(Cpu* cpu) {
			cpu->setRegisterValue( REG_A,
				cpu->getRegisterValue(REG_A) | cpu->getFetchedData() & 0xFF
			);

			cpu->setFlags( cpu->getRegisterValue(REG_A) == 0,
				0, 0, 0
			);
		}


		/**
		 * @brief On a Instruction CP resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instCP(Cpu* cpu) {
			// compare A- REG (Since this can be signed or result on a negative val)
			int val = (int)cpu->getRegisterValue(REG_A) - (int)cpu->getFetchedData();
			cpu->setFlags( val == 0, 1,
				((int)cpu->getRegisterValue(REG_A) & 0x0F) - ((int)cpu->getFetchedData() & 0x0F) < 0,
				val < 0
			);
		}


		/**
		 * @brief Helper array for parsing the registor type on Prefix CP operations
		 */
		const RegisterType helperPCB[] = {
			REG_B, REG_C, REG_D, REG_E, REG_H, REG_L, REG_HL, REG_A
			
		};


		/// <summary>
		/// Gets the Target registor from the prefix CB opCode
		/// </summary>
		/// <param name="cpOp">Target OpCode value</param>
		/// <returns>corresponding RegisterType</returns>
		RegisterType getPREFCBRegistor(bit8 cpOp) {
			if (cpOp > 0b111) {
				return REG_NONE;
			}
			return helperPCB[cpOp];
		}

		/**
		 * @brief On a Instruction PREFIX CB resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instPRECB(Cpu* cpu) {
			bit8 OPPrf = cpu->getFetchedData();
			/*
				For Decoding this prefix CB table, EX.:
				{00}  || RLC B   ||
					  || Z 0 0 C ||
				RLC -> pInst_type
				B -> REGs
				For the registor from x0 to x6 == B to (HL)
								from x7 to xF == B to (HL)
				Soo the first 3 bits can indicate the registor type
			*/

			RegisterType rType = getPREFCBRegistor((OPPrf & 0b111));
		
			/*
				For decoding the bit value (if used) and the operation, EX.:
				{A5}  || RES 4,L ||
					  || - - - - ||
				RES -> pInst_type
				4 -> Bit value
				L -> REGs
				Looking for this OP binary value ob 1010 0101
								1 0 1 0  0 1 0 1
			                    |_| >    |___|-> Can be used as above, the registor
								 |	|____|
								 |	   |
								 |	   |____> Makes the current bit beeing use, since they go
								 |		 		From 0 to 7
								 |
								 ^
						Makes the current operation
			 */

			bit8 PBit = (OPPrf >> 3) & 0b111;
			bit8 POperation  = (OPPrf >> 6) & 0b11;

			// Using a 8bit value since all operations occour as  8bit value
			bit8 reg_value = cpu->getRegisterValueByte(rType);


			cpu->requestCycles(1);

			// For the operations with the HL Registed
			if(rType == REG_HL){
				cpu->requestCycles(2);
			}

			// For the BIT, RES and SET operations, OPCode 0xX~ (X >= 4) -> POperations 01 and bigger
			switch (POperation) {
			case 0b01: {
				// BIT OPERATION
				// Operation evaluates if the defined bit  is set on the registor and set it on the
				// Z Flag, if this bit is set, the z value is 0 else is 1
				cpu->setFlags(!(reg_value & (1 << PBit)), 0, 1, -1);
				return;
			}
			case 0b10:
				// RES OPERATION
				// This operation resets the defined bit
				// current registor value and the defined notBit 
				reg_value &= ~(1 << PBit);
				cpu->setRegisterValueByte(rType, reg_value);
				return;

			case 0b11:
				// SET OPERATION
				// This operation sets the defined bit value
				reg_value |= (1 << PBit);
				cpu->setRegisterValueByte(rType, reg_value);
				return;
			}

			// for OPCodes less than {0x40}, those use the C flag value
			bit8 cFl = GETBIT(cpu->getRegisterValue(REG_F), 4);

			/*
				To evaluate what istruction is beeing used the PBit value will be used
			*/
			switch (PBit) {
				case 0b000: {
					// RLC OPERATION
					// Rotate Left operation, shift the value 1 to left and set the old 1st bit to the end
					bit8 rRes = (reg_value << 1) & 0xFF;
					bool setC = false;
					if((reg_value & (1 << 7)) != 0){
						rRes |= 1;
						setC = true;
					}
					cpu->setRegisterValueByte(rType, rRes);

					// C flag is set if a carry value existed
					cpu->setFlags( rRes == 0, 0, 0, setC);
					return;
				}

				case 0b001: {
					// RRC OPERATION
					// Rotate Right operation, shift the value 1 bit to right and set the old last bit to the begining
					bit8 rRes = reg_value;
					reg_value >>= 1;
					reg_value |= (rRes << 7);

					cpu->setRegisterValueByte(rType, reg_value);
					cpu->setFlags(reg_value == 0, 0, 0, rRes& 0x1);
					return;
				}

				case 0b010: {
					// RL OPERATION
					// Rotate Left using the current carry lflag value
					bit8 rRes = reg_value;
					reg_value <<= 1;
					reg_value |= cFl;

					cpu->setRegisterValueByte(rType, reg_value);
					cpu->setFlags(reg_value == 0x0, 0, 0, static_cast<bool>(rRes & 0x80));
					return;
				}

				case 0b011: {
					// RR OPERATION
					// Rotate right using the current carry lflag value
					bit8 rRes = reg_value;
					reg_value >>= 1;

					reg_value |= (cFl << 7);

					cpu->setRegisterValueByte(rType, reg_value);
					cpu->setFlags(reg_value == 0, 0, 0, rRes & 0b1);
					return;
				}

				case 0b100 : {
					// SLA OPERATION
					// Shift left arithmetic
					bit8 rRes = reg_value;
					reg_value <<= 1;

					cpu->setRegisterValueByte(rType, reg_value);
					cpu->setFlags(reg_value == 0, 0, 0, static_cast<bool>(rRes & 0x80));
					return;
				}

				case 0b101: {
					// SRA OPERATION
					// Shift right arithmetic, shift right arithmetic (b7=b7)
					// The most significant bit stays the same
					bit8 rRes = static_cast<int8_t>(reg_value) >> 1;

					cpu->setRegisterValueByte(rType, rRes);
					cpu->setFlags(rRes == 0, 0, 0, static_cast<bool>(reg_value & 0b1));
					return;
				}

				case 0b110: {
					// SWAP OPERATION
					// Swaps the low nibble to the high nibble for the defined value
					reg_value = ((reg_value & 0xF0) >> 4) | ((reg_value & 0xF) << 4);

					cpu->setRegisterValueByte(rType, reg_value);
					cpu->setFlags(reg_value == 0, 0, 0, 0);
					return;
				}

				case 0b111: {
					// SRL OPERATION
					// shift right logical (b7=0)
					bit8 rRes = reg_value >> 1;

					cpu->setRegisterValueByte(rType, rRes);
					cpu->setFlags(rRes == 0, 0, 0, reg_value & 0b1);
					return;
				}
					
			}
		}


		/**
		 * @brief On a Instruction RLCA resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instRLCA(Cpu* cpu) {
			bit8 regA = cpu->getRegisterValue(REG_A);
			bool cFlag = (regA >> 7) & 0x1;
			regA = (regA << 1) | static_cast<bit8>(cFlag);
			cpu->setRegisterValue(REG_A, regA);

			cpu->setFlags(0, 0, 0, cFlag);
		}


		/**
		 * @brief On a Instruction RRCA resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instRRCA(Cpu* cpu) {
			bit16 regAVal = cpu->getRegisterValue(REG_A);
			bit8 cFlag = regAVal & 0x1;
			regAVal >>= 1;
			regAVal |= (cFlag << 7);

			cpu->setRegisterValue(REG_A, regAVal);
			cpu->setFlags(0, 0, 0, cFlag);
		}


		/**
		 * @brief On a Instruction RLA resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instRLA(Cpu* cpu) {
			bit8 cFlag = (cpu->getRegisterValue(REG_A) >> 7) & 0x1;
		
			cpu->setRegisterValue(REG_A, 
				(cpu->getRegisterValue(REG_A) << 1) | GETBIT(cpu->getRegisterValue(REG_F), 4)
			);
			cpu->setFlags(0, 0, 0, cFlag);
		}


		/**
		 * @brief On a Instruction RRA resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instRRA(Cpu* cpu) {
			bit8 cFlagVal = GETBIT(cpu->getRegisterValue(REG_F), 4);
			bit8 regAVal = cpu->getRegisterValue(REG_A);
			bit8 cFlag = regAVal & 0x1;

			regAVal >>= 1;
			regAVal |= (cFlagVal << 7);

			cpu->setRegisterValue(REG_A, regAVal);
			cpu->setFlags(0, 0, 0, cFlag);
		}


		/**
		 * @brief On a Instruction STOP resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instSTOP(Cpu* cpu) {
			std::cout << "[INSTRESOLVER] ::: STOP operation not defined!" << std::endl;
		}


		/**
		 * @brief On a Instruction DAA resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instDAA(Cpu* cpu) {
			/*
				The DAA instruction adjusts the results of a binary addition or subtraction 
				(as stored in the accumulator and flags) to retroactively turn it into a BCD addition or subtraction.
				It does so by adding or subtracting 6 from the result's upper nybble, lower nybble, or both.
				In order to work it has to know whether the last operation was an addition or a subtraction (the n flag),
				and whether a carry and/or a half-carry occurred (the c and h flags).
				Incidentally, the DAA instruction is the only thing that the n and h flags are normally ever used by,
				unless a program pushes the flags onto the stack and pops them into another register to explicitly inspect them.
			*/
			bit8 val = 0x0;
			bit8 cFlag = 0;
			bit8 aReg = cpu->getRegisterValueByte(REG_A);
			bit8 fReg = cpu->getRegisterValueByte(REG_F);

			if (GETBIT(fReg, 5) || (~GETBIT(fReg, 6) && (aReg & 0xF) > 9) ) {
				val = 0x6;
			}

			if (GETBIT(fReg, 4) || (~GETBIT(fReg, 6) && aReg > 0x99) ) {
				val |= 0x60;
				cFlag = 1;
			}

			aReg += (GETBIT(fReg, 6) ? -val : val);
			cpu->setRegisterValue(REG_A, aReg);
			cpu->setFlags(cpu->getRegisterValue(REG_A) == 0, -1, 0, cFlag);
		}


		/**
		 * @brief On a Instruction CPL resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instCPL(Cpu* cpu) {
			// Bit inversion
			cpu->setRegisterValue(REG_A, ~cpu->getRegisterValue(REG_A));
			cpu->setFlags(-1, 1, 1, -1);
		}

		/**
		 * @brief On a Instruction SCF resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instSCF(Cpu* cpu) {
			cpu->setFlags(-1, 0, 0, 1);
		}

		/**
		 * @brief On a Instruction CFF resolver
		 * @param cpu Requester cpu pointer
		 */
		static void instCCF(Cpu* cpu) {
			cpu->setFlags(-1, 0, 0, GETBIT(cpu->getRegisterValue(REG_F), 4) ^ 0x1);
		}

		/**
		 * @brief Evaluates a flag condition check
		 * @param cpu Pointer to the target Cpu object
		 * @return true If the condition passes
		 * @return false If the condition fails
		 */
		bool validateCondition(Cpu* cpu){
			bool z_val = cpu->getZFlag();
			bool c_val = cpu->getCFlag();

			switch (cpu->getCurrInstruct()->conType) {
				case CONDTYPE_NONE: return true;
				case CONDTYPE_C: return c_val;
				case CONDTYPE_NC: return !c_val;
				case CONDTYPE_Z: return z_val;
				case CONDTYPE_NZ: return !z_val;
			}
			return false;
		}

		/**
		 * @brief Generic Jump instruction, this can be used on the JP instruction and the Call isntruction
		 * Since this is the same as JP but updates the Program Counter register
		 * @param cpu Pointer to the target Cpu object
		 * @param addre Address to be setted on the program Counter register
		 * @param pushPC Marks if should push the PC Registed to the stack pointer
		 */
		void jumpToAddress(Cpu* cpu, bit16 addr, bool pushPC) {
			if(validateCondition(cpu)){
				if(pushPC) {
					// 2 cycles for a bit16 push
					cpu->requestCycles(2);
					cpu->push16(cpu->getRegisterValue(REG_PC));
				}
				cpu->setRegisterValue(REG_PC, addr);
				cpu->requestCycles(1);
			}
		}

		/**
		 * @brief Defines Instruction type to the resolvers
		 */
		static INST_FUNC instructResolvers[] = {
			/*INST_NONE*/	instNone,
			/*INST_NOP*/ 	instNOP,
			/*INST_LD*/		instLD,
			/*INST_DEC*/ 	instDEC,
			/*INST_JP*/		instJP,
			/*INST_DI*/ 	instDI,
			/*INST_EI*/ 	instEI,
			/*INST_INC*/ 	instINC,
			/*INST_HALT*/ 	instHALT,
			/*INST_LDH*/ 	instLDH,
			/*INST_POP*/ 	instPOP,
			/*INST_PUSH*/ 	instPUSH,
			/*INST_CALL*/ 	instCALL,
			/*INST_RET*/ 	instRET,
			/*INST_RETI*/ 	instRETI,
			/*INST_JR*/ 	instJR,
			/*INST_RST*/ 	instRST,
			/*INST_ADD*/ 	instADD,
			/*INST_ADC*/ 	instADC,
			/*INST_SUB*/ 	instSUB,
			/*INST_SBC*/ 	instSBC,
			/*INST_AND*/ 	instAND,
			/*INST_XOR*/ 	instXOR,
			/*INST_OR*/ 	instOR,
			/*INST_CP*/ 	instCP,
			/*INST_PRECB*/ 	instPRECB,
			/*INST_RLCA*/ 	instRLCA,
			/*INST_RRCA*/ 	instRRCA,
			/*INST_RLA*/ 	instRLA,
			/*INST_RRA*/ 	instRRA,
			/*INST_STOP*/ 	instSTOP,
			/*INST_DAA*/ 	instDAA,
			/*INST_CPL*/ 	instCPL,
			/*INST_SCF*/ 	instSCF,
			/*INST_CCF*/ 	instCCF
		};

		/**
		 * @brief Get the Instruct Process function to a given instruction type
		 * @param type Defined instruction type
		 * @return INST_FUNC Pointer to the instruction resolver
		 */
		INST_FUNC getInstructProcess(InstructType type){
			return instructResolvers[type];
		}

	} // namespace CpuFuncs
} // namespace TheBoy
