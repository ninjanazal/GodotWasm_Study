#ifndef CPU_H
#define CPU_H

#include "common.h"
#include "interrupt.h"
#include "addressbus.h"
#include "instruc_funcs.h"
#include <chrono>

namespace TheBoy {
	class EmulatorController;

/*
	16-bit	Hi	Lo	Name/Function
	----------------------------------
	AF	A	-	Accumulator & Flags
	BC	B	C	BC
	DE	D	E	DE
	HL	H	L	HL
	SP	-	-	Stack Pointer
	PC	-	-	Program Counter/Pointer
	----------------------------------

As shown above, most registers can be accessed either as one 16-bit register,
 or as two separate 8-bit registers.

	[Flags]
	----------------------------------
	7	z	Zero flag
	6	n	Subtraction flag (BCD)
	5	h	Half Carry flag (BCD)
	4	c	Carry flag
	----------------------------------

*/
	typedef struct {
		bit8 A;
		bit8 F;
		bit8 B;
		bit8 C;
		bit8 D;
		bit8 E;
		bit8 H;
		bit8 L;
		bit16 SP;
		bit16 PC;
	} Registers;

	/**
	 * @brief Holds the Cpu definitions
	 */
	class Cpu {
	public:
		/**
		 * @brief Construct a new Cpu object
		 */
		Cpu(EmulatorController* ctrl);


		/**
		 * @brief Destroy the Cpu object
		 */
		~Cpu() = default;

		/**
		 * @brief Defines the cpu iteration
		 */
		void step();

		/**
		 * @brief Get the Register Value object
		 * @param regType Defined register to get
		 * @return bit16 Data on the setted register
		 */
		bit16 getRegisterValue(RegisterType regType);


		/**
		 * @brief Set the Register Value
		 * @param type Registor type to be set
		 * @param value Value to be set
		 */
		void setRegisterValue(RegisterType regType, bit16 value);


		/**
		 * @brief Get the Register Value Byte object 
		 * @param regType Defined register to get
		 * @return bit8 Data on the setted register
		 */
		bit8 getRegisterValueByte(RegisterType regType);


		/**
		 * @brief Set the Register Value Byte object
		 * @param regType Registor type to be set
		 * @param value Value to be set
		 */
		void setRegisterValueByte(RegisterType regType, bit8 value);


		/**
		 * @brief Get the Interrupt value
		 * @return true/False Current interrupt state
		 */
		bool getMasterInterrupt();


		/**
		 * @brief Set the Interrupt State value
		 * @param iState New interrupt state value
		 */
		void setInterruptMasterState(bool iState);


		/**
		 * @brief Sets the enablingIntMaster to enable the interrup master
		 */
		void enablingIME();


		/**
		 * @brief Gets the current Z flag value
		 * @return bit8 Current z value
		 */
		bit8 getZFlag();


		/**
		 * @brief Gets the current C flag value
		 * @return bit8 Current C value
		 */
		bit8 getCFlag();


		/**
		 * @brief Get the Current OP Code value
		 * @return bit8 Current OP Code value
		 */
		bit8 getCurrentOPCode();


		/**
		 * @brief Set the Flags values
		 * @param z Zero flag
		 * @param n Subtraction flag (null flag)
		 * @param h Half Carry flag
		 * @param c Carry flag
		 */
		void setFlags(bit8 z, bit8 n, bit8 h, bit8 c);


		/**
		 * @brief Get the Curr Instruct object
		 * @return Instruc* Pointer to the current instruction
		 */
		Instruc* getCurrInstruct();

		/**
		 * @brief Get the Fetched Data value
		 * @return bit16 Fetched data value
		 */
		bit16 getFetchedData();


		/**
		 * @brief Gets if the internal memory marks that fetched is memoryLoc
		 * @return true/false Is memory location
		 */
		bool getDestenyIsMem();


		/**
		 * @brief Get the Memory Dest internal memory value
		 * @return bit16 Memory destination
		 */
		bit16 getMemoryDest();


		/**
		 * @brief Updates the cpu program counter to the current fetched data
		 */
		void updatePCtoFetched();


		/**
		 * @brief Requests the emulator controller to emulate a defined cycle value
		 * @param cycles Cycle value
		 */
		void requestCycles(const int& cycles);


		/**
		 * @brief Requests Writes to a defined address
		 * @param addr Target address to be written
		 * @param val Value to be setted on the address 
		 */
		void requestBusWrite(bit16 addr, bit8 val);


