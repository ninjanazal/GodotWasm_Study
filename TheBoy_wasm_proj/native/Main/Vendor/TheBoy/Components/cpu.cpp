#include "cpu.h"
#include "emulatorController.h"

namespace TheBoy {
	/**
	 * @brief Construct a new Cpu object
	 */
	Cpu::Cpu(EmulatorController* ctrl) : emuCtrl(ctrl) {
		regs = std::make_shared<Registers>();
		reset();
		startTime = std::chrono::high_resolution_clock::now();
		std::cout << "[CPU] ::: Cpu has been created!" << std::endl;
	}


	/**
	 * @brief Resets the current cpu state
	 */
	void Cpu::reset() {
		*((bit16*)&regs->A) = 0xB001;
		*((bit16*)&regs->B) = 0x1300;
		*((bit16*)&regs->D) = 0xD800;
		*((bit16*)&regs->H) = 0x4D01;

		regs->PC = 0x100;
		regs->SP = 0xFFFE;

		cpuHLT = false;

		interruptEnable = 0x0;
		interruptFlags = 0x0;
		interruptMasterState = false;
		enablingIntMaster = false;

		currInstruct = nullptr;
		currOpcode = 0x0;

		emuCtrl->getTimer()->setRegisterDIV(0xABCC);

		std::cout << "[CPU] ::: Reseted" << std::endl;
	}


	/**
	 * @brief Defines the cpu iteration
	 */
	void Cpu::step() {
		if (!cpuHLT) {
#if VERBOSE
			bit16 tempPc = regs->PC;
#endif

			fetch_inst();
			requestCycles(1);
			fetch_data();

#if VERBOSE
			printf(
				"[CPU] ::: [%08lX] Regs State { A: %2.2X F: %2.2X BC: %2.2X %2.2X DE: %2.2X %2.2X HL: %2.2X %2.2X SP: %4.4X PC %4.4X }\n",
				static_cast<unsigned long>(emuCtrl->getTicks()), regs->A, regs->F, regs->B, regs->C, regs->D, regs->E, regs->H, regs->L, regs->SP, regs->PC
			);
			printf("[CPU] ::: ->  OPCODE: %2.2X | PC: %2.2X\n", currOpcode, tempPc);
			fflush(stdout);
#endif

			//emuCtrl->getView()->setRegistorsVals(regs.get());
			executeInst();
		}
		else {
			// During an halted state

			requestCycles(1);

			if (interruptFlags) {
				cpuHLT = false;
			}

		}

		if (interruptMasterState) {
			InterruptFuncs::handle_interrupt(this);
			enablingIntMaster = false;
		}

		if (enablingIntMaster) {
			interruptMasterState = true;
		}
	}


	/**
	 * @brief Get the Register Value object
	 * @param regType Defined register to get
	 * @return bit16 Data on the setted register
	 */
	bit16 Cpu::getRegisterValue(RegisterType regType) {
		switch (regType) {
		case REG_A: return regs->A;
		case REG_F: return regs->F;
		case REG_B: return regs->B;
		case REG_C: return regs->C;
		case REG_D: return regs->D;
		case REG_E: return regs->E;
		case REG_H: return regs->H;
		case REG_L: return regs->L;

		case REG_SP: return regs->SP;
		case REG_PC: return regs->PC;

		case REG_AF: return TheBoy::reverse16(*((bit16*)&regs->A));
		case REG_BC: return TheBoy::reverse16(*((bit16*)&regs->B));
		case REG_DE: return TheBoy::reverse16(*((bit16*)&regs->D));
		case REG_HL: return TheBoy::reverse16(*((bit16*)&regs->H));

		default: return 0x0;
		}
	}


	/**
	 * @brief Set the Register Value
	 * @param type Registor type to be set
	 * @param value Value to be set
	 */
	void Cpu::setRegisterValue(RegisterType regType, bit16 value) {
		switch (regType) {
		case REG_A: regs->A = value & 0xFF;	return;
		case REG_F: regs->F = value & 0xFF; return;
		case REG_B: regs->B = value & 0xFF; return;
		case REG_C: regs->C = value & 0xFF; return;
		case REG_D: regs->D = value & 0xFF; return;
		case REG_E: regs->E = value & 0xFF; return;
		case REG_H: regs->H = value & 0xFF; return;
		case REG_L: regs->L = value & 0xFF; return;

		case REG_SP: regs->SP = value; return;
		case REG_PC: regs->PC = value; return;

		case REG_AF: *((bit16*)&regs->A) = TheBoy::reverse16(value); return;
		case REG_BC: *((bit16*)&regs->B) = TheBoy::reverse16(value); return;
		case REG_DE: *((bit16*)&regs->D) = TheBoy::reverse16(value); return;
		case REG_HL: *((bit16*)&regs->H) = TheBoy::reverse16(value); return;
		}
	}


