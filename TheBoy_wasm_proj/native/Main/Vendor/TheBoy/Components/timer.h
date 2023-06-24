#ifndef TIMER_H
#define TIMER_H

#include "emulatorController.h"

namespace TheBoy {
	class Timer {
	/*
		Timer and Divider Registers
		FF04 - DIV - Divider Register (R/W)
		FF05 - TIMA - Timer counter (R/W)
		FF06 - TMA - Timer Modulo (R/W)
		FF07 - TAC - Timer Control (R/W)
	*/

	typedef struct {
		bit16 DIV;
		bit8 TIMA;
		bit8 TMA;
		bit8 TAC;
	} TimerRegisters;
	

	public:
	/**
	 * @brief Construct a new Timer object
	 * @param ctrl Target Emulator controller
	 */
	Timer(EmulatorController* ctrl);
	
	
	/**
	 * @brief Destroy the Timer object
	 */
	~Timer() = default;


	/**
	 * @brief Timer iteration call
	 */
	void tick();


	/**
	 * @brief Writes a value to a defined addres
	 * @param addr target address
	 * @param val value to be written
	 */
	void write(bit16 addr, bit8 val);


	/**
	 * @brief Reads the value from the defined addres
	 * @param addr Target read address
	 * @return bit8 Value 
	 */
	bit8 read(bit16 addr);


	/**
	 * @brief Get the Register DIV object
	 * @return bit16 DIV Register value
	 */
	bit16 getRegisterDIV();


	/**
	 * @brief Set the Register DIV object value
	 * @param val Defined value to be setted
	 */
	void setRegisterDIV(bit16 val);


	private:
		/**
		 * @brief Pointer to the target emulator controller
		 */
		EmulatorController* emulCtrl;

		/**
		 * @brief Holds the internal timer register values
		 */
		TimerRegisters regs { };
	
	};	
} // namespace TheBoy


#endif