#pragma once
#ifndef PPU_STATES_H
#define PPU_STATES_H


namespace TheBoy {
	class EmulatorController;

	/// <summary>
	/// Defines the PPU internal state machine resolvers
	/// </summary>
	namespace PpuStates {
		/*
		One frame:
			70224 dots
			@ 59.7 fps

		Mode 2
			OAM scan
			80 dots
		
		Mode 3
			Drawing pixels
			172�289 dots
		
		Mode 0
			Horizontal blank
			87�204 dots	

		*/


		/// <summary>
		/// OAM Scan (Mode 2), this will scan for 80 pixels
		/// </summary>
		/// <param name="ctrl">Reference to the Target emulatorController</param>
		void mode_OAM(EmulatorController* ctrl);


		/// <summary>
		/// Drawing Pixels (Mode 3), this will iterate for at least 172 after the Mode 2
		/// </summary>
		/// <param name="ctrl">Reference to the Target emulatorController</param>
		void mode_XFER(EmulatorController* ctrl);


		/// <summary>
		/// Vertical Blank (Mode 1), this will iterate for all the 154 display lines
		/// </summary>
		/// <param name="ctrl">Reference to the Target emulatorController</param>
		void mode_VBLANK(EmulatorController* ctrl);

		/// <summary>
		/// Horizontal Blank (Mode 0), this will iterate for all the horizontal lines
		/// </summary>
		/// <param name="ctrl"Reference to the Target emulatorController></param>
		void mode_HBLANK(EmulatorController* ctrl);

		/// <summary>
		/// Holds the vertical line increment action
		/// </summary>
		/// <param name="ctrl">Reference to the current emulator controller</param>
		void vertLineIncrement(EmulatorController* ctrl);


		/// <summary>
		/// Loads the Sprites for the target line
		/// </summary>
		/// <param name="ctrl">Reference to the current emulator controller</param>
		void loadLineSpt(EmulatorController* ctrl);
	
	}
}

#endif // !PPU_STATES_H