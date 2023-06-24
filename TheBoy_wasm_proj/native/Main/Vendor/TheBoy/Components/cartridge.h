#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <collections.h>

namespace TheBoy {
	class EmulatorController;

	/**
	 * @brief Defines the Cartridge state
	 */
	typedef struct {
		bit8 newCartType;	// New layout in use	(0x033)
		bit8 entry[4];		// Entry Point 			(0x100 -> 0x0103)
		bit8 logo[0x30]; 	// Nintendo Logo 		(0x0104 -> 0x0133)
		char title[16];		// game Title 			(0x0134 -> 0x0143)
		char manu_code[4]; 	// Manufacturer code 	(0x013F -> 0x0142)
		bit8 cgb_flag; 		// CGB flag 			(0x0143)
		bit16 lic_code; 	// New Licensse Code 	(0x0144 -> 0x0145)
		bit8 sgb_flag; 		// SGB flag 			(0x0146)
		bit8 cart_type;		// cartridge type		(0x0147)
		bit8 rom_size;		// ROM size				(0x0148)
		bit8 ram_size;		// RAM size				(0x0149)
		bit8 dest_code;		// Destination Code		(0x014A)
		bit8 old_lic_code;	// Old Licensee Code	(0x014B)
		bit8 rom_version;	// ROM Version number	(0x014C)
		bit8 h_checksum;	// header checksumn		(0x014D)
		bit16 checksum;		// Global Checksum		(0x014E -> 0x014F)
	} CartridgeState;


	/**
	 * @brief Holds the cartridge logic information and definitions
	 */
	class Cartridge {
	public:
		/**
		 * @brief Construct a new Cartridge object
		 * @param path Path to the cartridge to be loaded
		 * @param ctrl Target emulator controller reference
		 */
		Cartridge(EmulatorController* ctrl, const char* path);


		/**
		 * @brief Destroy the Cartridge object
		 */
		~Cartridge() = default;


		/**
		 * @brief Tries to load the cartridge from the defined path
		 * @return true If was able to load
		 * @return false If failed to load
		 */
		bool loadCartridgeFromFile();


		/**
		 * @brief Get the Cart Licensee Name object
		 * @return char* Cartridge Licensee Name
		 */
		const char* getCartLicenseeName();


		/**
		 * @brief Get the Cart Type Name object
		 * @return char* Cartridge Type name
		 */
		const char* getCartTypeName();


		/**
		 * @brief Gets if the current cartridge is using a new layout
		 * @return true Is using the new layout
		 * @return false Is using the old layout
		 */
		bool isNewTypeCartridge();


		/**
		 * @brief Reads data from the loaded cartridge
		 * @param addr Cartridge address
		 * @return bit8 Valeu on the defined address
		 */
		bit8 read(bit16 addr);


		/**
		 * @brief Writes data to the defined cartridge address
		 * @param addr Target address
		 * @param val Value to be written
		 */
		void write(bit16 addr, bit8 val);

		/// <summary>
		/// Get if the cart has a battery
		/// </summary>
		bool cartHasBattery();

		/// <summary>
		/// Get if it needs to be saved
		/// </summary>
		bool needSave();

		/// <summary>
		/// Loads data from the battery mem
		/// </summary>
		void batteryLoad();


		/// <summary>
		/// Saves data to the battery mem
		/// </summary>
		void batterySave();

	private:
		/**
		 * @brief Pointer to the target emulator controller
		 */
		 ///
		EmulatorController* emulCtrl;

		/**
		 * @brief Holds the cartridge path
		 */
		const char* path;

		/**
		 * @brief Holds the current rom Size (kiBytes)
		 */
		bit32 rom_size = 0;


		/**
		 * @brief Pointer to the loaded rom byte data
		 */
		 //bit8 rom_data;
		bit8* rom_data = nullptr;


		/**
		 * @brief Holds the current cartridge state
		 */
		std::shared_ptr<CartridgeState> cart_state;

		/// For MBC1 Banking 

		/// <summary>
		/// A000-BFFF - RAM Bank 00-03, if any (Read/Write)
		/// </summary>
		bool enabledRam;


		/// <summary>
		/// This area normally contains the first 16 KiB (bank 00) of the cartridge ROM.
		/// In 1 MiB and larger cartridges(that use the 2 - bit second banking register for extended ROM banking),
		/// entering mode 1 (see below) will allow that second banking register to apply to reads 
		/// from this region in addition to the regular 4000 - 7FFF banked region,
		/// resulting in accessing banks $20 / $40 / $60 for regular large ROM carts,
		///  or banks $10 / $20 / $30 for an MBC1M multi - cart(see note below).
		/// </summary>
		bool bankingRam;

		/// <summary>
		/// Pointer to the bank x value
		/// </summary>
		bit8* romBankX;

		/// <summary>
		/// Defined Banking Mode
		/// </summary>
		bit8 bankingMode;

		/// <summary>
		/// Rom Bank Value
		/// </summary>
		bit8 romBankVal;

		/// <summary>
		/// Ram Bank Value
		/// </summary>
		bit8 ramBankVal;

		/// <summary>
		/// In Use bank
		/// </summary>
		bit8* currRambank;

		/// <summary>
		/// Available banks
		/// </summary>
		bit8* ramBanks[16];


		/// For Battery

		/// <summary>
		/// Mark if has a bettery
		/// </summary>
		bool hasBattery;

		/// <summary>
		/// Flags if needs saving
		/// </summary>
		bool needsSave;


		/**
		 * @brief Prints the values from a loaded cartridge
		 */
		void printCartridgeValues();

		/**
		 * @brief Validates the 8bit checksum values on the $0134-014C header area
		 * @return true Valid cartridge
		 * @return false Invalid cartridge
		 */
		bool cartridgeCheckSum();


		/// <summary>
		/// Get if the cart is a MBC1 
		/// </summary>
		bool isMBCOne();

		/// <summary>
		/// Get if the cart is a MBC3 type
		/// </summary>
		/// <returns>Is a mbc3 type</returns>
		bool isMBCThree();

		/**
		 * @brief Assigned the loaded data to the correct struct values
		 */
		void assignCartData();

		/// <summary>
		/// Create cartridge banks
		/// </summary>
		void createBanks();
	};
} // namespace TheBoy
#endif