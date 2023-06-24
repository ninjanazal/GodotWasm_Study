#include "inputController.h"

namespace TheBoy
{
	/// <summary>
	/// InputController class constructor
	/// </summary>
	/// <param name="ctrl">Target Emulator controller ref</param>
	InputController::InputController(EmulatorController* ctrl) : emulCtrl(ctrl) {
		states = new InputStates();

	}
	/// <summary>
	/// Class destructor
	/// </summary>
	InputController::~InputController() {
		delete states;
	}
	/// <summary>
	/// Get if it's currently selecting a button
	/// </summary>
	/// <returns>Button selected</returns>
	bool InputController::selectedButton() {
		return _buttonSelected;
	}
	/// <summary>
	/// Get if it's currently selecting a direction
	/// </summary>
	/// <returns>Direction selected</returns>
	bool InputController::selectedDirection() {
		return _directionSelected;
	}
	/// <summary>
	/// Defines a new button state 
	/// </summary>
	/// <param name="val">New state value</param>
	void InputController::setSelection(bit8 val) {
		_buttonSelected = static_cast<bool>(val & 0x20); //(0b 0010 0000)
		_directionSelected = static_cast<bool>(val & 0x10); // (ob 0001 0000)
	}
	/// <summary>
	/// Get a pointer to the current input state object
	/// </summary>
	/// <returns>Current state object pointer</returns>
	InputStates* InputController::getState() {
		return states;
	}
	/// <summary>
	/// Get the inputOutput result
	/// </summary>
	/// <returns>Input output values</returns>
	bit8 InputController::getOutput() {
		bit8 out = 0xCF;

		if (!selectedButton()) {
			if (states->start) { out &= ~(1 << 3); }
			if (states->select) { out &= ~(1 << 2); }
			if (states->a) { out &= ~(1 << 0); }
			if (states->b) { out &= ~(1 << 1); }
		}

		if (!selectedDirection()) {
			if (states->left) { out &= ~(1 << 1); }
			if (states->right) { out &= ~(1 << 0); }
			if (states->up) { out &= ~(1 << 2); }
			if (states->down) { out &= ~(1 << 3); }
		}
		return out;
	}
}