	/**
	 * @brief Get the Register Value Byte object
	 * @param regType Defined register to get
	 * @return bit8 Data on the setted register
	 */
	bit8 Cpu::getRegisterValueByte(RegisterType regType) {
		switch (regType) {
		case REG_A: return regs->A;
		case REG_F: return regs->F;
		case REG_B: return regs->B;
		case REG_C: return regs->C;
		case REG_D: return regs->D;
		case REG_E: return regs->E;
		case REG_H: return regs->H;
		case REG_L: return regs->L;
		case REG_HL: return requestBusRead(getRegisterValue(REG_HL));
		}
		return 0x0;
	}


	/**
	 * @brief Set the Register Value Byte object
	 * @param regType Registor type to be set
	 * @param value Value to be set
	 */
	void Cpu::setRegisterValueByte(RegisterType regType, bit8 value) {
		switch (regType) {
		case REG_A: regs->A = value & 0xFF; break;
		case REG_F: regs->F = value & 0xFF; break;
		case REG_B: regs->B = value & 0xFF; break;
		case REG_C: regs->C = value & 0xFF; break;
		case REG_D: regs->D = value & 0xFF; break;
		case REG_E: regs->E = value & 0xFF; break;
		case REG_H: regs->H = value & 0xFF; break;
		case REG_L: regs->L = value & 0xFF; break;
		case REG_HL:
			requestBusWrite(getRegisterValue(REG_HL), value);
			break;
		}
	}


	/**
	 * @brief Get the Interrupt value
	 * @return true/False Current interrupt state
	 */
	bool Cpu::getMasterInterrupt() {
		return interruptMasterState;
	}

	/**
	 * @brief Set the Interrupt State value
	 * @param iState New interrupt state value
	 */
	void Cpu::setInterruptMasterState(bool iState) {
		interruptMasterState = iState;
	}


	/**
	 * @brief Sets the enablingIntMaster to enable the interrup master
	 */
	void Cpu::enablingIME() {
		enablingIntMaster = true;
	}


	/**
	 * @brief Gets the current Z flag value
	 * @return bit8 Current z value
	 */
	bit8 Cpu::getZFlag() {
		return (GETBIT(regs->F, 7));
	}


	/**
	 * @brief Gets the current C flag value
	 * @return bit8 Current C value
	 */
	bit8 Cpu::getCFlag() {
		return (GETBIT(regs->F, 4));
	}


	/**
	 * @brief Get the Current OP Code value
	 * @return bit8 Current OP Code value
	 */
	bit8 Cpu::getCurrentOPCode() {
		return currOpcode;
	}


	/**
	 * @brief Set the Flags values
	 * @param z Zero flag
	 * @param n Subtraction flag (null flag)
	 * @param h Half Carry flag
	 * @param c Carry flag
	 */
	void Cpu::setFlags(bit8 z, bit8 n, bit8 h, bit8 c) {
		using namespace CpuFuncs;
		if (z != 0xFF) { SETBIT(regs->F, 7, z); }
		if (n != 0xFF) { SETBIT(regs->F, 6, n); }
		if (h != 0xFF) { SETBIT(regs->F, 5, h); }
		if (c != 0xFF) { SETBIT(regs->F, 4, c); }
	}


	/**
	 * @brief Get the Curr Instruct object
	 * @return Instruc* Pointer to the current instruction
	 */
	Instruc* Cpu::getCurrInstruct() {
		return currInstruct;
	}


	/**
	 * @brief Get the Fetched Data value
	 * @return bit16 Fetched data value
	 */
	bit16 Cpu::getFetchedData() {
		return intMem.fetchData;
	}


	/**
	 * @brief Gets if the internal memory marks that fetched is memoryLoc
	 * @return true/false Is memory location
	 */
	bool Cpu::getDestenyIsMem() {
		return intMem.destIsMem;
	}

	/**
	 * @brief Get the Memory Dest internal memory value
	 * @return bit16 Memory destination
	 */
	bit16 Cpu::getMemoryDest() {
		return intMem.memDest;
	}


