#include "timer.h"
#include "interrupt.h"

namespace TheBoy {

	/**
 	* @brief Construct a new Timer object
	 * @param ctrl Target Emulator controller
	 */
	Timer::Timer(EmulatorController* ctrl) : emulCtrl(ctrl) {
		// Timer DIV default value
		regs.DIV = 0xAC00;
		std::cout << "[TIMER] ::: Timer has been created" << std::endl;
	}

	/**
	 * @brief Timer iteration call
	 */
	void Timer::tick() {
		bit16 oldDiv = regs.DIV;
		regs.DIV++;

		bool tUpdate = false;

		/*
			LookUp for the Input clock select
			Bits 1-0 - Input Clock Select
				00: CPU Clock / 1024 (DMG, SGB2, CGB Single Speed Mode:   4096 Hz, SGB1:   ~4194 Hz, CGB Double Speed Mode:   8192 Hz)
				01: CPU Clock / 16   (DMG, SGB2, CGB Single Speed Mode: 262144 Hz, SGB1: ~268400 Hz, CGB Double Speed Mode: 524288 Hz)
				10: CPU Clock / 64   (DMG, SGB2, CGB Single Speed Mode:  65536 Hz, SGB1:  ~67110 Hz, CGB Double Speed Mode: 131072 Hz)
				11: CPU Clock / 256  (DMG, SGB2, CGB Single Speed Mode:  16384 Hz, SGB1:  ~16780 Hz, CGB Double Speed Mode:  32768 Hz)

		*/
		int offMatch[] = {9, 3, 5, 7};
		tUpdate = (oldDiv & (1 << offMatch[(regs.TAC & 0b11)])) &&
				(!(regs.DIV & (1 << offMatch[(regs.TAC & 0b11)])));

		// Bit  2   - Timer Enable
		if(tUpdate && regs.TAC & (1 << 2)){
			regs.TIMA++;
			if(regs.TIMA == 0xFF){
				regs.TIMA = regs.TMA;
				emulCtrl->getCpu()->requestInterrupt(InterruptFuncs::INTR_TIMER);
			}
		}
	}


	/**
	 * @brief Writes a value to a defined addres
	 * @param addr target address
	 * @param val value to be written
	 */
	void Timer::write(bit16 addr, bit8 val){
		switch (addr) {
			/*
			FF04 - DIV - Divider Register (R/W)
				This register is incremented at a rate of 16384Hz (~16779Hz on SGB).
				Writing any value to this register resets it to $00.
			*/
		case 0xFF04:
			regs.DIV = 0x0;
			break;

			/*
			FF05 - TIMA - Timer counter (R/W)
			*/
		case 0xFF05:
			regs.TIMA = val;
			break;

			/*
			FF06 - TMA - Timer Modulo (R/W)
			*/
		case 0xFF06:
			regs.TMA = val;
			break;
		
			/*
			FF07 - TAC - Timer Control (R/W)
			*/
		case 0xFF07:
			regs.TAC = val;
			break;
		}
	}


	/**
	 * @brief Reads the value from the defined addres
	 * @param addr Target read address
	 * @return bit8 Value 
	 */
	bit8 Timer::read(bit16 addr) {
		switch (addr) {
		case 0xFF04:
			// only using the top byte of the registor
			return regs.DIV >> 8;

		case 0xFF05:
			return regs.TIMA;

		case 0xFF06:
			return regs.TMA;

		case 0xFF07:
			return regs.TAC;
		
		default:
			return 0x0;
		}
	}


	/**
	 * @brief Get the Register DIV object
	 * @return bit16 DIV Register value
	 */
	bit16 Timer::getRegisterDIV(){
		return regs.DIV;
	}

	/**
	 * @brief Set the Register DIV object value
	 * @param val Defined value to be setted
	 */
	void Timer::setRegisterDIV(bit16 val) {
		regs.DIV = val;
	}
	
	
} // namespace TheBoy
