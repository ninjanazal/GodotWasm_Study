#ifndef EMULATORCONTROLLER_H
#define EMULATORCONTROLLER_H

#include <iostream>
#include <thread>
#include "cartridge.h"
#include "cpu.h"
#include "ram.h"
#include "dma.h"
#include "ppu.h"
#include "lcd.h"
#include "iogb.h"
#include "timer.h"
#include "inputController.h"
#include "main.h"


namespace Concept1{
	class Main;
}
/**
 * @brief Core Project Namespace 
 */
namespace TheBoy {
	class AddressBus;
	class Cpu;
	class Ram;
	class Dma;
	class Ppu;
	class Lcd;
	class IO;
	class Timer;
	class InputController;

	
	/**
	 * @brief Defines the Emulator components
	 */
	typedef struct {
		/**
		 * @brief Holds reference to the current cartriddge
		 */
		std::shared_ptr<Cartridge> cart;

		/**
		 * @brief Emulator Address bus object
		 */
		std::shared_ptr<AddressBus> bus;

		/**
		 * @brief Emulator cpu object
		 */
		std::shared_ptr<Cpu> cpu;

		/**
		 * @brief Emulator Ram object
		 */
		std::shared_ptr<Ram> ram;


		/**
		 * @brief Emulator Dma object
		 */
		std::shared_ptr<Dma> dma;


		/**
		 * @brief Emulator Ppu object
		 */
		std::shared_ptr<Ppu> ppu;

		/// <summary>
		/// Emulator Lcd object
		/// </summary>
		std::shared_ptr<Lcd> lcd;

		/**
		 * @brief Emulator IO object
		 */
		std::shared_ptr<IO> io;


		/**
		 * @brief Emulator Timer object
		 */
		std::shared_ptr<Timer> timer;

		/// <summary>
		/// Current emulator input controller
		/// </summary>
		std::shared_ptr<InputController> inputCtrl;

	} EmulatorComponents;
	


	/**
	 * @brief Base emulator controller class
	 */
	class EmulatorController {
	private:
	

		/**
		 * @brief Holds the current emulator state values
		 */
		EmulatorState emu_state;

		/**
		 * @brief Holds references for the Emulator controller components
		 */
		EmulatorComponents comps;

		/**
		 * @brief Thred for the cpu process
		 */
		std::unique_ptr<std::thread> instThread;


		/**
		 * @brief Debug message buffer pointer
		 */
		std::string debugBuffer = "";


		/// <summary>
		/// Marks if has pending output from Serial Transfer (Link Cable)
		/// </summary>
		bool _pendingNewOut = false;


		/**
		 * @brief Updates the debug, information if available
		 */
		void debugUpdate();


		/**
		 * @brief Outputs the current IO buffer debug
		 */
		void debugOutput();


		/// <summary>
		/// Gather the input information
		/// </summary>
		void gatherInput(Concept1::Main* view);


	private:

		/**
		 * @brief Emulator internal loop
		 */
		void _run();

		/**
		 * @brief Internal event manager function
		 */
		//void manageEvents();



	public:
		/**
		 * @brief Construct a new Emulator Controller object
		 */
		EmulatorController();

		/**
		 * @brief Destroy the Emulator Controller object
		 */
		~EmulatorController();

		long ppuFrame = 0;


		void iteration(Concept1::Main* main);

		/**
		 * @brief Initialize the Emulator with a defined size
		 * @param rom_path Path to the target rom
		 */
		void Start(const char* rom_path);

		/**
		 * @brief Stops the emulation execution with a defined message
		 * @param msg Stop message
		 */
		void forceEmuStop(const char* msg);

		/**
		 * @brief Emulates a defined number of cpu cycles
		 * @param cycles 
		 */
		void emulCycles(const int& cycles);

		/**
		 * @brief Get the Ticks count
		 * @return bit16 Current tick count
		 */
		bit64 getTicks();
		
		/**
		 * @brief Get the Cartridge object
		 * @return std::shared_ptr<Cartridge> Shared pointer to the inUse cartridge
		 */
		std::shared_ptr<Cartridge> getCartridge();


		/**
		 * @brief Get the Bus object
		 * @return std::shared_ptr<AddressBus> Shared pointer to the inUse AddressBus
		 */
		std::shared_ptr<AddressBus> getBus();


		/**
		 * @brief Get the Dma object
		 * @return std::shared_ptr<Dma> Shared pointer to the inUse Dma
		 */
		std::shared_ptr<Dma> getDma();


		/**
		 * @brief Get the Ram object
		 * @return std::shared_ptr<Ram> Shared pointer to the inUse Ram
		 */
		std::shared_ptr<Ram> getRam();


		/**
		 * @brief Get the Ppu object
		 * @return std::shared_ptr<Ppu> Shared pointer to the inUse Ppu
		 */
		std::shared_ptr<Ppu> getPpu();


		/**
		 * @brief Get the Cpu object
		 * @return std::shared_ptr<Cpu> Shared pointer to the inUse Cpu
		 */
		std::shared_ptr<Cpu> getCpu();

		/**
		 * @brief Gets the IO object
		 * @return std::shared_ptr<IO> Shared pointer to the inUse IO
		 */
		std::shared_ptr<IO> getIO();


		/**
		 * @brief Get the Timer object
		 * @return std::shared_ptr<Timer> Shared pointer to the inUse Timer
		 */
		std::shared_ptr<Timer> getTimer();


		/// <summary>
		/// Gets the Lcd object
		/// </summary>
		/// <returns>Shared pointer to the inUse Lcd</returns>
		std::shared_ptr<Lcd> getLcd();

		/// <summary>
		/// Gets the input controller pointer
		/// </summary>
		/// <returns>Shared pointer to the inUse Input controller</returns>
		std::shared_ptr<InputController> getInput();

				/**
		 * @brief Steps the defined cpu
		 * @param state Current emulator state
		 * @param cpu Target step cpu
		 */
		void cpuStep(Concept1::Main* view);

	};
	
} // namespace TheBoy


#endif