	/**
	 * @brief Updates the cpu program counter to the current fetched data
	 */
	void Cpu::updatePCtoFetched() {
		regs->PC = intMem.fetchData;
	}


	/**
	 * @brief Requests the emulator controller to emulate a defined cycle value
	 * @param cycles Cycle value
	 */
	void Cpu::requestCycles(const int& cycles) {
		emuCtrl->emulCycles(cycles);
	}


	/**
	 * @brief Writes to a defined address
	 * @param addr Target address to be written
	 * @param val Value to be setted on the address
	 */
	void Cpu::requestBusWrite(bit16 addr, bit8 val) {
		emuCtrl->getBus()->abWrite(addr, val);
	}


	/**
	 * @brief Requests Writes to a defined address
	 * @param addr Target address to be written
	 * @param val 16bit Value to be setted on the address
	 */
	void Cpu::requestBusWrite16(bit16 addr, bit16 val) {
		emuCtrl->getBus()->abWrite16(addr, val);
	}


	/**
	 * @brief Request Read to a defined addres
	 * @param addr Target read address
	 * @return bit8 Readed value
	 */
	bit8 Cpu::requestBusRead(bit16 addr) {
		return emuCtrl->getBus()->abRead(addr);
	}


	/**
	 * @brief Get the Cpu IE Register value
	 * @return bit8 Current IE register value
	 */
	bit8 Cpu::getCpuIERegister() {
		return interruptEnable;
	}

	/**
	 * @brief Set the Cpu IE Register value
	 * @param val New Ie register value
	 */
	void Cpu::setCpuIERegister(bit8 val) {
		interruptEnable = val;
	}



	/**
	 * @brief Push a 8bit data to the next stack pointer location, decrement the sp val
	 * @param data Data to be pushed
	 */
	void Cpu::push(bit8 data) {
		regs->SP--;
		requestBusWrite(regs->SP, data);
	}

	/**
	 * @brief Push a 16bit data to the next stack pointer location, decrement the sp val
	 * @param data Data to be pushed
	 */
	void Cpu::push16(bit16 data) {
		// To push a 16bit value, use 2 sets of 8bit
		/*
			EX: (01010111) >> 8 = (00000101) & 1111 = 0101 low
			(01010111) & 1111 = 0111 high
		*/
		push((data >> 8) & 0xFF);
		push(data & 0xFF);
	}

	/**
	 * @brief Pop a 8bit data from the current stack pointer location, increment sp val
	 * @return bit8 Current stack pointer data location
	 */
	bit8 Cpu::pop() {
		// Gets the current data from the bus on the current Stack pointer addres, and increment
		return requestBusRead(regs->SP++);
	}


	/**
	 * @brief Pop a 16bit data from the current stack pointer location, increment sp val
	 * @return bit8 Current stack pointer data location
	 */
	bit16 Cpu::pop16() {
		bit8 l = pop();
		bit8 h = pop();

		return (h << 8) | l;
	}


	/**
	 * @brief Set the Halted Value
	 * @param value New halted value
	 */
	void Cpu::setHaltedValue(bool value) {
		cpuHLT = value;
	}


	/**
	 * @brief Get the Halted State value
	 * @return true/false Current halted value
	 */
	bool Cpu::getHaltedState() {
		return cpuHLT;
	}


	/**
	 * @brief Get the Interr Flags Vvalue
	 * @return bit8 Flags value
	 */
	bit8 Cpu::getInterrFlags() {
		return interruptFlags;
	}


	/**
	 * @brief Set the Interr Flags value
	 * @param flags New Flags value
	 */
	void Cpu::setInterrFlags(bit8 flags) {
		interruptFlags = flags;
	}


	/**
	 * @brief Requests a interrupt by type on the current Cpu
	 * @param iType Iterrupt type
	 */
	void Cpu::requestInterrupt(InterruptFuncs::InterruptType iType) {
		interruptFlags |= iType;
	}


	/// <summary>
	/// Gets the cpu summary string, used to print
	/// </summary>
	/// <param name="cpuStr">Pointer to target Registor summary string</param>
	/// <param name="opCodeStr">Pointer to target opCode summary string</param>
	void Cpu::getCpuSummary(char* cpuStr, char* opCodeStr) {
		snprintf(cpuStr, 256,
			"|:: Registors state\n"
			"A: %2.2X      F: %2.2X\n"
			"BC: %2.2X %2.2X  DE: %2.2X %2.2X  HL: %2.2X %2.2X\n"
			"SP: %4.4X   PC %4.4X",
			regs->A, regs->F,
			regs->B, regs->C, regs->D, regs->E, regs->H, regs->L,
			regs->SP, regs->PC
		);

		snprintf(
			opCodeStr, 64,
			(currInstruct == nullptr) ? "-> Opcode %2.2X failed to load" : "-> Opcode %2.2X",
			currOpcode
		);
	}



