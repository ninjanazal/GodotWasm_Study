#ifndef DMA_H
#define DMA_H

#include "emulatorController.h"

namespace TheBoy {
	/*
		FF46 - DMA (DMA Transfer and Start Address) (R/W)
			Writing to this register launches a DMA transfer from ROM or RAM to OAM (Object Attribute Memory).
			The transfer takes 160 machine cycles
	*/

	class Dma {
	public:
		/**
		 * @brief Construct a new Dma object
		 * @param ctrl 
		 */
		Dma(EmulatorController* ctrl);


		/**
		 * @brief Destroy the Dma object
		 */
		~Dma() = default;


		/**
		 * @brief Starts a Dma data Transfer
		 * @param val Initial value
		 */
		void start(bit8 val);


		/**
		 * @brief Direct memory access iteration
		 */
		void step();


		/**
		 * @brief Gets if is currently transfering data
		 * @return true/flase Current transfering state
		 */
		bool isTransfering();

	private:	
		
		/**
		 * @brief Pointer to the target emulator controller
		 */
		EmulatorController* emulCtrl;

		/**
		 * @brief Marks the current Dma State
		 */
		bool enabled = false;

		/**
		 * @brief Current Byte marker
		 */
		bit8 currByte = 0x0;

		/**
		 * @brief Current Working value
		 */
		bit8 currVal = 0x0;


		/**
		 * @brief Start Delay value 
		 */
		bit8 s_Delay = 2;

	};

} // namespace TheBoy


#endif