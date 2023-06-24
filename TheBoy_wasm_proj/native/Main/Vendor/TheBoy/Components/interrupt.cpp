#include "interrupt.h"
#include "cpu.h"

namespace TheBoy {
	namespace InterruptFuncs {

		/**
		 * @brief Maps the address to the corresponding interrupt type
		 */
		static std::tuple<bit8, InterruptType> InterruptAddres[] = {
			std::make_tuple(0x40, INTR_VBLANK),
			std::make_tuple(0x48, INTR_STAT),
			std::make_tuple(0x50, INTR_TIMER),
			std::make_tuple(0x58, INTR_SERIAL),
			std::make_tuple(0x60, INTR_JOYPAD)
		};


		/**
		 * @brief Aux function to push the current PC value to the stack and assign the interrup addres
		 * @param cpu Target Cpu
		 * @param addr Interrupt jump address
		 */
		void manage_interrupt(Cpu* cpu, bit16 addr) {
			cpu->push16(cpu->getRegisterValue(REG_PC));
			cpu->setRegisterValue(REG_PC, addr);
		}


		/**
		 * @brief Check the current state for the defined interrup type
		 * @param cpu Target Cpu object
		 * @param addr interrupt address
		 * @param iType Interrupt type enum value
		 * @return true/false If the interrupt was resolved
		 */
		bool interrupt_check(Cpu* cpu, bit16 addr, InterruptType iType){
			if(cpu->getInterrFlags() & iType && cpu->getCpuIERegister() & iType) {
				manage_interrupt(cpu, addr);
				cpu->setInterrFlags(cpu->getInterrFlags() & ~iType);
				cpu->setHaltedValue(false);
				cpu->setInterruptMasterState(false);

				return true;
			}
			return false;
		}


		/**
		 * @brief Requests a Interrupt on the defined cpu
		 * @param cpu Target Cpu
		 * @param iType Interrupt type
		 */
		void request_interrupt(Cpu* cpu, InterruptType iType) {

		}


		/**
		 * @brief Handles a interrupt on the defined cpu
		 * @param cpu Target Cpu
		 */
		void handle_interrupt(Cpu* cpu) {
			// Check all the interrupt types
			for (int i = 0; i < 5; i++) {
				if(interrupt_check(
					cpu, std::get<0>(InterruptAddres[i]), std::get<1>(InterruptAddres[i])
				)) { return; }
			}
		}

	} // namespace InterruptFuncs	
} // namespace TheBoy