	/**
	 * @brief Defined fetch instuction from memory
	 */
	void Cpu::fetch_inst() {
		currOpcode = emuCtrl->getBus()->abRead(regs->PC);
		currInstruct = TheBoy::getByOpcode(currOpcode);

		char* bfr(new char[64]{});
		if (currInstruct == nullptr) {
			snprintf(bfr, 64, "-> Opcode %2.2X failed to load", currOpcode);
		}
		else {
			snprintf(bfr, 64, "-> Opcode %2.2X", currOpcode);
		}

		//emuCtrl->getView()->setCurrOperation(bfr);
		delete[] bfr;

		regs->PC++;
	}


	/**
	 * @brief Defines fetch data for the loaded instruction
	 */
	void Cpu::fetch_data() {
		intMem.fetchData = 0x0;
		intMem.destIsMem = false;

		if (currInstruct == NULL) {
			std::cout << "Failed to get the current Instruction" << std::endl;
			return;
		}

		switch (currInstruct->opMode) {
		case OPMODE_NONE: 	// No memory operation needed			
			return;


		case OPMODE_R:		// Memory operation only over a register
			intMem.fetchData = getRegisterValue(currInstruct->regTypeL);
			return;


		case OPMODE_R_R:	// Memory operation on two registers
			intMem.fetchData = getRegisterValue(currInstruct->regTypeR);
			return;


		case OPMODE_R_V8:	// Memory operation on a register and 8bit value
			intMem.fetchData = requestBusRead(regs->PC);
			requestCycles(1);

			regs->PC++;
			return;


		case OPMODE_R_V16:	// Memory operation on a register and 16biy value
		case OPMODE_V16: {	// Memory operation on a 16bit value
			bit16 low = requestBusRead(regs->PC);
			requestCycles(1);
			regs->PC++;

			bit16 high = requestBusRead(regs->PC);
			requestCycles(1);

			intMem.fetchData = (low | (high << 8));
			regs->PC++;
			return;
		}

		case OPMODE_V8:		// Memory operation on 8bit value
			intMem.fetchData = requestBusRead(regs->PC);
			requestCycles(1);
			regs->PC++;
			return;


		case OPMODE_AR_R:	// Memory operation on registor adress and registor
			intMem.fetchData = getRegisterValue(currInstruct->regTypeR);
			intMem.memDest = getRegisterValue(currInstruct->regTypeL);
			intMem.destIsMem = true;

			/*
			For register C
Load to the address specified by the 8-bit C register, data from the 8-bit A register. The full 16-bit absolute
address is obtained by setting the most significant byte to 0xFF and the least significant byte to the value of C,
so the possible range is 0xFF00-0xFFFF.
			*/
			if (currInstruct->regTypeL == REG_C) {
				intMem.memDest |= 0xFF00;
			}
			return;


		case OPMODE_R_AR: {	// Memory operation on a registor and a  memory registor address
			bit16 address = getRegisterValue(currInstruct->regTypeR);

			/*
			For register C
Load to the address specified by the 8-bit C register, data from the 8-bit A register. The full 16-bit absolute
address is obtained by setting the most significant byte to 0xFF and the least significant byte to the value of C,
so the possible range is 0xFF00-0xFFFF.
			*/
			if (currInstruct->regTypeR == REG_C) {
				address |= 0xFF00;
			}
			intMem.fetchData = requestBusRead(address);
			requestCycles(1);
			return;
		}

		case OPMODE_R_HLI:	// memory operation on a registor and the HL register, incrementing
			intMem.fetchData = requestBusRead(getRegisterValue(currInstruct->regTypeR));

			requestCycles(1);
			setRegisterValue(REG_HL, getRegisterValue(REG_HL) + 0x1);
			return;


		case OPMODE_R_HLD:	// Memory operation on registor and the HL register, decrementing
			intMem.fetchData = requestBusRead(getRegisterValue(currInstruct->regTypeR));

			requestCycles(1);
			setRegisterValue(REG_HL, getRegisterValue(REG_HL) - 0x1);
			break;


		case OPMODE_HLI_R:	// Memory operation on HL register from register, incrementing
			intMem.fetchData = getRegisterValue(currInstruct->regTypeR);
			intMem.memDest = getRegisterValue(currInstruct->regTypeL);
			intMem.destIsMem = true;
			setRegisterValue(REG_HL, getRegisterValue(REG_HL) + 0x1);
			return;


		case OPMODE_HLD_R:	// Memory operation on HL register from register, decrementing
			intMem.fetchData = getRegisterValue(currInstruct->regTypeR);
			intMem.memDest = getRegisterValue(currInstruct->regTypeL);
			intMem.destIsMem = true;
			setRegisterValue(REG_HL, getRegisterValue(REG_HL) - 0x1);
			return;


		case OPMODE_R_A8: 	// Memory operation on registor from 8bit memory address
			intMem.fetchData = requestBusRead(regs->PC);
			requestCycles(1);
			regs->PC++;
			return;


		case OPMODE_R_A16: {	// Memory operation on registor from 16bit memory address
			bit16 low = requestBusRead(regs->PC);
			requestCycles(1);
			regs->PC++;

			bit16 high = requestBusRead(regs->PC);
			requestCycles(1);
			regs->PC++;

			bit16 addr = (low | (high << 8));
			intMem.fetchData = requestBusRead(addr);
			requestCycles(1);
			return;
		}


		case OPMODE_A8_R:	// Memory operation on 8bit address to registor
			intMem.memDest = requestBusRead(regs->PC) | 0xFF00; // 8bit address value
			intMem.destIsMem = true;
			requestCycles(1);
			regs->PC++;
			return;


		case OPMODE_HL_SPR:	// Memory operation on SP and Hl registers
			intMem.fetchData = requestBusRead(regs->PC);
			requestCycles(1);
			regs->PC++;
			return;


		case OPMODE_A16_R: {	// Memory operation on registor to 16bit address
			bit16 low = requestBusRead(regs->PC);
			requestCycles(1);
			regs->PC++;

			bit16 high = requestBusRead(regs->PC);
			requestCycles(1);

			intMem.memDest = (low | (high << 8));
			intMem.destIsMem = true;

			regs->PC++;
			intMem.fetchData = getRegisterValue(currInstruct->regTypeR);
			return;
		}

		case OPMODE_AR_V8:	// Memory operation on 8bit value to registor address
			intMem.fetchData = requestBusRead(regs->PC);
			requestCycles(1);
			regs->PC++;
			intMem.memDest = getRegisterValue(currInstruct->regTypeL);
			intMem.destIsMem = true;
			return;

		case OPMODE_AR:		// Memory operation on registor address
			intMem.memDest = getRegisterValue(currInstruct->regTypeL);
			intMem.destIsMem = true;
			intMem.fetchData = requestBusRead(getRegisterValue(currInstruct->regTypeL));
			requestCycles(1);
			return;


		default:			// If none, this is a unknow operation mode
			char* m(new char[128]{});
			snprintf(m, 128, "[CPU] ::: Unknown Operation mode on the instruction [OPCODE: %2.2X]\n", currOpcode);
			emuCtrl->forceEmuStop(m);
			delete[] m;
			return;
		}
	}

	/**
 * @brief Defines the execution process for the instruction with the data
 */
	void Cpu::executeInst() {
		CpuFuncs::INST_FUNC exe = CpuFuncs::getInstructProcess(currInstruct->insType);
		if (!exe || currInstruct->insType == INST_NONE) {
			char* m(new char[128]{});
			snprintf(m, 128, "[CPU] ::: Unknown execution function for [OPCODE: %2.2X]\n", currOpcode);
			emuCtrl->forceEmuStop(m);
			delete[] m;
			return;
		}
		exe(this);
	}


	/// <summary>
	/// Gets the current Cpu execution tick value
	/// </summary>
	/// <returns>Execution tick value</returns>
	bit32 Cpu::getTicks() {
		std::chrono::time_point current = std::chrono::high_resolution_clock::now();
		return static_cast<bit32>(
			std::chrono::duration_cast<std::chrono::milliseconds>(current - startTime).count()
			);
	}

	/// <summary>
	/// Sleeps the cpu thread for a defined ms value
	/// </summary>
	/// <param name="msVal">Sleep ms value</param>
	void Cpu::sleepCpu(bit32 msVal) {
		std::this_thread::sleep_for(std::chrono::milliseconds(msVal));
	}
}