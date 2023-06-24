#pragma once
#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include "emulatorController.h"

namespace TheBoy
{
	/// <summary>
	/// Defined structure for holding the current inputState
	/// </summary>
	typedef struct InputStates {
		bool start;
		bool select;
		bool a;
		bool b;
		bool up;
		bool down;
		bool left;
		bool right;

		/// <summary>
		/// Initialize the state values
		/// </summary>
		void init() {

		}
	} InputStates;

	/// <summary>
	/// Input manager class
	/// </summary>
	class InputController
	{
		/*
		* FF00 - P1/JOYP - Joypad (R/W)
		*	The eight Game Boy action/direction buttons are arranged as a 2x4 matrix.
		*	Select either action or direction buttons by writing to this register, 
		*	then read out the bits 0-3.
		*		Bit 7 - Not used
		*		Bit 6 - Not used
		*		Bit 5 - P15 Select Action buttons    (0=Select)
		*		Bit 4 - P14 Select Direction buttons (0=Select)
		*		Bit 3 - P13 Input: Down  or Start    (0=Pressed) (Read Only)
		*		Bit 2 - P12 Input: Up    or Select   (0=Pressed) (Read Only)
		*		Bit 1 - P11 Input: Left  or B        (0=Pressed) (Read Only)
		*		Bit 0 - P10 Input: Right or A        (0=Pressed) (Read Only)
		*
		**/


	public:
		/// <summary>
		/// InputController class constructor
		/// </summary>
		/// <param name="ctrl">Target Emulator controller ref</param>
		InputController(EmulatorController* ctrl);

		/// <summary>
		/// Class destructor
		/// </summary>
		~InputController();

		/// <summary>
		/// Get if it's currently selecting a button
		/// </summary>
		/// <returns>Button selected</returns>
		bool selectedButton();

		/// <summary>
		/// Get if it's currently selecting a direction
		/// </summary>
		/// <returns>Direction selected</returns>
		bool selectedDirection();

		/// <summary>
		/// Defines a new button state 
		/// </summary>
		/// <param name="val">New state value</param>
		void setSelection(bit8 val);

		/// <summary>
		/// Get a pointer to the current input state object
		/// </summary>
		/// <returns>Current state object pointer</returns>
		InputStates* getState();

		/// <summary>
		/// Get the inputOutput result
		/// </summary>
		/// <returns>Input output values</returns>
		bit8 getOutput();
	private:

		/// <summary>
		/// Pointer to the target emulator controller
		/// </summary>
		EmulatorController* emulCtrl;

		/// <summary>
		/// Input controller internal reference
		/// </summary>
		InputStates* states;

		/// <summary>
		/// Select Action buttons
		/// </summary>
		bool _buttonSelected = false;

		/// <summary>
		/// Select Direction buttons
		/// </summary>
		bool _directionSelected = false;
	};
}

#endif // !INPUTCONTROLLER_H
