#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "tuple"

namespace TheBoy {
	class Cpu;
	
	namespace InterruptFuncs {

		/*
			INT 40 - VBlank Interrupt
				The VBlank interrupt occurs ca. 59.7 times a second on a handheld Game Boy (DMG or CGB) 
				During this period video hardware is not using VRAM so it may be freely accessed.
				This period lasts approximately 1.1 milliseconds.

			INT 48 - STAT Interrupt
				One very popular use is to indicate to the user when the video hardware is about to redraw
				a given LCD line. This can be useful for dynamically controlling the SCX/SCY registers
				($FF43/$FF42) to perform special video effects.

			INT 50 - Timer Interrupt
				Transmitting and receiving serial data is done simultaneously.
				The received data is automatically stored in SB.
				The serial I/O port on the Game Boy is a very simple 
				setup and is crude compared to standard RS-232 (IBM-PC) or RS-485 (Macintosh) serial ports.
				There are no start or stop bits.
			
			INT 60 - Joypad Interrupt
				The Joypad interrupt is requested when any of P1 bits 0-3 change from High to Low.
				This happens when a button is pressed (provided that the action/direction buttons are enabled
				by bit 5/4, respectively), however, due to switch bounce, one or more High to Low transitions 
				are usually produced when pressing a button.

		*/

		// Defined as flags on a 8bit value, will be used as a mask
		typedef enum {
			INTR_VBLANK = 0b1,
			INTR_STAT = 0b10,
			INTR_TIMER = 0b100,
			INTR_SERIAL = 0b1000,
			INTR_JOYPAD	= 0b10000
		} InterruptType;


		/**
		 * @brief Requests a Interrupt on the defined cpu
		 * @param cpu Target Cpu
		 * @param iType Interrupt type
		 */
		void request_interrupt(Cpu* cpu, InterruptType iType);


		/**
		 * @brief Handles a interrupt on the defined cpu
		 * @param cpu Target Cpu
		 */
		void handle_interrupt(Cpu* cpu);

	} // namespace InterruptFuncs	
} // namespace TheBoy



#endif