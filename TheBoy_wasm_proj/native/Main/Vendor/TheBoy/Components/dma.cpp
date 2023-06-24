#include "dma.h"


namespace TheBoy {

	Dma::Dma(EmulatorController* ctrl) : emulCtrl(ctrl) {
		std::cout << "[DMA] ::: DMA has been created" << std::endl;
	}


	/**
	 * @brief Starts a Dma data Transfer
	 * @param val Initial value
	 */
	void Dma::start(bit8 val) {
		enabled = true;
		currByte = 0;
		s_Delay = 2;
		currVal = val;
	}


	/**
	 * @brief Direct memory access iteration
	 */
	void Dma::step() {
		if(!enabled){
			return;
		}

		if(s_Delay) {
			s_Delay--;
			return;
		}

		/*
		The written value specifies the transfer source address divided by $100, that is,
		source and destination are:

			Source:      $XX00-$XX9F   ;XX = $00 to $DF
			Destination: $FE00-$FE9F

		*/

		emulCtrl->getPpu()->oamWrite(
			currByte,
			emulCtrl->getBus()->abRead((currVal * 0x100) + currByte)
		);
		currByte++;
		enabled = currByte < 0xA0;
	}


	/**
	 * @brief Gets if is currently transfering data
	 * @return true/flase Current transfering state
	 */
	bool Dma::isTransfering() {
		return enabled;
	}
	
} // namespace TheBoy
