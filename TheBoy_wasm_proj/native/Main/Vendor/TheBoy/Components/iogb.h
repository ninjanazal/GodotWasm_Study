#ifndef IO_H
#define IO_H

#include "emulatorController.h"

namespace TheBoy {

	/**
	 * @brief Class managing the IO Operrations
	 */
	class IO {
	public:

		/**
		 * @brief Construct a new IO object
		 * @param ctrl Target emulator controller
		 */
		IO(EmulatorController* ctrl);

		/**
		 * @brief Destroy the IO object
		 */
		~IO();


		/**
		 * @brief Reads the IO valeu on the defined address
		 * @param addr Target read addres
		 * @return bit8 Value on the target addres 
		 */
		bit8 read(bit16 addr);


		/**
		 * @brief Write the io value on the defined addres
		 * @param addr Target Write addres
		 * @param val value to be written
		 */
		void write(bit16 addr, bit8 val);
	
	private:
		/**
		 * @brief Pointer to the target emulator controller
		 */
		EmulatorController* emulCtrl;


		/**
		 * @brief Holds the current IO Serial data
		 */
		bit8* seriaData;
	};	
} // namespace TheBoy
#endif