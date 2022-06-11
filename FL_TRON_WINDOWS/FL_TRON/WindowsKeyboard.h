#ifndef _WINDOWS_KEYBOARD_H_
#define _WINDOWS_KEYBOARD_H_

#include <Windows.h>

#include "SystemKeyboard.h"

class WindowsKeyboard : public SystemKeyboard
{
	const bool GetKeyState(int key) override;
};

#endif // !_WINDOWS_KEYBOARD_H_