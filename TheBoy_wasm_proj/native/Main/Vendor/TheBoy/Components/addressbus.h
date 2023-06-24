#ifndef ADDRESSBUS_H
#define ADDRESSBUS_H

#include "common.h"

namespace TheBoy {
	class EmulatorController;


// Memory map
//  START   END		Description						Notes
// --------------------------------------------------------------------------------------------
//	0000	3FFF	16 KiB ROM bank 00				From cartridge, usually a fixed bank
//	4000	7FFF	16 KiB ROM Bank 01~NN			From cartridge, switchable bank via mapper (if any)
//	8000	9FFF	8 KiB Video RAM (VRAM)			In CGB mode, switchable bank 0/1
//	A000	BFFF	8 KiB External RAM				From cartridge, switchable bank if any
//	C000	CFFF	4 KiB Work RAM (WRAM)	
//	D000	DFFF	4 KiB Work RAM (WRAM)			In CGB mode, switchable bank 1~7
//	E000	FDFF	Mirror of C000~DDFF (ECHO RAM)	Nintendo says use of this area is prohibited.
//	FE00	FE9F	Sprite attribute table (OAM)	
//	FEA0	FEFF	Not Usable						Nintendo says use of this area is prohibited
//	FF00	FF7F	I/O Registers	
//	FF80	FFFE	High RAM (HRAM)	
//	FFFF	FFFF	Interrupt Enable register (IE)

	/**
	 * @brief Holds the AddressBus definitions
	 */
	class AddressBus {
	public:
		/**
		 * @brief Construct a new Address Bus object
		 */
		AddressBus(EmulatorController* ctrl);


		/**
		 * @brief Destroy the Address Bus object
		 */
		~AddressBus() = default;

		/**
		 * @brief Reads the value from the defined address
		 * @param addr Read address value
		 * @return bit8 Value on the defined address
		 */
		bit8 abRead(bit16 addr);


		/**
		 * @brief Reads the 16bit value from the defined address 
		 * @param addr Read address value
		 * @return bit16 Value on the defined address
		 */
		bit16 abRead16(bit16 addr);

		/**
		 * @brief Writes to a defined address
		 * @param addr Target address to be written
		 * @param val Value to be setted on the address 
		 */
		void abWrite(bit16 addr, bit8 val);


		/**
		 * @brief Writes to the defined address a 16bit value
		 * @param addr Target address to be written
		 * @param val Value to be setted on the address
		 */
		void abWrite16(bit16 addr, bit16 val);
	
	private:
		/**
		 * @brief Pointer to the emulator controller
		 */
		EmulatorController* emuCtrl;


	};
} // namespace TheBoy
#endif