		/**
		 * @brief Requests Writes to a defined address
		 * @param addr Target address to be written
		 * @param val 16bit Value to be setted on the address 
		 */
		void requestBusWrite16(bit16 addr, bit16 val);


		/**
		 * @brief Request Read to a defined addres
		 * @param addr Target read address
		 * @return bit8 Readed value
		 */
		bit8 requestBusRead(bit16 addr);

		/**
		 * @brief Get the Cpu IE Register value
		 * @return bit8 Current IE register value
		 */
		bit8 getCpuIERegister();

		/**
		 * @brief Set the Cpu IE Register value
		 * @param val New Ie register value
		 */
		void setCpuIERegister(bit8 val);


		/**
		 * @brief Push a 8bit data to the next stack pointer location, decrement the sp val
		 * @param data Data to be pushed
		 */
		void push(bit8 data);

		/**
		 * @brief Push a 16bit data to the next stack pointer location, decrement the sp val
		 * @param data Data to be pushed
		 */
		void push16(bit16 data);

		/**
		 * @brief Pop a 8bit data from the current stack pointer location, increment sp val
		 * @return bit8 Current stack pointer data location
		 */
		bit8 pop();


		/**
		 * @brief Pop a 16bit data from the current stack pointer location, increment sp val
		 * @return bit8 Current stack pointer data location
		 */
		bit16 pop16();


		/**
		 * @brief Set the Halted Value
		 * @param value New halted value
		 */
		void setHaltedValue(bool value);


		/**
		 * @brief Get the Halted State value
		 * @return true/false Current halted value
		 */
		bool getHaltedState();


		/**
		 * @brief Get the Interr Flags Vvalue
		 * @return bit8 Flags value
		 */
		bit8 getInterrFlags();


		/**
		 * @brief Set the Interr Flags value
		 * @param flags New Flags value
		 */
		void setInterrFlags(bit8 flags);


		/**
		 * @brief Requests a interrupt by type on the current Cpu
		 * @param iType Iterrupt type
		 */
		void requestInterrupt(InterruptFuncs::InterruptType iType);


		/// <summary>
		/// Gets the cpu summary string, used to print
		/// </summary>
		/// <param name="cpuStr">Pointer to target Registor summary string</param>
		/// <param name="opCodeStr">Pointer to target opCode summary string</param>
		void getCpuSummary(char* cpuStr, char* opCodeStr);


		/// <summary>
		/// Gets the current Cpu execution tick value
		/// </summary>
		/// <returns>Execution tick value</returns>
		bit32 getTicks();


		/// <summary>
		/// Sleeps the cpu thread for a defined ms value
		/// </summary>
		/// <param name="msVal">Sleep ms value</param>
		void sleepCpu(bit32 msVal);

	private:
		/**
		 * @brief Pointer to the emulator controller
		 */
		EmulatorController* emuCtrl;


		/**
		 * @brief Shared pointer to the registers values
		 */
		std::shared_ptr<Registers> regs;


		/// <summary>
		/// Marks the starting time stamp
		/// </summary>
		std::chrono::high_resolution_clock::time_point startTime;


		/**
		 * @brief Defined and declared instruction memory map
		 */
		struct InternMem {
			bit16 fetchData;
			bit16 memDest;
			bool destIsMem;
		} intMem;


		/**
		 * @brief Marks if the current cpu has been halted, its in idle mode
		 */
		bool cpuHLT;

		/**
		 * @brief Marks if the Cpu is currently suspended by an interrupt handler
		 */
		bool interruptMasterState;


		/**
		 * @brief Marks the enabling the interrupt master state (IME), used to managing the cpu cycles
		 */
		bool enablingIntMaster;


		/**
		 * @brief Cpu Interrupt Enable register (0xFFFF) on memory
		 */
		bit8 interruptEnable;


		/**
		 * @brief Marks the current interrupt Setted Flags
		 */
		bit8 interruptFlags;


		/**
		 * @brief Marks the current opcode instruction
		 */
		bit8 currOpcode;


		/**
		 * @brief Pointer to the current target instruction 
		 */
		Instruc *currInstruct;


		/**
		 * @brief Resets the current cpu state
		 */
		void reset();


		/**
		 * @brief Defined fetch instuction from memory
		 */
		void fetch_inst();


		/**
		 * @brief Defines fetch data for the loaded instruction
		 */
		void fetch_data();


		/**
		 * @brief Defines the execution process for the instruction with the data
		 */
		void executeInst();
	};
}

#endif