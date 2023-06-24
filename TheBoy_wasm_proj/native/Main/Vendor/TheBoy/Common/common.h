#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <map>
#include <cstdint>
#include <memory>


namespace TheBoy {

	// - - - - - - - - - - - - - - - - - - - -
	//                 MACROS               //
	// - - - - - - - - - - - - - - - - - - - -


	/**
	 * @brief Defines the GameBoy ScreenSize
	 */
#define BASE_SCREEN (unsigned int[]){160, 144}


	 /**
	  * @brief Gets the bit n state from value
	  */
#define GETBIT(from, n) (from >> n) & 1


	  /**
	   * @brief Sets a defined bit value
	   */
#define SETBIT(to, n, val) (to = (to & ~(1 << n)) | (val << n));


	   /**
		* @brief Checks if a value is between a min and max value
		*/
#define BETWEEN(val, min, max) ((val >= min) && (val <= max))


	// - - - - - - - - - - - - - - - - - - - -
	//               TypeDefs               //
	// - - - - - - - - - - - - - - - - - - - -

	/**
	 * @brief Defines the GameBoy ScreenSize
	 */
	const unsigned int kBaseScreen[] = { 160, 144 };

	/**
	 * @brief Defined types for the current project type
	 * Available types bit8(uint8_t), bit16(uint16_t), bit32(uint32_t) and bit64(uint_64_t)
	 */
	typedef uint8_t bit8;
	typedef uint16_t bit16;
	typedef uint32_t bit32;
	typedef uint64_t bit64;


	/**
	 * @brief Defines teh Emulator state
	 */
	typedef struct EmulatorState {
		/**
		 * @brief Marks if the current emulator is paused
		 */
		bool paused;


		/**
		 * @brief marks if the current emulator is running
		 */
		bool running;


		/**
		 * @brief Holds the processor/Timer ticks count
		 */
		bit64 ticks;

		/**
		 * @brief Resets the Emulator state values
		 */
		void reset() {
			paused = false;
			running = true;
			ticks = 0;
		};

	} EmulatorState;


	/**
	 * @brief Invert the order of a bit16 value, swith high 8bit to the lower
	 * @param val Value to ve reversed
	 * @return bit16 Reversed value
	 */
	static bit16 reverse16(const bit16 val) {
		return ((val & 0xFF00) >> 8 | (val & 0x00FF) << 8);
	}

}

#endif