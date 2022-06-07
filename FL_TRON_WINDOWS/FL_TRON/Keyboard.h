#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "InputHandler.h"
#include "SystemKeyboard.h"

#include <stdexcept>

//keyboard size set to 255 - probably all of keyCodes
#define KEYBOARD_SIZE 0xFF

class Keyboard : public InputHandler
{
public:
	Keyboard();
	~Keyboard() = default;
	void SetSystemKeyboard(SystemKeyboardPtr systemKeyboardPtr);

	const bool GetKeyDown(int keyCode) const override;
	const bool GetKeyUp(int keyCode) const override;
	const bool GetKeyPressed(int keyCode) override;
	const bool GetKeyReleased(int keyCode) override;

private:
	const bool isOutOfRange(int keyCode) const;

	SystemKeyboardPtr systemKeyboardPtr;
	bool lastKeyState[KEYBOARD_SIZE] = {};
};

#endif // !_KEYBOARD_H_