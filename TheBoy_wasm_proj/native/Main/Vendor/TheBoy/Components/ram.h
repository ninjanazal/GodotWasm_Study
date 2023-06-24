#ifndef RAM_H
#define RAM_H

#include "common.h"

namespace TheBoy {
	class EmulatorController;

	class Ram {
	public:

		/**
		 * @brief Construct a new Ram object
		 * @param ctrl Emulator controller pointer
		 */
		Ram(EmulatorController* ctrl);


		/// <summary>
		/// Destroy the Ram object
		/// </summary>
		~Ram();


		/**
		 * @brief Reads a 16bit addres value from the working RAM
		 * @param addr Target Memory address
		 * @return bit8 Value read
		 */
		bit8 wRead(bit16 addr);


		/**
		 * @brief Writes a 8bit value to the defined 16bit addres on the working RAM
		 * @param addr Target Memory address
		 * @param val value to be writen
		 */
		void wWrite(bit16 addr, bit8 val);


		/**
		 * @brief Reads a 8bit value on the 16bit address from the High RAM
		 * @param addr Target Memory address
		 * @return bit8 8bit Readed value
		 */
		bit8 hRead(bit16 addr);


		/**
		 * @brief Writes a 8bit value to the defined 16bit address from the High RAM
		 * @param addr Target Memory address
		 * @param val value to be writen
		 */
		void hWrite(bit16 addr, bit8 val);

	private:
		/**
		 * @brief work RAM Memory allocation
		 */
		bit8* workRam;

		/**
		 * @brief High RAM Memory allocation
		 */
		bit8* highRam;


		/**
		 * @brief Pointer to the emulator controller
		 */
		EmulatorController* emuCtrl;
	};
} // namespace TheBoy


#endif