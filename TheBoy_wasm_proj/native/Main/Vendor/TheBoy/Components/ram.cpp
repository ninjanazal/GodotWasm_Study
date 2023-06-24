#include "ram.h"
#include "emulatorController.h"


namespace TheBoy {

	/**
	 * @brief Construct a new Ram object
	 */
	Ram::Ram(EmulatorController* ctrl ){
		std::cout << "[RAM] ::: RAM has been created!" << std::endl;
		emuCtrl = ctrl;

		workRam = new bit8[0x2000] { };
		highRam = new bit8[0x80] { };
	}

	/// <summary>
	/// Destroy the Ram object
	/// </summary>
	Ram::~Ram() {
		delete[] workRam;
		delete[] highRam;
	}

	/**
	 * @brief Reads a 16bit addres value from the working RAM
	 * @param addr Target Memory address
	 * @return bit8 Value read
	 */
	bit8 Ram::wRead(bit16 addr) {
		// Since this memory block start at $C000
		addr -= 0xC000;
		
		if(addr >= 0x2000){
			printf("[RAM] ::: Invalid work RAM Read at %05X!", (addr + 0xC000));
			fflush(stdout);
			return 0x0;
		}
#if VERBOSE
		printf("[RAM] ::: Reading Work RAM at  %.4X! -> %2.2X\n", (addr + 0xC000), workRam[addr]);
		fflush(stdout);
#endif

		return workRam[addr];
	}


	/**
	 * @brief Writes a 8bit value to the defined 16bit addres on the working RAM
	 * @param addr Target Memory address
	 * @param val value to be writen
	 */
	void Ram::wWrite(bit16 addr, bit8 val) {
		// Since this memory block start at $C000
		addr -= 0xC000;
#if VERBOSE
		printf("[RAM] ::: Writing Work RAM at %.4X -> %2.2X!\n", (addr + 0xC000), val);
		fflush(stdout);
#endif

		workRam[addr] = val;
	}


	/**
	 * @brief Reads a 8bit value on the 16bit address from the High RAM
	 * @param addr Target Memory address
	 * @return bit8 8bit Readed value
	 */
	bit8 Ram::hRead(bit16 addr) {
		// Since this memory block start at $FF80
		addr -= 0xFF80;
#if VERBOSE
		printf("[RAM] ::: Reading High RAM at  %.4X\n!", (addr + 0xC000));
		fflush(stdout);
#endif
		return highRam[addr];
	}


	/**
	 * @brief Writes a 8bit value to the defined 16bit address from the High RAM
	 * @param addr Target Memory address
	 * @param val value to be writen
	 */
	void Ram::hWrite(bit16 addr, bit8 val) {
		// Since this memory block start at $FF80
		addr -= 0xFF80;
#if VERBOSE
		printf("[RAM] ::: Writing High RAM at %.4X -> %2.2X!\n", (addr + 0xC000), val);
		fflush(stdout);
#endif
		highRam[addr] = val;
	}

} // namespace TheBoy

